#pragma once

#include "Scene.h"
#include "TileMap2D.h"
#include "Character.h"
#include "Monster.h"
#include "Portal.h"
#include "DeathOverlapBox.h"
#include <vector>
#include <windows.h>
#include <memory>

// 전방 선언
class CTGAImage;
class CImageData;
class CDDrawDevice;


class Stage2Scene : public Scene
{
public:
    Stage2Scene(CDDrawDevice* pDrawDevice, HWND hWnd);
    virtual ~Stage2Scene();

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    virtual void ResetInterpolation() override;
    virtual void InterpolatePosition(float alpha) override;

    virtual const SceneType GetSceneType() override;

private:
    void UpdateCamera(int screenWidth, int screenHeight);

private:
    // DirectDraw 장치 및 윈도우 핸들
    CDDrawDevice* m_pDrawDevice;
    HWND m_hWnd;

    // 이미지 리소스들
    CImageData* m_pPlayerImgData = nullptr;
    CImageData* m_pCentipedeImgData = nullptr;
    CTGAImage* m_pBackgroundImage = nullptr;

    // 해당 클래스만 ImgData를 소유
    // 동적할당해제를 안해도 알아서 객체가 죽으면 할당해제를 해줌
    std::unique_ptr<CImageData> m_pPortalImgData = nullptr;

    // 타일맵
    TileMap2D* m_TileMap = nullptr;

    // 배경 이미지 크기
    int m_bgWidth = 0;
    int m_bgHeight = 0;

    // 카메라 처리용 변수들
    int m_cameraOffsetX = 0;
    int m_cameraOffsetY = 0;
    int m_playerRenderX = 0;
    int m_playerRenderY = 0;
    int m_backgroundPosX = 0;
    int m_backgroundPosY = 0;


    // 게임 오브젝트:
    Character* m_pPlayer = nullptr;
    std::unique_ptr<Portal> m_pPortal = nullptr;
    std::vector<Monster*> m_vMonsters;
    std::unique_ptr<DeathOverlapBox> m_pDeathOverlapBox = nullptr;

};
