#pragma once

#include "Scene.h"
#include "SceneLoader.h"
#include "SceneTypes.h"
#include <windows.h>
#include <vector>
#include <future>


class CDDrawDevice; // 전방 선언

class SceneManager
{
public:
    // 싱글톤
    static SceneManager* GetInstance();

    // Game 클래스의 Initialize에서 반드시 호출 할 것
    static void Initialize(CDDrawDevice* pDrawDevice, HWND hWnd);
    static void CleanUp();

    // 동기 씬 전환: 현재 씬을 정리하고, 새 씬을 동기적으로 생성 및 초기화
    void ChangeScene(SceneType newSceneType);

    // 비동기 씬 전환: 로딩 씬을 표시한 후, 백그라운드에서 새 씬 생성
    void AsyncChangeScene(SceneType newSceneType);

    void Update(float deltaTime);
    void InterpolatePosition(float alpha);
    void ResetInterpolation();
    void Render();

private:
    // 생성자 및 소멸자는 private
    SceneManager(CDDrawDevice* pDrawDevice, HWND hWnd);
    ~SceneManager();

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // 백그라운드에서 씬 로딩 상태를 확인
    bool PollAsyncSceneLoading();

private:
    static SceneManager* s_instance;

    Scene* m_pCurrentScene;
    Scene* m_pPreviousScene;
    SceneLoader m_sceneLoader;              // 씬 생성은 SceneLoader를 통해 처리
    CDDrawDevice* m_pDrawDevice;
    HWND m_hWnd;

    // 비동기 로딩 관련 멤버
    std::future<Scene*> m_futureScene;
    bool m_loading;
};
