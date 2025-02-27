#pragma once
#include <cmath>
#include <stdexcept>

constexpr float PI = 3.14159265f;

inline float degToRad(float degrees) {
    return degrees * PI / 180.0f;
}

template <typename T>
class Vector2 {
public:
    T x, y;

    // 기본 생성자
    Vector2() : x(T(0)), y(T(0)) {}

    // 매개변수 생성자
    Vector2(T x, T y) : x(x), y(y) {}

    // 형 변환 생성자: 다른 타입의 Vector2로부터 변환
    template <typename U>
    explicit Vector2(const Vector2<U>& other)
        : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

    // 벡터 길이 계산
    T Length() const {
        return static_cast<T>(std::sqrt(x * x + y * y));
    }

    // 정규화된 벡터 반환 (길이가 0이면 예외 발생)
    Vector2 Normalized() const {
        T len = Length();
        if (len == T(0))
            throw std::runtime_error("Cannot normalize zero-length vector.");
        return *this / len;
    }

    // 연산자 오버로딩
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(T scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(T scalar) const {
        if (scalar == T(0))
            throw std::runtime_error("Division by zero in Vector2 operator/");
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }
};

// 글로벌 벡터 연산 함수들
template <typename T>
T Dot(const Vector2<T>& a, const Vector2<T>& b) {
    return a.x * b.x + a.y * b.y;
}

template <typename T>
Vector2<T> Normalize(const Vector2<T>& a) {
    T len = a.Length();
    if (len == T(0))
        return Vector2<T>(T(0), T(0));
    return a / len;
}

template <typename T>
Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
    return Vector2<T>(scalar * vec.x, scalar * vec.y);
}


class Transform {
public:
    Vector2<float> position;    // 위치
    float rotation;             // 회전 (도 단위)
    Vector2<float> scale;       // 스케일

    Transform() : position(0, 0), rotation(0.0f), scale(1.0f, 1.0f) {}
};
