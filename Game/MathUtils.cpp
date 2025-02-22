#include "stdafx.h"
#include "MathUtils.h"

// degree를 radian으로 변환하는 함수 구현
float degToRad(float degrees) {
    return degrees * PI / 180.0f;
}

// Vector2 클래스의 구현

// 기본 생성자
Vector2::Vector2() : x(0.0f), y(0.0f) {}

// 매개변수 생성자
Vector2::Vector2(float x, float y) : x(x), y(y) {}

// 벡터 길이 계산
float Vector2::Length() const {
    return std::sqrt(x * x + y * y);
}

// 정규화된 벡터 반환 (길이가 0이면 예외 발생)
Vector2 Vector2::Normalized() const {
    float len = Length();
    if (len == 0.0f)
        throw std::runtime_error("Cannot normalize zero-length vector.");
    return *this / len;
}

// 덧셈 연산자
Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

// 뺄셈 연산자
Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

// 스칼라 곱셈 연산자
Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

// 스칼라 나눗셈 연산자 (0으로 나누면 예외 발생)
Vector2 Vector2::operator/(float scalar) const {
    if (scalar == 0.0f)
        throw std::runtime_error("Division by zero in Vector2 operator/");
    return Vector2(x / scalar, y / scalar);
}

// 누적 덧셈 연산자
Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

// 단항 음수 연산자
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


// Transform 클래스의 구현

Transform::Transform()
    : position(0.0f, 0.0f), rotation(0.0f), scale(1.0f, 1.0f) {}
