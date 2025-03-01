#include "stdafx.h"
#include "SceneLoader.h"
#include "DebugManager.h"
#include "TitleScene.h"
#include "Stage1Scene.h"
#include "Stage2Scene.h"
#include "GameOverScene.h"
#include "LoadingScene.h"


SceneLoader::SceneLoader(CDDrawDevice* pDrawDevice, HWND hWnd)
    : m_pDrawDevice(pDrawDevice), m_hWnd(hWnd)
{
}

SceneLoader::~SceneLoader()
{
}

Scene* SceneLoader::LoadScene(SceneType type)
{
    LOG_MESSAGE(L"NewScene (" + SceneTypeToWString(type) + L") Memory Allocation Start");
    Scene* pScene = nullptr;
    switch (type)
    {
    case SceneType::Title:
        pScene = new TitleScene(m_pDrawDevice, m_hWnd);
        break;
    case SceneType::Stage1:
        pScene = new Stage1Scene(m_pDrawDevice, m_hWnd);
        break;
    case SceneType::Stage2:
        pScene = new Stage2Scene(m_pDrawDevice, m_hWnd);
        break;
    case SceneType::GameOver:
        pScene = new GameOverScene(m_pDrawDevice, m_hWnd);
        break;
    case SceneType::Loading:
        pScene = new LoadingScene(m_pDrawDevice, m_hWnd);
        break;
    default:
        break;
    }

    LOG_MESSAGE(L"NewScene (" + SceneTypeToWString(type) + L") Memory Allocation Complete");


    LOG_MESSAGE(L"NewScene (" + SceneTypeToWString(type) + L") Initialization Start");
    if (pScene)
    {
        pScene->Initialize();
    }
    LOG_MESSAGE(L"NewScene (" + SceneTypeToWString(type) + L") Initialization Complete");

    return pScene;

}
