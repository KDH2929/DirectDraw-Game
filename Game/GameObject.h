#pragma once
#include "MathUtils.h"
#include "Collider.h"
#include <string>

class CDDrawDevice;


class GameObject {
public:
    GameObject();
    virtual ~GameObject();

    // 매 프레임 업데이트 (deltaTime: 밀리초 단위)
    virtual void Update(float deltaTime);
    // 매 프레임 렌더링
    virtual void Render(CDDrawDevice* pDevice);     // 만약 winAPI 렌더기능을 쓰고 싶으면 HDC를 매개변수로 추가

    // Transform 접근자/설정자
    Transform& GetTransform();
    Vector2 GetPosition() const;
    void SetTransform(const Transform& newTransform);

    // Collider 관련
    Collider* GetCollider();

    // 보간 관련 함수들 (항상 보간 적용)
    virtual void SetPosition(const Vector2& newPos);
    void ResetInterpolation();
    void UpdateInterpolation(float alpha);
    Vector2 GetInterpolatedPosition() const;

protected:
    Transform m_Transform;
    Collider* m_pCollider;

    // 보간을 위한 위치 변수
    Vector2 m_CurrentPosition;
    Vector2 m_PreviousPosition;
    Vector2 m_InterpolatedPosition;
};
