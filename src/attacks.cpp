#include "attacks.hpp"
#include "bitboard.hpp"
#include "types.hpp"
#include <array>
#include <bit>
#include <cassert>

// Relevant potential blocker occupancy bitboards for sliding pieces
Bitboard BISHOP_MASKS_BB[SQUARE_COUNT];
Bitboard ROOK_MASKS_BB[SQUARE_COUNT];

// Non-sliding attacks
Bitboard PAWN_ATTACKS_BB[COLOR_COUNT][SQUARE_COUNT];
Bitboard KNIGHT_ATTACKS_BB[SQUARE_COUNT];
Bitboard KING_ATTACKS_BB[SQUARE_COUNT];

// Sliding attacks
std::array<std::array<Bitboard, 512>, SQUARE_COUNT> BISHOP_ATTACKS_BB;
std::array<std::array<Bitboard, 4096>, SQUARE_COUNT> ROOK_ATTACKS_BB;

// Squares between two given squares
Bitboard BETWEEN_BB[SQUARE_COUNT][SQUARE_COUNT];

// Squares of a line given by two squares
Bitboard LINE_BB[SQUARE_COUNT][SQUARE_COUNT];