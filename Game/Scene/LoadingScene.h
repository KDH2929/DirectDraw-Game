#pragma once

#include "Scene.h"
#include <windows.h>

class CDDrawDevice;
class UI_Text;

class LoadingScene : public Scene {
public:
    LoadingScene(CDDrawDevice* pDrawDevice, HWND hWnd);
    virtual ~LoadingScene();

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    virtual void ResetInterpolation() override;
    virtual void InterpolatePosition(float alpha) override;

    virtual const SceneType GetSceneType() override;

private:
    CDDrawDevice* m_pDrawDevice;  // DirectDraw 장치 포인터
    HWND m_hWnd;                  // 렌더링에 사용할 윈도우 핸들
    UI_Text* m_pLoadingText;      // 중앙에 표시할 텍스트 (Loading 문구)

    float m_timer;                // 텍스트 상태 변경 타이머
    int m_currentState;           // 현재 로딩 텍스트 상태 인덱스
};
