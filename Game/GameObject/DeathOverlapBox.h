#pragma once
#include "GameObject.h"
#include "SceneTypes.h"



class DeathOverlapBox : public GameObject {
public:

    DeathOverlapBox(float startX, float startY, float width, float height);
    virtual ~DeathOverlapBox();

    virtual void Update(float deltaTime) override;
    virtual void Render(CDDrawDevice* pDevice) override;

    virtual void OnCollision(const CollisionInfo& collisionInfo) override;

    void SetNextSceneType(SceneType Scene);
    const SceneType GetNextSceneType();


private:
    void InitCollider(float x, float y, float width, float height);

    bool m_isDeathOverlapBoxActivated = false;
    SceneType m_nextScene = SceneType::Title;

};