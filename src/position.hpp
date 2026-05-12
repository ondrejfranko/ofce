#pragma once

#include "types.hpp"
// #include <string_view>

struct alignas(64) Position {
    Piece squares[SQUARE_COUNT];
    Bitboard piece_BB[PIECE_COUNT];
    Bitboard color_BB[COLOR_COUNT];

    Color side_to_move;
    CastlingRights castling_rights;
    Square en_passant_square;
    int fifty_move_rule_ply;
    int game_ply;

    Key zobrist_key;

    // TODO: auxiliary bitboards
};

void print_position(const Position &pos);