#pragma once
#include "SpriteAnim.h"

enum class CentipedeAnimState {
    Attack1,
    Attack2,
    Attack3,
    Attack4,
    Death,
    Hurt,
    Idle,
    Sneer,
    Walk
};


using CentipedeAnim = SpriteAnim<CentipedeAnimState>;