#pragma once
#include "SpriteAnim.h"

enum class PortalAnimState {
	Idle,
};


using PortalAnim = SpriteAnim<PortalAnimState>;