import bpy

# 既存のオブジェクトをすべて削除
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete()

# 新しいキューブを作成
bpy.ops.mesh.primitive_cube_add(
    size=2,  # キューブのサイズ（各辺の長さ）
    location=(0, 0, 0),  # 位置（X, Y, Z）
    rotation=(0, 0, 0)   # 回転（X, Y, Z）ラジアン単位
)

# 作成したキューブを取得
cube = bpy.context.active_object

# キューブの名前を設定
cube.name = "MyGameCube"

# マテリアルを作成
material = bpy.data.materials.new(name="CubeMaterial")
material.use_nodes = True

# マテリアルのベースカラーを設定（青色）
bsdf = material.node_tree.nodes["Principled BSDF"]
bsdf.inputs[0].default_value = (0.2, 0.5, 0.8, 1.0)  # RGBA

# キューブにマテリアルを適用
cube.data.materials.append(material)

# キューブをOBJ形式でエクスポート
export_path = "/mnt/c/Users/ryuto/OneDrive/ドキュメント/GitHub/MyEngineGame/Resources/Models/generated_cube/cube.obj"

# エクスポートディレクトリが存在しない場合は作成
import os
os.makedirs(os.path.dirname(export_path), exist_ok=True)

# OBJファイルとしてエクスポート
bpy.ops.export_scene.obj(
    filepath=export_path,
    use_selection=True,
    use_materials=True,
    use_triangles=True,
    use_normals=True,
    use_uvs=True
)

print(f"キューブが正常に作成され、{export_path}にエクスポートされました。")