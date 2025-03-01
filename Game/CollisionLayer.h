#pragma once


// 32비트 비트마스크 방식의 충돌 레이어를 위한 타입 (유니티의 Layer개념)
using CollisionLayer = unsigned int;

// 모든 레이어와 충돌하도록 하는 기본 마스크
const CollisionLayer DEFAULT_COLLISION_LAYER_MASK = 0xFFFFFFFF;
const CollisionLayer NO_COLLISION_LAYER_MASK = 0x00000000;

// 각 레이어
const CollisionLayer DEFAULT_COLLISION_LAYER = 1 << 0;         // (0b00...000001)
const CollisionLayer OVERLAP_COLLISION_LAYER = 1 << 1;         // (0b00...000010)
const CollisionLayer CHARACTER_COLLISION_LAYER = 1 << 2;     // (0b00...000100)       
const CollisionLayer TILE_COLLISION_LAYER = 1 << 3;                // (0b00...001000)
const CollisionLayer MONSTER_COLLISION_LAYER = 1 << 4;