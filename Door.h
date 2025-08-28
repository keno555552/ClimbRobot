#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
#include "crashDecision.h"
#include "myMathForAL.h"

class GameScene;
class Door {
public:
	void Initialize(const Camera* camera, const Vector3& position);
	/// 更新
	void Update();
	/// 描画
	void Render();

	Vector3 GetWorldPosition();
	AABB GetAABB();

private:
	/// 単独モデル
	Model* model_ = nullptr;
	/// ワールドトランスフォーム
	WorldTransform worldTransform_;
	/// カメラ
	const Camera* camera_ = nullptr;
	/// ゲームシーン
	GameScene* gameScene_ = nullptr;
};
