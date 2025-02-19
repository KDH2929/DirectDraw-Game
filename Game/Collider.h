#pragma once
#include "MathUtils.h"
#include <string>
#include <vector>
#include <utility>
#include <Windows.h>


// 충돌체 종류
enum class ColliderType {
    AABB,
    OBB,
};

// 기본 Collider 클래스 (추상 클래스)
class Collider {
public:
    Collider(ColliderType type) : m_type(type) {}
    virtual ~Collider() {}

    ColliderType GetType() const { return m_type; }

    // 두 Collider 간의 충돌 여부를 검사하는 추상 함수
    virtual bool CheckCollision(const Collider* other) const = 0;


    virtual void Update(const Transform& transform) = 0;
    virtual void Render(HDC hDC, int renderX, int renderY) const = 0;


    // 디버그용 문자열 반환
    virtual std::string GetDebugString() const = 0;

protected:
    ColliderType m_type;
};