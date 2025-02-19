#include "stdafx.h"
#include "MathUtils.h"

// Transform�� �⺻ ������ ����
Transform::Transform()
    : position{ 0.0f, 0.0f }, rotation(0.0f), scale{ 1.0f, 1.0f } {}

// degree�� radian���� ��ȯ�ϴ� �Լ� ����
float degToRad(float degrees) {
    return degrees * PI / 180.0f;
}

// ���� ���� ����
Vector2 operator+(const Vector2& a, const Vector2& b) {
    return { a.x + b.x, a.y + b.y };
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
    return { a.x - b.x, a.y - b.y };
}

Vector2 operator*(const Vector2& a, float s) {
    return { a.x * s, a.y * s };
}

float Dot(const Vector2& a, const Vector2& b) {
    return a.x * b.x + a.y * b.y;
}

Vector2 Normalize(const Vector2& a) {
    float len = std::sqrt(a.x * a.x + a.y * a.y);
    return (len > 0) ? Vector2{ a.x / len, a.y / len } : Vector2{ 0, 0 };
}
