#pragma once

#include "types.hpp"
#include <string_view>

struct alignas(64) Position {
    Piece squares[SQUARE_COUNT];
    Bitboard piece_BB[PIECE_COUNT];
    Bitboard color_BB[COLOR_COUNT];

    Color side_to_move;
    int8_t castling_rights;
    Square en_passant_square;
    int fifty_move_rule_ply;
    int game_ply;

    Key zobrist_key;

    // TODO: auxiliary bitboards
};

constexpr Piece char_to_piece(char c) {
    switch (c) {
        case 'P': return WHITE_PAWN;
        case 'N': return WHITE_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'R': return WHITE_ROOK;
        case 'Q': return WHITE_QUEEN;
        case 'K': return WHITE_KING;
        case 'p': return BLACK_PAWN;
        case 'n': return BLACK_KNIGHT;
        case 'b': return BLACK_BISHOP;
        case 'r': return BLACK_ROOK;
        case 'q': return BLACK_QUEEN;
        case 'k': return BLACK_KING;
        default: return PIECE_NONE;
    }
}

constexpr Color piece_color(Piece p) {
    if (p <= WHITE_KING) {
        return WHITE;
    } else {
        return BLACK;
    }
}

void clear_square(Position &pos, Square square);
void set_square(Position &pos, Square square, Piece piece);

void clear_position(Position &pos);
void set_position(Position &pos, std::string_view fen);
void print_position(const Position &pos);