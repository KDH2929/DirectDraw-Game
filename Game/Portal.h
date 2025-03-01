#pragma once
#include "GameObject.h"
#include "PortalAnim.h"
#include "SceneTypes.h"


// 전방 선언
class CImageData;
class SpriteManager;


class Portal : public GameObject {
public:

    Portal(CImageData* pPortalImgData, float startX, float startY);
    virtual ~Portal();

    virtual void Update(float deltaTime) override;
    virtual void Render(CDDrawDevice* pDevice) override;

    virtual void OnCollision(const CollisionInfo& collisionInfo) override;

    void SetNextSceneType(SceneType Scene);
    const SceneType GetNextSceneType();


private:
    void InitSpriteManager(CImageData* pPortalImgData, int frameWidth, int frameHeight);
    void InitAnimation();
    void InitCollider(int x, int y, int width, int height);

    SpriteManager* m_spriteManager;
    PortalAnim* m_anim;
    bool m_isPortalActivated = false;
    SceneType m_nextScene = SceneType::Title;

};