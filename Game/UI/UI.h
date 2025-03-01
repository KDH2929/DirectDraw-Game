#pragma once

#include <windows.h>

// 추상클래스

class UI {
public:
    UI() = default;
    virtual ~UI() = default;

    virtual bool Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(HDC hdc) = 0;
};