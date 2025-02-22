#include "stdafx.h"
#include "MathUtils.h"

// degree�� radian���� ��ȯ�ϴ� �Լ� ����
float degToRad(float degrees) {
    return degrees * PI / 180.0f;
}

// Vector2 Ŭ������ ����

// �⺻ ������
Vector2::Vector2() : x(0.0f), y(0.0f) {}

// �Ű����� ������
Vector2::Vector2(float x, float y) : x(x), y(y) {}

// ���� ���� ���
float Vector2::Length() const {
    return std::sqrt(x * x + y * y);
}

// ����ȭ�� ���� ��ȯ (���̰� 0�̸� ���� �߻�)
Vector2 Vector2::Normalized() const {
    float len = Length();
    if (len == 0.0f)
        throw std::runtime_error("Cannot normalize zero-length vector.");
    return *this / len;
}

// ���� ������
Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

// ���� ������
Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

// ��Į�� ���� ������
Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

// ��Į�� ������ ������ (0���� ������ ���� �߻�)
Vector2 Vector2::operator/(float scalar) const {
    if (scalar == 0.0f)
        throw std::runtime_error("Division by zero in Vector2 operator/");
    return Vector2(x / scalar, y / scalar);
}

// ���� ���� ������
Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

// ���� ���� ������
Vector2 Vector2::operator-() const {
    return Vector2(-x, -y);
}


float Dot(const Vector2& a, const Vector2& b) {
    return a.x * b.x + a.y * b.y;
}

Vector2 Normalize(const Vector2& a) {
    float len = a.Length();
    if (len == 0.0f)
        return Vector2(0.0f, 0.0f);
    return a / len;
}


// Transform Ŭ������ ����

Transform::Transform()
    : position(0.0f, 0.0f), rotation(0.0f), scale(1.0f, 1.0f) {}
