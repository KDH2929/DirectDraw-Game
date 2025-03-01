#include "stdafx.h"
#include "SceneManager.h"
#include "DebugManager.h"
#include <chrono>
#include <future>

// 초기화된 싱글톤 인스턴스
SceneManager* SceneManager::s_instance = nullptr;

// 싱글톤 인스턴스 접근
SceneManager* SceneManager::GetInstance()
{
    return s_instance;
}

// Game 클래스의 Initialize에서 반드시 호출할 초기화 함수
void SceneManager::Initialize(CDDrawDevice* pDrawDevice, HWND hWnd)
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }

    s_instance = new SceneManager(pDrawDevice, hWnd);
}

// 싱글톤 인스턴스 해제
void SceneManager::CleanUp()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

SceneManager::SceneManager(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice),
    m_hWnd(hWnd),
    m_pCurrentScene(nullptr),
    m_pPreviousScene(nullptr),
    m_sceneLoader(pDrawDevice, hWnd),
    m_loading(false)
{
}

SceneManager::~SceneManager()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Cleanup();
        delete m_pCurrentScene;
        m_pCurrentScene = nullptr;
    }

    if (m_pPreviousScene)
    {
        m_pPreviousScene->Cleanup();
        delete m_pPreviousScene;
        m_pPreviousScene = nullptr;
    }
}

void SceneManager::ChangeScene(SceneType newSceneType)
{
    if (m_pCurrentScene)
    {
        m_pPreviousScene = m_pCurrentScene;
    }

    // 새로운 씬 로드
    m_pCurrentScene = m_sceneLoader.LoadScene(newSceneType);
    
    SceneType DebugType = m_pCurrentScene->GetSceneType();
    LOG_MESSAGE(L"Changed Scene: " + SceneTypeToWString(DebugType));

    // 여기서 이전 Scene의 동적할당을 해제하면 안됨
}

void SceneManager::AsyncChangeScene(SceneType newSceneType)
{
    ChangeScene(SceneType::Loading);

    // 백그라운드에서 새 씬을 비동기적으로 생성
    m_futureScene = std::async(std::launch::async, [this, newSceneType]() -> Scene* {
        return m_sceneLoader.LoadScene(newSceneType);
        });
    m_loading = true;
}

void SceneManager::Update(float deltaTime)
{
    // 비동기 로딩 중이면 백그라운드 작업 상태를 확인
    if (m_loading)
    {
        if (PollAsyncSceneLoading())
        {
            // m_futureScene.get();을 호출하면, std::future가 결과를 받아오고 비동기 쓰레드는 자동 정리됨
            Scene* loadedScene = m_futureScene.get();
            if (m_pCurrentScene)
            {
                m_pPreviousScene = m_pCurrentScene;
            }

            m_loading = false;

            m_pCurrentScene = loadedScene;
            SceneType DebugType = m_pCurrentScene->GetSceneType();
            LOG_MESSAGE(L"Changed Scene (" + SceneTypeToWString(DebugType) + L") AsyncLoad Completed.");
        }
    }

    // 여기서 이전 Scene의 동적할당을 해제할 것
    if (m_pPreviousScene)
    {
        SceneType previousSceneType = m_pPreviousScene->GetSceneType();
        LOG_MESSAGE(L"Previous Scene (" + SceneTypeToWString(previousSceneType) + L") Memory Deallocation Start");

        m_pPreviousScene->Cleanup();
        delete m_pPreviousScene;
        m_pPreviousScene = nullptr;
        
        LOG_MESSAGE(L"Previous Scene (" + SceneTypeToWString(previousSceneType) + L") Memory Deallocation Completed");
    }

    if (m_pCurrentScene)
    {
        m_pCurrentScene->Update(deltaTime);
    }
}

void SceneManager::InterpolatePosition(float alpha)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->InterpolatePosition(alpha);
    }
}

void SceneManager::ResetInterpolation()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->ResetInterpolation();
    }
}

void SceneManager::Render()
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Render();
    }
}

// 백그라운드에서 씬 로딩 상태를 확인하고 전환하는 함수
bool SceneManager::PollAsyncSceneLoading()
{
    // 비동기 로딩이 완료되었는지 0ms 대기하여 상태 확인
    if (m_futureScene.valid() &&
        m_futureScene.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
    {
        return true;
    }

    return false;
}
