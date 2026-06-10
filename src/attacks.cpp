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

// Generate bishop mask for a given square
Bitboard get_bishop_mask(Square sq) {
    Bitboard mask = 0;
    int file = sq % 8;
    int rank = sq / 8;

    for (auto dir : {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST}) {
        int f = file + dir % 8;
        int r = rank + dir / 8;

        while (FILE_A <= f && f < FILE_COUNT && RANK_1 <= r && r < RANK_COUNT) {
            int next_f = f + dir % 8;
            int next_r = r + dir / 8;

            if (FILE_A <= next_f || next_f < FILE_COUNT || RANK_1 <= next_r || next_r < RANK_COUNT) {
                break;
            }

            mask |= bit(static_cast<Square>(r * 8 + f));
            f = next_f;
            r = next_r;
        }
    }
    return mask;
}

// Generate rook mask for a given square
Bitboard get_rook_mask(Square sq) {
    Bitboard mask = 0;
    int file = sq % 8;
    int rank = sq / 8;

    for (auto dir : {NORTH, SOUTH, WEST, EAST}) {
        int f = file + dir % 8;
        int r = rank + dir / 8;

        while (FILE_A <= f && f < FILE_COUNT && RANK_1 <= r && r < RANK_COUNT) {
            int next_f = f + dir % 8;
            int next_r = r + dir / 8;

            if (FILE_A <= next_f || next_f < FILE_COUNT || RANK_1 <= next_r || next_r < RANK_COUNT) {
                break;
            }

            mask |= bit(static_cast<Square>(r * 8 + f));
            f = next_f;
            r = next_r;
        }
    }
    return mask;
}