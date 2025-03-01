#pragma once
#include "SceneTypes.h"

class Scene {
public:
    virtual ~Scene() {}

    virtual void Initialize() = 0;
    virtual void Cleanup() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;

    // 보간 처리를 위한 API
    virtual void ResetInterpolation() = 0;
    virtual void InterpolatePosition(float alpha) = 0;

    virtual const SceneType GetSceneType() = 0;
};
