#pragma once

#include "types.hpp"

extern Key ZOBRIST_PSQ[PIECE_COUNT][SQUARE_COUNT];
extern Key ZOBRIST_EP[FILE_COUNT];
extern Key ZOBRIST_CASTLING[16];
extern Key ZOBRIST_SIDE_TO_MOVE;

void init_zobrist();