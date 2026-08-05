#pragma once

#include "move.hpp"
#include "position.hpp"
#include "types.hpp"

// Get the new position after playing the move (copy-make principle)
Position make_move(const Position &pos, Move move);

// Check if the king of the specified color is attacked
bool is_king_attacked(const Position &pos, Color us);

// Check if the pseudo-legal move is legal
bool is_legal_move(const Position &pos, Move move);