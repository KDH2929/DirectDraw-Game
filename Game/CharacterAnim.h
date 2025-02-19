#pragma once
#include <vector>
#include <algorithm>


class CharacterAnim {
public:
    enum class AnimState {
        Idle,
        Run,
        Attack,     // 2 Combo Attack
        Death,
        Hurt,
        Jump,       // Jump
        UpToFall,   // Jump �� �������� ����
        Fall,           // ���� Fall
        EdgeGrab,
        EdgeIdle,
        WallSlide,
        Crouch,
        Dash,
        DashAttack,
        Slide,
        LadderGrab
    };

    // Sprite �ִϸ��̼��̹Ƿ� ���⼭ Frame�� SpriteSheet �迭�� Index�� ����
    struct AnimSequence {
        AnimState state;
        int startFrame;
        int endFrame;
        float frameDuration;        // ms����
    };

    CharacterAnim();
    ~CharacterAnim();

    void SetSequences(const std::vector<AnimSequence>& sequences);

    // ���� �ִ� ���� ���� (���� ��ȯ �� �����Ӱ� Ÿ�̸� ����)
    void SetState(AnimState state);

    void Update(float deltaTime);

    // ���� �ִ� ������ �ε��� ��ȯ
    int GetCurrentFrame() const;

    AnimState GetState() const;


private:
    std::vector<AnimSequence> m_sequences;      // ��ü �ִϸ��̼� sequence���� �������� ��� ��Ƶ�
    AnimState m_currentState;
    int m_currentFrame;
    float m_timer;                  // frameDuration�� m_timer�� �ʰ��� ���� ���������� ��ȯ

};
