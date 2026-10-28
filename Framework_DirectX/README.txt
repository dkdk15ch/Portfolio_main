DirectX 12 / SquashBreaker プロジェクト案内

==================================================
1. ビルド・実行
==================================================

推奨環境
  Visual Studio 2022
  Platform Toolset: v143
  Windows SDK
  DirectX 12対応のWindows環境

Visual Studio
  project/SquashBreaker.slnを開き、ビルドします。
  SquashBreakerがメインの実行プロジェクトです。

実行時の注意
　project/x64/Debug・Relase/bin　にアセットをまとめた.binファイルが
入っています。元のアセット含まれないため削除しないようにお願いします。


==================================================
2. プロジェクト全体
==================================================

・実行ファイルの中心
  SquashBreaker/
    main.cpp
      ゲームアプリケーションのエントリポイントです。
    SquashBreaker.h / SquashBreaker.cpp
      ゲーム全体を管理するクラスです。
      初期化、終了処理、毎フレーム更新、描画、Windowsメッセージ処理を確認できます。

・プロジェクトの依存関係
  SquashBreaker
    ├─ Modules
    ├─ FrameworkDirectX
    └─ MyLibrary

・大まかな処理の流れ
  main.cpp
    -> SquashBreaker
      	-> GraphicContext（DirectX 12の描画環境）
      	-> Manager群生成（ゲーム全体の管理）
	-> Sequenceでゲーム遷移
	-> Levelで各ゲームオブジェクトのゲームサイクルを回す。(描画はRenderManager)
		
・ゲームサイクルは、GameBaseクラス参照
	start(初回サイクル時のみ) -> preUpdate -> update -> postUpdate 
	-> 当たり判定 -> ヒット応答 
	->prerender -> render -> postRender

==================================================
以降、各プロジェクトの説明
==================================================

==================================================
3. FrameworkDirectX
==================================================

場所
  FrameworkDirectX/include/
    DirectX 12用クラスの宣言、共通型、描画関連のヘッダー
  FrameworkDirectX/src/
    上記クラスの実装

DirectX 12の初期化とフレーム処理
  App.h
    アプリケーション基底クラスです。
    ウィンドウ、メインループ、フレーム更新、描画の共通処理を確認できます。

  GraphicContext.h
    DirectX 12のデバイス、コマンドキュー、スワップチェーン、
    ディスクリプタプール、バックバッファ、深度バッファなどを管理します。
    描画環境の中心となるファイルです。

コマンド実行とGPU同期
  CommandList.h
    コマンドリストの作成、リセット、取得、終了処理
  Fence.h   
    CPUとGPUの同期
  DescriptorPool.h
    RTV、DSV、SRV、UAVなどのディスクリプタ管理

GPUリソース
  ColorTarget.h    		カラーバッファ、レンダーターゲット
  DepthTarget.h     		深度バッファ、深度ステンシル
  VertexBuffer.h    		頂点バッファ
  IndexBuffer.h     		インデックスバッファ
  ConstantBuffer.h   		定数バッファ
  Texture.h     		テクスチャの読み込み・管理
  UnorderAccessTarget.h     	UAVとして使用するリソース

モデル・マテリアル・アニメーション
  iMesh.h     			メッシュデータと描画
  Material.h     		マテリアル、テクスチャ、定数バッファの管理
  include/Animation/		モデル・ボーン・アニメーションの管理
  include/Converter/		モデルデータなどの変換処理(不採用)

描画補助
  include/DrawHelper/  		画像などの2D描画補助
  include/Util/		    	JSON読み込みや数学・文字列などの補助処理

シェーダ・外部ライブラリ
  FrameworkDirectX/include/d3dx12.h	Microsoft提供のD3D12ヘルパーです。変更なし。
  FrameworkDirectX/external/		Assimp、FBX SDKなどの外部ライブラリ関連ファイルです。


==================================================
4. Modules
==================================================

場所
  Modules/include/
    ゲームで共通利用するクラスの宣言
  Modules/src/
    共通クラスの実装

ゲームオブジェクトとコンポーネント
  include/GameObject/	   	ゲームオブジェクト、プリミティブ、カメラなど
  include/Component/		Transform、移動、衝突、描画などのコンポーネント
  include/Component/Render/	メッシュ描画、インスタンシング描画
  include/Component/Collider/   コライダーと衝突判定

ゲーム管理
  include/Manager/		マネージャクラス
  include/Level/		レベルの共通処理
  include/Sequence/		親子関係を持つシーケンス・画面遷移の共通処理
  include/State/		ステートマシンの共通処理

入力・UI・データ読み込み
  include/InOutput/		Pad、Audioなどの入力・出力
  include/UI/			Canvas、HUD、UIImageなどのUI
  include/Util/			Loader、フォント、JSON、計算ヘルパなど


==================================================
5. SquashBreakerのゲーム本体
==================================================

ゲーム全体
  SquashBreaker/SquashBreaker.h / SquashBreaker.cpp
    Appを継承したゲーム本体です。
    各Managerを作成し、ゲームの初期化・更新・描画をまとめます。

Manager
  SquashBreaker/Manager/
    AssetManager	モデル・テクスチャなどのアセット管理
    DrawManager		ルートシグネチャ、描画用リソース、描画設定
    LightManager	ライト用の定数バッファ
    SquashGameManager   ゲーム進行全体の管理

ゲームオブジェクト
  SquashBreaker/GameObject/
    Player      	プレイヤー
    Enemy/	      	敵キャラクター
    Ball		ボール
    Block、BlockWall	ブロックとブロック配置
    Item/		アイテム
    Param/	　      各オブジェクトのパラメータ
    State/	　      プレイヤー・敵の状態管理
    Strategy/　		衝突時の振る舞い
    Test/	　      モデル読み込みなどのテスト用処理

各オブジェクトの移動処理
  SquashBreaker/Component/Mover/
    PlayerMovementComp
    BallMovementComp
    BatMovementComp
      各オブジェクトの移動・操作処理

レベル
  SquashBreaker/Level/
    GameLevel		通常ゲームのレベル
    ClearLevel		クリア画面
    GameOverLevel	ゲームオーバー画面
    LoadTestLevel、TestScene、AnimationTestScene	読み込み・動作確認用のレベル

画面遷移
  SquashBreaker/Sequence/
    Title
    Ready
    GamePlay
    PlayGuid
    GameClear
    GameOver
      タイトルからゲーム開始、プレイ、クリア、ゲームオーバーまでの画面単位の処理

UI・共通データ
  SquashBreaker/UI/				固有UI
  SquashBreaker/Global.h / Global.cpp		ゲーム全体で共有する定義
  SquashBreaker/AdditionalConstantBuffer.h 	ゲーム固有の定数バッファ定義

シェーダ
　HLSL/  
    SquashBreakerで使用するピクセルシェーダ等です。


==================================================
6. MyLibrary
==================================================

MyLibraryは、FrameworkDirectXやModules、SquashBreakerから利用される
共通ユーティリティ層です。

場所
  MyLibrary/include/
 主な役割
  FileUtil.h 		    ファイルパス、拡張子、ファイル検索など
  Logger.h 		    ログ出力
  FileLog.h 		    ファイルログ
  StringHelper.h 	    文字列変換
  Delegate.h		    Delegate、イベント通知
  PointerHelper.h	    ポインタ・スマートポインタ関連のラッパークラス
  binFileLoader.h 	    バイナリファイル読み込み
