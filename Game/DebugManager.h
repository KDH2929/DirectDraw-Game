#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include "MathUtils.h"
#include <cmath>

// ���� DebugMessage ����ü
struct DebugMessage {
    std::wstring text;  // ����� �ؽ�Ʈ
    float duration;     // ȭ�鿡 ���� ���� �ð� (ms ����)
    float elapsed;      // ����� �ð�
};


struct DebugBox {
    // OBB ������ �ڽ� ������ ���� �ʵ�
    Vector2<float> center;        // �ڽ� �߽�
    float width;                  // ��ü ��
    float height;                 // ��ü ����
    float rotation;               // ȸ���� (�� ����)
    COLORREF color;               // �ڽ� �� ����

    float duration;               // ȭ�鿡 ���� �ð� (duration �� �� ����, ���������� ms�� ��ȯ�ؼ� ���)
    float elapsed;                // ��� �ð�


    // �Ű����� ������
    DebugBox(const Vector2<float>& _center, float _width, float _height, float _rotation,
        COLORREF _color = RGB(255, 0, 0), float _duration = 1.0f)
        : center(_center), width(_width), height(_height), rotation(_rotation),
        color(_color), duration(_duration * 1000.0f), elapsed(0.0f) 
    {}
};


struct DebugLine {
    Vector2<float> start;         // ������
    Vector2<float> end;           // ����
    COLORREF color;               // �� ����
    int thickness;                // �� �β�
    float duration;               // ȭ�鿡 ���� �ð� (�� ����, ���������� ms�� ��ȯ)
    float elapsed;                // ����� �ð�

    DebugLine(const Vector2<float>& _start, const Vector2<float>& _end,
        COLORREF _color = RGB(0, 0, 0), int _thickness = 1, float _duration = 1.0f)
        : start(_start), end(_end), color(_color), thickness(_thickness),
        duration(_duration * 1000.0f), elapsed(0.0f)
    {}
};



class DebugManager {
public:
    static DebugManager& GetInstance() {
        static DebugManager instance;
        return instance;
    }

    // ȭ�鿡 �޽����� ǥ���ϴ� �Լ� (duration�� �� ����)
    void AddOnScreenMessage(const std::wstring& msg, float duration);
    // �ܼ�/����� ��¿� �޽����� ����ϴ� �Լ�
    void LogMessage(const std::wstring& msg);

    void SetCameraOffset(Vector2<float> cameraOffset);
    void AddDebugBox(const DebugBox& box);
    void AddDebugLine(const DebugLine& line);

    void Update(float deltaTime);
    void Render(HDC hDC);

private:
    DebugManager();
    ~DebugManager();
    DebugManager(const DebugManager&) = delete;
    DebugManager& operator=(const DebugManager&) = delete;

    std::vector<DebugMessage> m_messages;
    std::vector<DebugBox> m_debugBoxes;
    std::vector<DebugLine> m_debugLines;

    Vector2<float> m_cameraOffset = Vector2<float>(0.0f, 0.0f);
};

