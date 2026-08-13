#pragma once

#include "position.hpp"
#include <cstdint>

// perft (leaf node count of the position tree)
uint64_t perft(const Position &pos, int depth);

// perft divide (leaf node count of every legal root move)
uint64_t perft_divide(const Position &pos, int depth);