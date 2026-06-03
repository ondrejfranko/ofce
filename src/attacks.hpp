#pragma once

#include "types.hpp"
#include <array>
#include <immintrin.h>

// Relevant potential blocker occupancy bitboards for sliding pieces
extern Bitboard BISHOP_MASKS_BB[SQUARE_COUNT];
extern Bitboard ROOK_MASKS_BB[SQUARE_COUNT];

// Non-sliding attack bitboards
extern Bitboard PAWN_ATTACKS_BB[COLOR_COUNT][SQUARE_COUNT];
extern Bitboard KNIGHT_ATTACKS_BB[SQUARE_COUNT];
extern Bitboard KING_ATTACKS_BB[SQUARE_COUNT];

// Sliding attack bitboards
extern std::array<std::array<Bitboard, 512>, SQUARE_COUNT> BISHOP_ATTACKS_BB;
extern std::array<std::array<Bitboard, 4096>, SQUARE_COUNT> ROOK_ATTACKS_BB;

// Squares between two given squares
extern Bitboard BETWEEN_BB[SQUARE_COUNT][SQUARE_COUNT];

// Squares of a line given by two squares
extern Bitboard LINE_BB[SQUARE_COUNT][SQUARE_COUNT];

// PEXT inline attack getters
inline Bitboard get_bishop_attacks(Square sq, Bitboard occupancy) {
    return BISHOP_ATTACKS_BB[sq][_pext_u64(occupancy, BISHOP_MASKS_BB[sq])];
}

inline Bitboard get_rook_attacks(Square sq, Bitboard occupancy) {
    return ROOK_ATTACKS_BB[sq][_pext_u64(occupancy, ROOK_MASKS_BB[sq])];
}

inline Bitboard get_queen_attacks(Square sq, Bitboard occupancy) {
    return get_bishop_attacks(sq, occupancy) | get_rook_attacks(sq, occupancy);
}

// Non-sliding attack getters
inline Bitboard get_king_attacks(Square sq) {
    return KING_ATTACKS_BB[sq];
}

inline Bitboard get_knight_attacks(Square sq) {
    return KNIGHT_ATTACKS_BB[sq];
}

inline Bitboard get_pawn_attacks(Color color, Square sq) {
    return PAWN_ATTACKS_BB[color][sq];
}

// Initialization of attack tables
void init_attacks();