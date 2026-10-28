# SquashBreaker

DirectX 12で制作した、3Dアクションゲームのポートフォリオ作品です。  
※メイン成果物になります。

> **このリポジトリについて**  
> GitHub上でソースコードを確認するためのリポジトリです。  

## 作品概要

| 項目 | 内容 |
| --- | --- |
| 作品名 | SquashBreaker |
| ジャンル | 3Dアクション |
| 開発環境 | Visual Studio 2022 / C++ |
| グラフィックスAPI | DirectX 12 |
| 対応プラットフォーム | Windows |
| ビルド対象 | x64 |

## 見ていただきたいポイント

- **DirectX 12の描画基盤**
  - 低レベルな描画部分から作成し理解を深めるため、
  - デバイス、リソース、パイプラインの生成から描画を行っています。
  - ※背景画像や文字描画はSpriteBatch/SpriteFont使用
- **ゲーム本体と共通機能の分離**
  - ファイルの管理がしやすくなるように、
  - `FrameworkDirectX`、`Modules`、`MyLibrary`、`SquashBreaker`に役割を分け、描画基盤・共通ゲーム機能・ユーティリティ・ゲーム固有処理を整理しています。
- **コンポーネント指向のゲームオブジェクト**
  - ゲームオブジェクトと移動、描画、衝突などの処理をコンポーネントに分離しています。静的なパラメータも分離しました。(※ECSではありません)
- **パラメータ管理**
  - ハードコーディングを防ぎ、変更しやすくするため、
  - ゲームオブジェクト、衝突レイヤーなどの静的なパラメータを
    Excelで作成・JSON出力して変更できるようにしています。
  - ※VBAはAIで作成
 - **ゲームサイクルの分離**
  - 処理のタイミングが分かりやすいように、
  - `start`、`preUpdate`、`update`、`postUpdate`、当たり判定・ヒット応答、`prerender`、`render`、`postRender`の順に処理を分けています。


## システム構成

```text
SquashBreaker
  |    └─ ゲーム本体、ゲーム固有のオブジェクト・レベル・画面遷移
  ├─ Modules
  │    └─ ゲームオブジェクト、コンポーネント、マネージャ、入力、UI
  ├─ FrameworkDirectX
  │    └─ DirectX 12の初期化、GPUリソース、描画、モデル・アニメーション
  └─ MyLibrary
       └─ ファイル操作、ログ、文字列変換、バイナリ読み込みなどの共通処理
```

## コード詳細
詳細なファイル構成や処理の確認順は、
- [Framework_DirectX/PROJECT_GUID.txt](Framework_DirectX/PROJECT_GUID.txt) をご覧ください。

## ビルド・実行
### 必要な環境

- Windows
- Visual Studio 2022
- Desktop development with C++ workload
- Windows SDK
- DirectX 12を利用できるGPU

### Visual Studioからビルドする場合

1. `Framework_DirectX/project/SquashBreaker.sln`をVisual Studio 2022で開きます。
2. 構成を`Debug`、プラットフォームを`x64`に設定します。
3. `SquashBreaker`をスタートアッププロジェクトに設定してビルドします。
4. 開発中の動作確認では、`project/x64/Debug/bin`にあるアセットを削除せず、
   実行ファイルから参照できる状態で起動します。

### MSBuildからビルドする場合

```powershell
msbuild .\Framework_DirectX\SquashBreaker\SquashBreaker.vcxproj `
  /p:Configuration=Debug /p:Platform=x64
```

アセットは`.bin`ファイルにまとめられており、ビルド出力の`bin`ディレクトリに
配置されています。元のアセットは含まれていないため、実行時に必要な`.bin`ファイルを削除しないでください。

### ビルド済みゲームの配布

ビルド済みのゲームのみダウンロードしたい場合は、
Releaseページからダウンロードしてください。

ダウンロード後、SQUASH_BREAKER.exeをクリックし、遊ぶことができます。

###　参考文献 
- DirectX3D 実践ガイド|著） Pocol
- DirectX12 Programing Vol.2,3,4 |著) 山田 英伸
- ゲームプログラマになる前に覚えておきたい技術 |著) 平山 尚
- ゲームエンジンアーキテクスチャ |著) ジェイソン・グレゴリー

