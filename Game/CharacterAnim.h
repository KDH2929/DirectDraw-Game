#pragma once
#include "SpriteAnim.h"


enum class CharacterAnimState {
    Idle,
    Run,
    Attack,       // 2 Combo Attack
    Death,
    Hurt,
    Jump,        // Jump
    UpToFall,    // Jump 후 떨어지기 직전
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


using CharacterAnim = SpriteAnim<CharacterAnimState>;