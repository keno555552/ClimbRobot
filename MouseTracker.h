#pragma once
#include "KamataEngine.h"
using namespace KamataEngine;
#include "Player.h"
#include "myMathForAL.h"

class MouseTracker {
public:
	void Initialize(Camera* camera, Player* player);
	void Update();
	void Render();

public:
	Vector2 GetOutput() { return {worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1]}; }

private:
	Input* input_ = nullptr;
	Vector3 mousePositionTarget_ = {};

private:
	Model* model_ = nullptr;
	Model* model_2_ = nullptr;
	Camera* camera_ = nullptr;
	WorldTransform worldTransform_ = {};
	Player* player_ = nullptr;

	float near_ = 0.1f;
	float far_ = 1000.0f;
};
