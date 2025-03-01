#pragma once
#include "Scene.h"
#include "SceneTypes.h"
#include <windows.h>

// 헤더파일 순환참조는 항상 주의할 것

class CDDrawDevice;

class SceneLoader {
public:
    SceneLoader(CDDrawDevice* pDrawDevice, HWND hWnd);
    ~SceneLoader();

    // 지정된 SceneType에 따라 Scene 객체를 생성하여 반환
    Scene* LoadScene(SceneType type);

private:
    CDDrawDevice* m_pDrawDevice;
    HWND m_hWnd;
};
