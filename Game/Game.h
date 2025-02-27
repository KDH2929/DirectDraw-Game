#pragma once
#include "DebugManager.h"
#include "Monster.h"
#include "MathUtils.h"
#include "ColliderManager.h"

// ���� ����
class CTGAImage;
class CImageData;
class Character;   // �÷��̾� ĳ����
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

    // �̹��� ���ҽ��� (�ݵ�� �����Ҵ� �� ���� �� ��!!)
    CImageData* m_pPlayerImgData = nullptr;
    CImageData* m_pCentipedeImgData = nullptr;
    CTGAImage* m_pBackgroundImage = nullptr;
    
    TileMap2D* m_TileMap = nullptr;

    int m_bgWidth = 0;
    int m_bgHeight = 0;

    // ī�޶� ó��
    int m_cameraOffsetX = 0;
    int m_cameraOffsetY = 0;
    int m_playerRenderX = 0;
    int m_playerRenderY = 0;
    int m_backgroundPosX = 0;
    int m_backgroundPosY = 0;

    Character* m_pPlayer = nullptr;
    std::vector<Monster*> m_vMonsters;

};

extern CGame* g_pGame;
