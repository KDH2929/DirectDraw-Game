#pragma once

#include "Scene.h"
#include "UI_Button.h"
#include "UI_Text.h"
#include <windows.h>

class CDDrawDevice;

class TitleScene : public Scene {
public:
    TitleScene(CDDrawDevice* pDrawDevice, HWND hWnd);
    virtual ~TitleScene();

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Update(float deltaTime) override;
    virtual void Render() override;

    virtual void ResetInterpolation() override;
    virtual void InterpolatePosition(float alpha) override;

    virtual const SceneType GetSceneType() override;

private:
    HWND m_hWnd;                        // 렌더링에 사용할 윈도우 핸들
    CDDrawDevice* m_pDrawDevice;          // DirectDraw 장치 포인터

    UI_Button* m_pStartButton;
    UI_Button* m_pExitButton;
    UI_Text* m_pTitleText;              // 타이틀 텍스트 ("Demo Game")
};
