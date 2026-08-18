#include "eval.hpp"
#include "types.hpp"

// Piece-square tables (including base piece values)
ScorePair PSQT[PIECE_COUNT][SQUARE_COUNT]{};

// Global pawn structure hash table
PawnTable PAWN_TABLE;