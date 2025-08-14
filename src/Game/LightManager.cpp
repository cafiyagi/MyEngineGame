#include "LightManager.h"
#include "Camera.h"
#include <cmath>
#include <random>

LightManager::LightManager() {
}

LightManager::~LightManager() {
}

void LightManager::Initialize() {
    // ディレクショナルライトの初期設定（完全に無効）
    directionalLight_.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    directionalLight_.direction = { 0.0f, -1.0f, 0.5f };
    directionalLight_.intensity = 0.0f;  // 完全に無効（懐中電灯のみの環境）
    
    // スポットライトの初期設定（リアルな懐中電灯風）
    spotLight_.color = { 1.0f, 0.95f, 0.85f, 1.0f };  // わずかに暖色系の白色LED
    spotLight_.position = { 0.0f, 5.0f, -2.0f };
    spotLight_.intensity = 2.8f;  // ホラーゲーム向けに弱め
    spotLight_.direction = { 0.0f, -1.0f, 0.3f };
    spotLight_.innerCone = cosf(3.0f * 3.14159265f / 180.0f);   // 内側3度（とても狭い明るい中心部）
    spotLight_.attenuation = { 1.0f, 0.35f, 0.44f };  // さらに強い減衰で影を強調
    spotLight_.outerCone = cosf(25.0f * 3.14159265f / 180.0f);  // 外側25度（狭めの光範囲）
    
    // 初期値をバックアップ
    dirLightIntensityBackup_ = directionalLight_.intensity;
    spotLightIntensityBackup_ = spotLight_.intensity;
    
    // ライトの初期位置を設定
    currentLightPosition_ = spotLight_.position;
    currentLightDirection_ = spotLight_.direction;
    
    // デフォルトでディレクショナルライトを無効化（スポットライトのみ使用）
    enableDirectionalLight_ = false;
}

void LightManager::Update() {
    UpdateLightIntensity();
}

