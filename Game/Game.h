#pragma once
#include "DebugManager.h"
#include "InputManager.h"
#include <windows.h>

// 전방 선언
class CDDrawDevice;
class SceneManager;

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
    void OnMouseMove(int x, int y);
    void OnMouseDown(MouseButton button);
    void OnMouseUp(MouseButton button);
    void OnUpdateWindowSize();
    void OnUpdateWindowPos();
    BOOL CaptureBackBuffer(const char* szFileName);


private:
    CDDrawDevice* m_pDrawDevice = nullptr;
    HWND m_hWnd = nullptr;

    DWORD m_dwGameFPS = 60;
    DWORD m_dwCurrentFPS = 0;
    float m_fTicksPerFrame = 0.0f;
    ULONGLONG m_prevFrameTick = 0;
    LARGE_INTEGER m_prevCounter = {};

    SceneManager* m_pSceneManager = nullptr;
};

extern CGame* g_pGame;
