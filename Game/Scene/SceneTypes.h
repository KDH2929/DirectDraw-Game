#pragma once
#include <string>

enum class SceneType {
    Title,
    Stage1,
    Stage2,
    GameOver,
    Loading,
};


inline std::wstring SceneTypeToWString(SceneType type)
{
    switch (type)
    {
    case SceneType::Title:     return L"Title";
    case SceneType::Stage1:    return L"Stage1";
    case SceneType::Stage2:    return L"Stage2";
    case SceneType::GameOver:  return L"GameOver";
    case SceneType::Loading:   return L"Loading";
    default:                   return L"Unknown";
    }
}