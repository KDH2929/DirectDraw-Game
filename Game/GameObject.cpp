#include "stdafx.h"
#include "GameObject.h"
#include "Collider.h"

GameObject::GameObject()
    : m_pCollider(nullptr),
    m_CurrentPosition(m_Transform.position),
    m_PreviousPosition(m_Transform.position),
    m_InterpolatedPosition(m_Transform.position),
    m_mass(1.0f), m_invMass(1.0f), m_physicsType(PhysicsType::Dynamic)
{
    // m_Transform은 기본 생성자에서 초기화됨.
}

GameObject::~GameObject() {
    if (m_pCollider) {
        delete m_pCollider;
        m_pCollider = nullptr;
    }
}

void GameObject::Update(float deltaTime) {
    // 보간 전 단계: 현재 위치를 이전 위치로 기록
    m_PreviousPosition = m_CurrentPosition;

    // 외부 로직(물리, 입력 처리 등)에 의해 m_Transform.position이 갱신되었다고 가정
    m_CurrentPosition = m_Transform.position;

    // Collider 업데이트
    if (m_pCollider)
        m_pCollider->Update(m_Transform);
}

void GameObject::Render(CDDrawDevice* pDevice) {

}

Transform& GameObject::GetTransform() {
    return m_Transform;
}

Vector2 GameObject::GetPosition() const {
    return m_Transform.position;
}

void GameObject::SetTransform(const Transform& newTransform) {
    m_Transform = newTransform;
    m_CurrentPosition = newTransform.position;
    m_PreviousPosition = newTransform.position;
    m_InterpolatedPosition = newTransform.position;
    if (m_pCollider)
        m_pCollider->Update(m_Transform);
}

Collider* GameObject::GetCollider() {
    return m_pCollider;
}

void GameObject::OnCollision(const CollisionInfo& collisionInfo) {
    // 기본 충돌 이벤트 처리 (상속받아 구체적인 처리구현)
    switch (collisionInfo.response) {
    case CollisionResponse::Block:
        break;
    case CollisionResponse::Overlap:
        break;
    case CollisionResponse::Ignore:
    default:
        break;
    }
}


void GameObject::SetPosition(const Vector2& newPos) {
    m_PreviousPosition = m_CurrentPosition;
    m_CurrentPosition = newPos;
    m_InterpolatedPosition = newPos;
    m_Transform.position = newPos;
}

void GameObject::ResetInterpolation() {
    m_PreviousPosition = m_CurrentPosition;
    m_InterpolatedPosition = m_CurrentPosition;
}

void GameObject::UpdateInterpolation(float alpha) {
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    m_InterpolatedPosition.x = m_PreviousPosition.x + (m_CurrentPosition.x - m_PreviousPosition.x) * alpha;
    m_InterpolatedPosition.y = m_PreviousPosition.y + (m_CurrentPosition.y - m_PreviousPosition.y) * alpha;
}

Vector2 GameObject::GetInterpolatedPosition() const {
    return m_InterpolatedPosition;
}



void GameObject::SetMass(float mass) {
    m_mass = mass;
    // 질량이 0이면 정적 객체로 간주하고, 역질량을 0으로 설정
    m_invMass = (mass != 0.0f) ? (1.0f / mass) : 0.0f;
}

float GameObject::GetMass() const {
    return m_mass;
}

float GameObject::GetInvMass() const {
    return m_invMass;
}

void GameObject::SetPhysicsType(PhysicsType type) {
    m_physicsType = type;
    // 정적 객체라면 질량은 0으로, 역질량은 0으로 설정
    if (type == PhysicsType::Static) {
        m_mass = 0.0f;
        m_invMass = 0.0f;
    }
}

PhysicsType GameObject::GetPhysicsType() const {
    return m_physicsType;
}