void LightManager::DrawImGui() {
    if (!showDebugWindow_) return;
    
    ImGui::Begin("Lighting Settings (F \u30ad\u30fc\u3067\u8868\u793a\u5207\u66ff)");
    
    // ディレクショナルライト設定
    ImGui::Separator();
    ImGui::Text("Directional Light");
    ImGui::Checkbox("Enable Directional Light", &enableDirectionalLight_);
    if (enableDirectionalLight_) {
        ImGui::ColorEdit3("Dir Light Color", &directionalLight_.color.x);
        ImGui::SliderFloat3("Dir Light Direction", &directionalLight_.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Dir Light Intensity", &directionalLight_.intensity, 0.0f, 3.0f);
        
        NormalizeDirectionalLightDirection();
    }
    
    // スポットライト設定
    ImGui::Separator();
    ImGui::Text("Spot Light");
    ImGui::Checkbox("Enable Spot Light", &enableSpotLight_);
    if (enableSpotLight_) {
        ImGui::ColorEdit3("Spot Light Color", &spotLight_.color.x);
        ImGui::DragFloat3("Spot Light Position", &spotLight_.position.x, 0.1f);
        ImGui::SliderFloat3("Spot Light Direction", &spotLight_.direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat("Spot Light Intensity", &spotLight_.intensity, 0.0f, 5.0f);
        
        // コーン角度（度数で表示）
        float innerAngle = acosf(spotLight_.innerCone) * 180.0f / 3.14159265f;
        float outerAngle = acosf(spotLight_.outerCone) * 180.0f / 3.14159265f;
        
        if (ImGui::SliderFloat("Inner Cone Angle", &innerAngle, 0.0f, 90.0f)) {
            spotLight_.innerCone = cosf(innerAngle * 3.14159265f / 180.0f);
        }
        if (ImGui::SliderFloat("Outer Cone Angle", &outerAngle, 0.0f, 90.0f)) {
            spotLight_.outerCone = cosf(outerAngle * 3.14159265f / 180.0f);
        }
        
        // 減衰パラメータ
        ImGui::Text("Attenuation");
        ImGui::SliderFloat("Constant", &spotLight_.attenuation.x, 0.0f, 2.0f);
        ImGui::SliderFloat("Linear", &spotLight_.attenuation.y, 0.0f, 0.5f);
        ImGui::SliderFloat("Quadratic", &spotLight_.attenuation.z, 0.0f, 0.1f);
    }
    
    // 現在のライト値の表示
    ImGui::Separator();
    ImGui::Text("\u73fe\u5728\u306e\u30e9\u30a4\u30c8\u5024:");
    ImGui::Text("Directional Light:");
    ImGui::Text("  \u5f37\u5ea6: %.2f", directionalLight_.intensity);
    ImGui::Text("  \u65b9\u5411: (%.2f, %.2f, %.2f)", 
        directionalLight_.direction.x, directionalLight_.direction.y, directionalLight_.direction.z);
    ImGui::Text("Spot Light:");
    ImGui::Text("  \u5f37\u5ea6: %.2f", spotLight_.intensity);
    ImGui::Text("  \u4f4d\u7f6e: (%.2f, %.2f, %.2f)", 
        spotLight_.position.x, spotLight_.position.y, spotLight_.position.z);
    
    ImGui::End();
}

void LightManager::NormalizeDirectionalLightDirection() {
    float dirLength = sqrtf(
        directionalLight_.direction.x * directionalLight_.direction.x +
        directionalLight_.direction.y * directionalLight_.direction.y +
        directionalLight_.direction.z * directionalLight_.direction.z
    );
    
    if (dirLength > 0.001f) {
        directionalLight_.direction.x /= dirLength;
        directionalLight_.direction.y /= dirLength;
        directionalLight_.direction.z /= dirLength;
    }
}

void LightManager::UpdateLightIntensity() {
    // ディレクショナルライトの強度管理
    if (!enableDirectionalLight_) {
        if (directionalLight_.intensity > 0.0f) {
            dirLightIntensityBackup_ = directionalLight_.intensity;
        }
        directionalLight_.intensity = 0.0f;
    } else if (directionalLight_.intensity == 0.0f) {
        directionalLight_.intensity = dirLightIntensityBackup_;
    }
    
    // スポットライトの強度管理
    if (!enableSpotLight_) {
        if (spotLight_.intensity > 0.0f) {
            spotLightIntensityBackup_ = spotLight_.intensity;
        }
        spotLight_.intensity = 0.0f;
    } else if (spotLight_.intensity == 0.0f) {
        spotLight_.intensity = spotLightIntensityBackup_;
    }
}

void LightManager::UpdateSpotLightPosition(const Vector3& playerPos, float playerRotationY) {
    if (!followPlayer_) return;
    
    // プレイヤーの位置から少し上にスポットライトを配置
    spotLight_.position = playerPos;
    spotLight_.position.y += 2.5f; // プレイヤーの頭上2.5ユニット
    
    // プレイヤーの前方にライトを向ける
    float cosY = cosf(playerRotationY);
    float sinY = sinf(playerRotationY);
    
    // 前方向を計算（プレイヤーの向いている方向）
    spotLight_.direction.x = sinY;
    spotLight_.direction.y = -0.8f; // 少し下向き
    spotLight_.direction.z = cosY;
    
    // 方向ベクトルを正規化
    float dirLength = sqrtf(
        spotLight_.direction.x * spotLight_.direction.x +
        spotLight_.direction.y * spotLight_.direction.y +
        spotLight_.direction.z * spotLight_.direction.z
    );
    
    if (dirLength > 0.001f) {
        spotLight_.direction.x /= dirLength;
        spotLight_.direction.y /= dirLength;
        spotLight_.direction.z /= dirLength;
    }
}

void LightManager::UpdateSpotLightForFirstPerson(Camera* camera) {
    if (!camera || !followPlayer_) return;
    
    // デルタタイム（仮に16ms = 60FPS）
    const float deltaTime = 0.016f;
    
    // カメラの基準位置（頭の揺れを除いた位置）と向きを取得
    Vector3 targetPos = camera->GetBasePosition();
    Vector3 targetDirection = camera->GetForwardVector();
    
    // ライト位置の滑らかな追従（現実的な慣性）
    const float positionSmoothness = 4.5f;  // より現実的な遅延（手持ちの懐中電灯の慣性）
    currentLightPosition_.x += (targetPos.x - currentLightPosition_.x) * positionSmoothness * deltaTime;
    currentLightPosition_.y += (targetPos.y - currentLightPosition_.y) * positionSmoothness * deltaTime;
    currentLightPosition_.z += (targetPos.z - currentLightPosition_.z) * positionSmoothness * deltaTime;
    
    // ライト方向の滑らかな追従（より強い慣性）
    const float directionSmoothness = 3.5f;  // 懐中電灯を持った腕の慣性を再現
    currentLightDirection_.x += (targetDirection.x - currentLightDirection_.x) * directionSmoothness * deltaTime;
    currentLightDirection_.y += (targetDirection.y - currentLightDirection_.y) * directionSmoothness * deltaTime;
    currentLightDirection_.z += (targetDirection.z - currentLightDirection_.z) * directionSmoothness * deltaTime;
    
    // 方向ベクトルを正規化
    float dirLength = sqrtf(
        currentLightDirection_.x * currentLightDirection_.x +
        currentLightDirection_.y * currentLightDirection_.y +
        currentLightDirection_.z * currentLightDirection_.z
    );
    
    if (dirLength > 0.001f) {
        currentLightDirection_.x /= dirLength;
        currentLightDirection_.y /= dirLength;
        currentLightDirection_.z /= dirLength;
    }
    
    // スポットライトに適用
    spotLight_.position = currentLightPosition_;
    spotLight_.direction = currentLightDirection_;
    
    // ホラー演出：電池切れ風のフリッカー効果
    flickerTimer_ += deltaTime;
    
    // ランダムジェネレータ
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> smallFlicker(0.75f, 1.0f);  // 小さなちらつきをより頻繁に
    static std::uniform_real_distribution<float> bigFlicker(0.2f, 0.4f);     // より顕著な電池切れ効果
    static std::uniform_real_distribution<float> flickerChance(0.0f, 1.0f);
    
    // 基本的な明るさ（常に微かに揺れる）
    float intensity = baseIntensity_ * (0.90f + 0.10f * smallFlicker(gen));
    
    // より頻繁に大きくフリッカー（電池切れ感）
    if (flickerTimer_ - lastFlickerTime_ > 2.0f) {  // 2秒ごとにチェック
        if (flickerChance(gen) < 0.35f) {  // 35%の確率で明度低下
            intensity *= bigFlicker(gen);
            lastFlickerTime_ = flickerTimer_;
        }
    }
    
    // 瞬きする頻度を増やす（懐中電灯特有の接触不良）
    if (flickerChance(gen) < 0.008f) {  // 0.8%の確率（4倍に増加）
        intensity *= 0.05f;  // より暗くなる
    }
    
    spotLight_.intensity = intensity;
    
    // 懐中電灯の色温度（わずかに暖色系の白色LED）
    float colorVariation = 0.85f + 0.15f * smallFlicker(gen);  // 色の変化も増やす
    spotLight_.color = { 1.0f, 0.95f * colorVariation, 0.85f * colorVariation, 1.0f };  // LED風
    
    // リアルな懐中電灯の光の設定（狭く集中した光）
    spotLight_.innerCone = cosf(3.0f * 3.14159265f / 180.0f);   // 内側3度（非常に狭い明るい中心部）
    spotLight_.outerCone = cosf(25.0f * 3.14159265f / 180.0f);  // 外側25度（狭めの光範囲）
    
    // 懐中電灯の減衰特性（より強い減衰で影を強調）
    spotLight_.attenuation = { 1.0f, 0.35f, 0.44f };  // 距離による非常に強い減衰
}