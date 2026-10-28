Attribute VB_Name = "Module1"
Option Explicit

' ★共通の保護パスワード（パスワードなしの場合は "" のままでOK）
Private Const SHEET_PW As String = ""

'------------------------
' JSON出力メイン処理
'------------------------
Sub ExportSheetWithValueTypeToJSON()
    Dim ws As Worksheet
    Set ws = ActiveSheet ' 出力元の作業シートを固定

    Dim lastRow As Long, lastCol As Long
    ' ★修正ポイント：B列(2列目)基準で最終行を取得するように変更
    lastRow = ws.Cells(ws.Rows.Count, 2).End(xlUp).Row
    lastCol = ws.Cells(1, ws.Columns.Count).End(xlToLeft).Column

    Dim data As Collection
    Set data = New Collection

    Dim i As Long, j As Long

    ' 3列目以降 = C列～、各キャラ列
    For j = 3 To lastCol
        Dim dict As Object
        Set dict = CreateObject("Scripting.Dictionary")

        ' 1行目キャラ名（C1,D1,...列）を"name"に
        dict.Add "name", ws.Cells(1, j).Value

        ' 2行目以降ループ
        For i = 2 To lastRow
            Dim paramName As String
            paramName = ws.Cells(i, 2).Value ' B列(name列)
            Dim paramValue As Variant
            paramValue = ws.Cells(i, j).Value
            
            ' パラメータ名が存在していれば辞書に追加
            If paramName <> "" Then
                dict(paramName) = paramValue
            End If
        Next i

        data.Add dict
    Next j

    ' Collection→配列
    Dim arr() As Variant
    ReDim arr(1 To data.Count)
    For i = 1 To data.Count
        Set arr(i) = data(i)
    Next i

    Dim json As String
    json = JsonConverter.ConvertToJson(arr, 2)

    ' ファイル保存ダイアログ
    Dim fd As FileDialog
    Set fd = Application.FileDialog(msoFileDialogSaveAs)
    fd.Title = "保存する場所とファイル名を指定"

    ' ExportLogの記録ファイルを初期パスに
    Dim lastPath As String
    lastPath = GetLastExportPath(ws.Name)
    If lastPath <> "" Then
        fd.InitialFileName = lastPath
    Else
        fd.InitialFileName = "output.json"
    End If

    If fd.Show = -1 Then
        Dim fn As String
        fn = fd.SelectedItems(1)
        
        ' 拡張子を強引に.jsonに直す
        Dim dotPos As Long
        dotPos = InStrRev(fn, ".")
        If dotPos > 0 Then
            fn = Left(fn, dotPos - 1) & ".json"
        Else
            fn = fn & ".json"
        End If

        ' ファイルがある場合は確認
        If Dir(fn) <> "" Then
            Dim ans As VbMsgBoxResult
            ans = MsgBox("同名のファイルが既に存在します。上書きしますか？", vbYesNo + vbExclamation, "確認")
            If ans = vbNo Then Exit Sub
        End If

        ' ADODB.Stream で UTF-8 保存
        Dim fso As Object
        Set fso = CreateObject("ADODB.Stream")
        fso.Type = 2 'テキスト
        fso.Charset = "UTF-8"
        fso.Open
        fso.WriteText json
        fso.SaveToFile fn, 2 '上書き
        fso.Close

        ' --- 履歴記録（ExportLogシートの更新） ---
        Call AddExportLog(ws.Name, fn)

        ' --- 元の作業シートをアクティブに戻す ---
        ws.Activate

        ' --- ブックを保存 ---
        ThisWorkbook.Save

        MsgBox "保存しました：" & fn
    End If
End Sub

'------------------------
' ExportLogの「シート名」で最新ファイルパスを取得
'------------------------
Function GetLastExportPath(sheetName As String) As String
    On Error Resume Next
    Dim wsLog As Worksheet
    Set wsLog = Worksheets("ExportLog")
    If wsLog Is Nothing Then
        GetLastExportPath = ""
        Exit Function
    End If

    Dim lastRow As Long, i As Long
    lastRow = wsLog.Cells(wsLog.Rows.Count, 1).End(xlUp).Row

    ' シート内を下から上に検索して、最新一致のファイルパスを返す
    For i = lastRow To 2 Step -1
        If wsLog.Cells(i, 2).Value = sheetName Then ' 2列目：シート名
            GetLastExportPath = wsLog.Cells(i, 3).Value ' 3列目：ファイルパス
            Exit Function
        End If
    Next i
    GetLastExportPath = ""
End Function

'------------------------
' ExportLogに新規追記、同じシート名があれば上書き更新
'------------------------
Sub AddExportLog(sheetName As String, fileName As String)
    On Error Resume Next ' ログ記録エラーでメイン処理を止めないための安全策
    
    Dim wsLog As Worksheet
    Set wsLog = GetOrCreateWorksheet("ExportLog")

    ' 保護解除を試みる
    If SHEET_PW = "" Then
        wsLog.Unprotect
    Else
        wsLog.Unprotect Password:=SHEET_PW
    End If

    ' 1行目タイトル書き込み（なければ追記）
    If wsLog.Cells(1, 1).Value = "" Then
        wsLog.Cells(1, 1).Value = "日付"
        wsLog.Cells(1, 2).Value = "シート名"
        wsLog.Cells(1, 3).Value = "ファイル名"
    End If

    Dim lastRow As Long, i As Long
    Dim targetRow As Long
    targetRow = 0

    lastRow = wsLog.Cells(wsLog.Rows.Count, 1).End(xlUp).Row

    ' 既存の同じシート名の行を探す
    For i = 2 To lastRow
        If wsLog.Cells(i, 2).Value = sheetName Then
            targetRow = i
            Exit For
        End If
    Next i

    ' 見つからなかった場合は末尾に新しい行を指定
    If targetRow <= 1 Then
        targetRow = lastRow + 1
    End If
    If targetRow <= 1 Then targetRow = 2

    ' 値の書き込み・更新
    wsLog.Cells(targetRow, 1).Value = Date
    wsLog.Cells(targetRow, 2).Value = sheetName
    wsLog.Cells(targetRow, 3).Value = fileName

    ' マクロからの操作を許可した保護
    If SHEET_PW = "" Then
        wsLog.Protect UserInterfaceOnly:=True
    Else
        wsLog.Protect Password:=SHEET_PW, UserInterfaceOnly:=True
    End If
    On Error GoTo 0
End Sub

'------------------------
' シートなければ作成（作成後も画面表示を切り替えない）
'------------------------
Function GetOrCreateWorksheet(sheetName As String) As Worksheet
    Dim ws As Worksheet
    On Error Resume Next
    Set ws = ThisWorkbook.Worksheets(sheetName)
    On Error GoTo 0

    If Not ws Is Nothing Then
        Set GetOrCreateWorksheet = ws
    Else
        ' 新規作成（画面を切り替えずに作成）
        Application.ScreenUpdating = False
        Set ws = ThisWorkbook.Worksheets.Add(After:=ThisWorkbook.Worksheets(ThisWorkbook.Worksheets.Count))
        ws.Name = sheetName
        Application.ScreenUpdating = True
        Set GetOrCreateWorksheet = ws
    End If
End Function
