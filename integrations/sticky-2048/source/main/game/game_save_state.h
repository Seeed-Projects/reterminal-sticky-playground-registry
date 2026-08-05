#pragma once

#include <cstdint>

#include "game/game_2048.h"

bool game_save_state(const Game2048 &game, uint32_t best_score);
bool game_load_state(Game2048 &game, uint32_t random_seed, uint32_t &best_score);
