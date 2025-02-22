#pragma once
#include "DebugManager.h"
#include "MathUtils.h"

// 전방 선언
class CTGAImage;
class CImageData;
class Character;   // 플레이어 캐릭터
class CDDrawDevice;
class TileMap2D;

class CGame
{
public:
    CGame();
    ~CGame();

    BOOL Initialize(HWND hWnd);
    void Process();
    void Cleanup();

    void OnKeyDown(UINT nChar, UINT uiScanCode);
    void OnKeyUp(UINT nChar, UINT uiScanCode);
    BOOL OnSysKeyDown(UINT nChar, UINT uiScanCode, BOOL bAltKeyDown);
    void OnUpdateWindowSize();
    void OnUpdateWindowPos();
    BOOL CaptureBackBuffer(const char* szFileName);

private:
    void UpdateGameFrame(ULONGLONG currentTick);
    void UpdatePosition(float deltaTime, int screenWidth, int screenHeight);
    void UpdateCamera(int screenWidth, int screenHeight);
    void InterpolatePosition(float alpha);
    void ResetInterpolation();

    void DrawScene();


private:
    CDDrawDevice* m_pDrawDevice = nullptr;
    HWND m_hWnd = nullptr;

    DWORD m_dwGameFPS = 60;
    DWORD m_dwCurrentFPS = 0;
    float m_fTicksPerFrame = 0.0f;
    ULONGLONG m_prevFrameTick = 0;
    LARGE_INTEGER m_prevCounter = {};

    CImageData* m_pPlayerImgData = nullptr;
    CTGAImage* m_pBackgroundImage = nullptr;

    // 카메라 처리
    int m_cameraOffsetX = 0;
    int m_cameraOffsetY = 0;
    int m_playerRenderX = 0;
    int m_playerRenderY = 0;
    int m_backgroundPosX = 0;
    int m_backgroundPosY = 0;

    Character* m_pPlayer = nullptr;

    TileMap2D* m_TileMap;
};

extern CGame* g_pGame;
