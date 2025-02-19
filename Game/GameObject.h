#pragma once
#include "MathUtils.h"
#include "Collider.h"
#include <string>

class CDDrawDevice;


class GameObject {
public:
    GameObject();
    virtual ~GameObject();

    // �� ������ ������Ʈ (deltaTime: �и��� ����)
    virtual void Update(float deltaTime);
    // �� ������ ������
    virtual void Render(CDDrawDevice* pDevice);     // ���� winAPI ��������� ���� ������ HDC�� �Ű������� �߰�

    // Transform ������/������
    Transform& GetTransform();
    Vector2 GetPosition() const;
    void SetTransform(const Transform& newTransform);

    // Collider ����
    Collider* GetCollider();

    // ���� ���� �Լ��� (�׻� ���� ����)
    virtual void SetPosition(const Vector2& newPos);
    void ResetInterpolation();
    void UpdateInterpolation(float alpha);
    Vector2 GetInterpolatedPosition() const;

protected:
    Transform m_Transform;
    Collider* m_pCollider;

    // ������ ���� ��ġ ����
    Vector2 m_CurrentPosition;
    Vector2 m_PreviousPosition;
    Vector2 m_InterpolatedPosition;
};
