#pragma once
#include "MathUtils.h"
#include "Collider.h"
#include <string>


enum class PhysicsType {
    Static,         // �������� ���� (ex: ��, �ٴ�)
    Dynamic,    // ���� �ùķ��̼ǿ� ���� ������
};

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

    // �浹 �̺�Ʈ �ݹ� (�浹 �߻� �� ȣ��)
    // other: �浹�� �ٸ� GameObject, response: �浹 ���� Ÿ��
    virtual void OnCollision(GameObject* other, CollisionResponse response);


    // ���� ���� �Լ���
    virtual void SetPosition(const Vector2& newPos);
    void ResetInterpolation();
    void UpdateInterpolation(float alpha);
    Vector2 GetInterpolatedPosition() const;


    // ���� ���� Getter/Setter
    void SetMass(float mass);         // ������ �����ϰ�, ���ÿ� �������� ���
    float GetMass() const;
    float GetInvMass() const;         // �̸� ���� ������ ��ȯ

    void SetPhysicsType(PhysicsType type);
    PhysicsType GetPhysicsType() const;


protected:
    Transform m_Transform;
    Collider* m_pCollider;

    // ������ ���� ��ġ ����
    Vector2 m_CurrentPosition;
    Vector2 m_PreviousPosition;
    Vector2 m_InterpolatedPosition;


    // ���� �Ӽ�
    float m_mass;         // ����
    float m_invMass;      // ������ (mass�� 0�̸� 0, �׷��� ������ 1/mass)
    PhysicsType m_physicsType;

};
