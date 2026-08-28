#pragma once

#include <cstdint>

using Bitboard = uint64_t;
using Key = uint64_t;
using Score = int; // TODO: possibly change to int16_t to fit into a TT entry

inline constexpr int MAX_POS_MOVES = 256;
inline constexpr int MAX_PLY_DEPTH = 256;

inline constexpr Score SCORE_DRAW = 0; // draw score
inline constexpr Score SCORE_MATE_IN_MAX = 32000; // mate score at max ply depth
inline constexpr Score SCORE_MATE = SCORE_MATE_IN_MAX + MAX_PLY_DEPTH; // mate at this node (32256)
inline constexpr Score SCORE_NONE = SCORE_MATE_IN_MAX + 1; // score not available, used for initialization (32257)

// Check if a score is a mate score
constexpr bool is_mate_score(Score s) {
    Score a = s < 0 ? -s : s;
    return a >= SCORE_MATE_IN_MAX;
}
// Get the score of mate in ply plies
constexpr Score mate_in(int ply) {
    return SCORE_MATE - ply;
}
// Get the score of mated in ply plies
constexpr Score mated_in(int ply) {
    return ply - SCORE_MATE;
}
// Get the ply distance to mate from a mate score
constexpr int mate_ply(Score s) {
    return s > 0 ? SCORE_MATE - s : SCORE_MATE + s;
}

// clang-format off
enum Square : int8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQUARE_NONE,
    SQUARE_COUNT = 64
};

enum File : int8_t {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H,
    FILE_COUNT = 8
};

enum Rank : int8_t {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8,
    RANK_COUNT = 8
};

enum Color : int8_t {
    WHITE, BLACK,
    COLOR_COUNT = 2
};

enum PieceType : int8_t {
    PIECE_TYPE_NONE,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PIECE_TYPE_COUNT = 7
};

enum Piece : int8_t {
    PIECE_NONE,
    WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
    PIECE_COUNT = 13
};
// clang-format on

enum CastlingRights : int8_t {
    CR_NONE = 0,
    CR_WHITE_KINGSIDE = 1 << 0,
    CR_WHITE_QUEENSIDE = 1 << 1,
    CR_BLACK_KINGSIDE = 1 << 2,
    CR_BLACK_QUEENSIDE = 1 << 3,
    CR_KINGSIDE = CR_WHITE_KINGSIDE | CR_BLACK_KINGSIDE,
    CR_QUEENSIDE = CR_WHITE_QUEENSIDE | CR_BLACK_QUEENSIDE,
    CR_WHITE = CR_WHITE_KINGSIDE | CR_WHITE_QUEENSIDE,
    CR_BLACK = CR_BLACK_KINGSIDE | CR_BLACK_QUEENSIDE,
    CR_ALL = CR_WHITE | CR_BLACK
};

enum Direction : int8_t {
    NORTH = 8,
    SOUTH = -8,
    WEST = -1,
    EAST = 1,
    NORTH_WEST = 7,
    NORTH_EAST = 9,
    SOUTH_WEST = -9,
    SOUTH_EAST = -7
};

// Square/File/Rank helpers
inline constexpr Square make_square(File f, Rank r) {
    return static_cast<Square>(r * 8 + f);
}

inline constexpr File get_file(Square sq) {
    return static_cast<File>(sq & 7);
}

inline constexpr Rank get_rank(Square sq) {
    return static_cast<Rank>(sq >> 3);
}

inline constexpr File& operator++(File& f) {
    return f = static_cast<File>(f + 1);
}

inline constexpr Rank& operator++(Rank& r) {
    return r = static_cast<Rank>(r + 1);
}

// Square direction helpers
inline constexpr Square operator+(Square sq, Direction d) {
    return static_cast<Square>(static_cast<int>(sq) + static_cast<int>(d));
}

inline constexpr Square operator-(Square sq, Direction d) {
    return static_cast<Square>(static_cast<int>(sq) - static_cast<int>(d));
}

inline constexpr Square& operator+=(Square& sq, Direction d) {
    return sq = static_cast<Square>(static_cast<int>(sq) + static_cast<int>(d));
}

inline constexpr Square& operator-=(Square& sq, Direction d) {
    return sq = static_cast<Square>(static_cast<int>(sq) - static_cast<int>(d));
}

// Color helpers
inline constexpr Color operator~(Color c) {
    return static_cast<Color>(c ^ 1);
}

inline constexpr Color& operator++(Color& c) {
    return c = static_cast<Color>(c + 1);
}

// Piece helpers
inline constexpr Piece make_piece(PieceType pt, Color c) {
    return static_cast<Piece>(pt + c * 6);
}

inline constexpr PieceType get_piece_type(Piece pc) {
    return static_cast<PieceType>(pc == PIECE_NONE ? PIECE_TYPE_NONE : ((pc - 1) % 6) + 1);
}

inline constexpr Color get_piece_color(Piece pc) {
    return static_cast<Color>((pc - 1) / 6);
}