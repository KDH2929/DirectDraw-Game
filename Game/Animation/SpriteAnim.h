#pragma once
#include <vector>
#include <algorithm>

// Template 클래스는 헤더파일안에 정의를 함
template <typename AnimState>
class SpriteAnim {
public:
    // Sprite 애니메이션에서는 Frame은 SpriteSheet 배열의 Index와 동일

    struct AnimSequence {
        AnimState state;                // 애니메이션 상태 (enum)
        int startFrame;                 // 시작 프레임
        int endFrame;                   // 종료 프레임
        float frameDuration;            // 각 프레임 지속 시간 (ms 단위)
        bool loop = true;              // true: 루프, false: 마지막 프레임에 고정
    };


    SpriteAnim()
        : m_currentState(), m_currentFrame(0), m_timer(0.0f)
    {
    }

    ~SpriteAnim() {}

    // 전체 애니메이션 시퀀스 정보 설정
    void SetSequences(const std::vector<AnimSequence>& sequences) {
        m_sequences = sequences;
    }

    // 현재 애니메이션 상태 변경 (상태 전환 시 프레임과 타이머 리셋)
    void SetState(AnimState state) {
        if (m_currentState != state) {
            m_currentState = state;
            for (const auto& seq : m_sequences) {
                if (seq.state == state) {
                    m_currentFrame = seq.startFrame;
                    m_timer = 0.0f;
                    break;
                }
            }
        }
    }


    // 주어진 애니메이션 상태에 해당하는 AnimSequence를 검색하여 반환 (없으면 nullptr 반환)
    const AnimSequence* GetSequence(AnimState state) const {
        for (const auto& seq : m_sequences) {
            if (seq.state == state)
                return &seq;
        }
        return nullptr;
    }


    // deltaTime: ms 단위로 가정 (혹은 ms로 변환하여 사용)
    void Update(float deltaTime) {

        // currentState에 해당하는 Sequence 찾기
        const AnimSequence* currentSeq = nullptr;
        for (const auto& seq : m_sequences) {
            if (seq.state == m_currentState) {
                currentSeq = &seq;
                break;
            }
        }

        if (currentSeq != nullptr) {
            m_timer += deltaTime;

            // deltaTIme이 매우 큰 경우, while문을 통해 여러 번 처리하는 식
            while (m_timer >= currentSeq->frameDuration) {
                m_timer -= currentSeq->frameDuration;
                m_currentFrame++;
                if (currentSeq->loop) {
                    // 루프일 경우, 끝 프레임을 지나면 시작 프레임으로
                    if (m_currentFrame > currentSeq->endFrame)
                        m_currentFrame = currentSeq->startFrame;
                }
                else {
                    // 루프가 아닐 경우, 끝 프레임에 고정
                    if (m_currentFrame > currentSeq->endFrame) {
                        m_currentFrame = currentSeq->endFrame;
                        break;
                    }
                }
            }
        }
    }

    // 현재 애니메이션 프레임 인덱스 반환
    int GetCurrentFrame() const { return m_currentFrame; }

    // 현재 애니메이션 상태 반환
    AnimState GetState() const { return m_currentState; }

private:
    std::vector<AnimSequence> m_sequences;  // 전체 애니메이션 시퀀스 정보
    AnimState m_currentState;                       // 현재 애니메이션 상태
    int m_currentFrame;                                // 현재 프레임 인덱스
    float m_timer;                                         // 누적 타이머 (ms 단위)
};
