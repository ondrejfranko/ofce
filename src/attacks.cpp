#include "attacks.hpp"
#include "bitboard.hpp"
#include "types.hpp"
#include <array>
#include <bit>
#include <cassert>
#include <immintrin.h>

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

static Bitboard compute_bishop_attacks(Square sq, Bitboard occupancy) {
    Bitboard attacks = 0;
    int file = sq % 8;
    int rank = sq / 8;

    for (auto dir : {NORTH_WEST, NORTH_EAST, SOUTH_WEST, SOUTH_EAST}) {
        int f = file + dir % 8;
        int r = rank + dir / 8;

        while (FILE_A <= f && f < FILE_COUNT && RANK_1 <= r && r < RANK_COUNT) {
            attacks |= bit(static_cast<Square>(r * 8 + f));

            if (occupancy & bit(static_cast<Square>(r * 8 + f))) {
                break;
            }

            f += dir % 8;
            r += dir / 8;
        }
    }
    return attacks;
}

static Bitboard compute_rook_attacks(Square sq, Bitboard occupancy) {
    Bitboard attacks = 0;
    int file = sq % 8;
    int rank = sq / 8;

    for (auto dir : {NORTH, SOUTH, WEST, EAST}) {
        int f = file + dir % 8;
        int r = rank + dir / 8;

        while (FILE_A <= f && f < FILE_COUNT && RANK_1 <= r && r < RANK_COUNT) {
            attacks |= bit(static_cast<Square>(r * 8 + f));

            if (occupancy & bit(static_cast<Square>(r * 8 + f))) {
                break;
            }

            f += dir % 8;
            r += dir / 8;
        }
    }
    return attacks;
}

static void build_bishop_attacks(Square sq) {
    Bitboard mask = BISHOP_MASKS_BB[sq];
    int relevant_bits = std::popcount(mask);
    int occupancy_variations = 1 << relevant_bits;

    // Generate attacks for all possible blocker configurations
    for (int i = 0; i < occupancy_variations; ++i) {
        // Use PDEP to create the occupancy bitboard for the current variation
        Bitboard occupancy = _pdep_u64(static_cast<uint64_t>(i), mask);
        // Use PEXT to get the index for the attack table
        BISHOP_ATTACKS_BB[sq][_pext_u64(occupancy, mask)] = compute_bishop_attacks(sq, occupancy);
    }
}

static void build_rook_attacks(Square sq) {
    Bitboard mask = ROOK_MASKS_BB[sq];
    int relevant_bits = std::popcount(mask);
    int occupancy_variations = 1 << relevant_bits;

    // Generate attacks for all possible blocker configurations
    for (int i = 0; i < occupancy_variations; ++i) {
        // Use PDEP to create the occupancy bitboard for the current variation
        Bitboard occupancy = _pdep_u64(static_cast<uint64_t>(i), mask);
        // Use PEXT to get the index for the attack table
        ROOK_ATTACKS_BB[sq][_pext_u64(occupancy, mask)] = compute_rook_attacks(sq, occupancy);
    }
}

void init_attacks() {
    // Pawn attacks
    for (int sq = A1; sq < SQUARE_COUNT; ++sq) {
        int file = sq % 8;
        int rank = sq / 8;
        Bitboard white_pawn_attacks = 0;
        Bitboard black_pawn_attacks = 0;

        for (int file_offset : {-1, 1}) {
            int f = file + file_offset;

            if (FILE_A <= f && f < FILE_COUNT) {
                if (RANK_1 <= rank + 1 && rank + 1 < RANK_COUNT) {
                    white_pawn_attacks |= bit(static_cast<Square>((rank + 1) * 8 + f));
                }
                if (RANK_1 <= rank - 1 && rank - 1 < RANK_COUNT) {
                    black_pawn_attacks |= bit(static_cast<Square>((rank - 1) * 8 + f));
                }
            }
        }

        PAWN_ATTACKS_BB[WHITE][sq] = white_pawn_attacks;
        PAWN_ATTACKS_BB[BLACK][sq] = black_pawn_attacks;
    }

    // Knight attacks
    for (int sq = A1; sq < SQUARE_COUNT; ++sq) {
        int file = sq % 8;
        int rank = sq / 8;
        Bitboard knight_attacks = 0;

        for (auto [file_offset, rank_offset] : {std::pair{2, 1}, {1, 2}, {2, -1}, {-1, 2}, {-2, 1}, {1, -2}, {-2, -1}, {-1, -2}}) {
            int f = file + file_offset;
            int r = rank + rank_offset;

            if (FILE_A <= f && f < FILE_COUNT && RANK_1 <= r && r < RANK_COUNT) {
                knight_attacks |= bit(static_cast<Square>(r * 8 + f));
            }
        }

        KNIGHT_ATTACKS_BB[sq] = knight_attacks;
    }

    // King attacks
    for (int sq = A1; sq < SQUARE_COUNT; ++sq) {
        int file = sq % 8;
        int rank = sq / 8;
        Bitboard king_attacks = 0;

        for (int file_offset = -1; file_offset <= 1; ++file_offset) {
            for (int rank_offset = -1; rank_offset <= 1; ++rank_offset) {
                if (file_offset == 0 && rank_offset == 0) {
                    continue;
                }

                int f = file + file_offset;
                int r = rank + rank_offset;

                if (FILE_A <= f && f < FILE_COUNT && RANK_1 <= r && r < RANK_COUNT) {
                    king_attacks |= bit(static_cast<Square>(r * 8 + f));
                }
            }
        }

        KING_ATTACKS_BB[sq] = king_attacks;
    }

    // Sliding masks and attacks
    for (int sq = A1; sq < SQUARE_COUNT; ++sq) {
        // Bishop mask
        BISHOP_MASKS_BB[sq] = 0;
        BISHOP_MASKS_BB[sq] |= get_bishop_mask(static_cast<Square>(sq));
        // Build bishop attacks for all occupancy variations
        build_bishop_attacks(static_cast<Square>(sq));

        // Rook mask
        ROOK_MASKS_BB[sq] = 0;
        ROOK_MASKS_BB[sq] |= get_rook_mask(static_cast<Square>(sq));
        // Build rook attacks for all occupancy variations
        build_rook_attacks(static_cast<Square>(sq));
    }
}