# Blenderマテリアルエクスポートガイド

このエンジンは、Blenderのマテリアル設定を正確に描画できるように拡張されました。

## 対応しているマテリアルパラメータ

### 基本パラメータ
- **Ka** (Ambient Color) - 環境光色
- **Kd** (Diffuse Color) - 拡散反射色
- **Ks** (Specular Color) - 鏡面反射色
- **Ke** (Emission Color) - 自己発光色
- **Ns** (Shininess) - 光沢度（0-1000）
- **d** (Opacity) - 不透明度（0.0-1.0）
- **Ni** (IOR) - 屈折率

### PBRパラメータ（Principled BSDF対応）
- **Pr** (Roughness) - ラフネス（0.0-1.0）
- **Pm** (Metallic) - メタリック度（0.0-1.0）

### テクスチャマップ
- **map_Kd** - ディフューズテクスチャ
- **map_Ks** - スペキュラテクスチャ
- **map_Bump** / **map_Kn** - 法線マップ
- **map_Pr** - ラフネスマップ
- **map_Pm** - メタリックマップ

## Blenderからのエクスポート手順

### 1. マテリアル設定
1. Blenderで3Dモデルを開く
2. Shading タブに切り替える
3. Principled BSDF シェーダーを使用する
4. 以下のパラメータを設定：
   - Base Color - 基本色
   - Metallic - 金属性（0-1）
   - Roughness - 粗さ（0-1）
   - IOR - 屈折率
   - Emission - 発光色と強度

### 2. エクスポート設定
1. File > Export > Wavefront (.obj) を選択
2. 以下の設定を確認：
   - ✓ Write Materials（マテリアルを書き出す）
   - ✓ Triangulate Faces（面を三角形化）
   - Forward: -Z Forward
   - Up: Y Up
3. Pathモード：「Strip Path」を選択（相対パスで出力）

### 3. MTLファイルの確認
エクスポート後、.mtlファイルに以下の項目が含まれていることを確認：
```mtl
# Blender MTL File
newmtl MaterialName
Ka 0.1 0.1 0.1      # 環境光
Kd 0.8 0.8 0.8      # 拡散反射
Ks 0.5 0.5 0.5      # 鏡面反射
Ke 0.0 0.0 0.0      # 発光
Ns 250              # 光沢度
d 1.0               # 不透明度
Ni 1.45             # 屈折率
Pr 0.5              # ラフネス（PBR）
Pm 0.0              # メタリック（PBR）
map_Kd texture.png  # ディフューズテクスチャ
```

## トラブルシューティング

### マテリアルが反映されない場合
1. MTLファイルがOBJファイルと同じディレクトリにあることを確認
2. OBJファイル内に `mtllib` 行があることを確認
3. テクスチャファイルのパスが正しいことを確認

### PBRパラメータが出力されない場合
Blenderの標準エクスポーターはPBRパラメータ（Pr, Pm）を出力しない場合があります。
その場合は、MTLファイルを手動で編集して追加してください。

### 推奨設定
- Roughness: 0.5（デフォルト）
- Metallic: 0.0（非金属）/ 1.0（金属）
- IOR: 1.45（プラスチック）/ 2.42（ダイヤモンド）

## シェーダーでの描画
このエンジンは以下の照明モデルをサポートしています：
- Lambert拡散反射
- Blinn-Phong鏡面反射
- 簡易PBR（メタリック・ラフネスワークフロー）
- 環境光と自己発光

マテリアルパラメータは自動的にシェーダーに渡され、Blenderでの見た目に近い描画が実現されます。