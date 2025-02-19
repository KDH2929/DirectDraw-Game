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
        UpToFall,   // Jump 후 떨어지기 직전
        Fall,           // 실제 Fall
        EdgeGrab,
        EdgeIdle,
        WallSlide,
        Crouch,
        Dash,
        DashAttack,
        Slide,
        LadderGrab
    };

    // Sprite 애니메이션이므로 여기서 Frame은 SpriteSheet 배열의 Index와 동일
    struct AnimSequence {
        AnimState state;
        int startFrame;
        int endFrame;
        float frameDuration;        // ms단위
    };

    CharacterAnim();
    ~CharacterAnim();

    void SetSequences(const std::vector<AnimSequence>& sequences);

    // 현재 애님 상태 변경 (상태 전환 시 프레임과 타이머 리셋)
    void SetState(AnimState state);

    void Update(float deltaTime);

    // 현재 애님 프레임 인덱스 반환
    int GetCurrentFrame() const;

    AnimState GetState() const;


private:
    std::vector<AnimSequence> m_sequences;      // 전체 애니메이션 sequence들의 정보들을 모두 담아둠
    AnimState m_currentState;
    int m_currentFrame;
    float m_timer;                  // frameDuration이 m_timer값 초과시 다음 프레임으로 전환

};
