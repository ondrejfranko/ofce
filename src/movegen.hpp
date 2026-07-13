#pragma once

#include "types.hpp"
#include "move.hpp"

struct Position; // forward declaration

// Classes of moves to generate
enum class GenType : uint8_t {
    ALL,
    CAPTURES,
    QUIETS,
};

// Generate pseudo-legal moves
void generate_moves(const Position &pos, MoveList &list, GenType gen_type = GenType::ALL);

// Per-piece-type generators (target restricts possible destination squares)
template <Color C>
void generate_pawn_moves(const Position &pos, MoveList &list, Bitboard target);

template <Color C>
void generate_knight_moves(const Position &pos, MoveList &list, Bitboard target);

template <Color C>
void generate_bishop_moves(const Position &pos, MoveList &list, Bitboard target);

template <Color C>
void generate_rook_moves(const Position &pos, MoveList &list, Bitboard target);

template <Color C>
void generate_queen_moves(const Position &pos, MoveList &list, Bitboard target);

template <Color C>
void generate_king_moves(const Position &pos, MoveList &list, Bitboard target);

template <Color C>
void generate_castling_moves(const Position &pos, MoveList &list);

// Bitboard of all squares attacked by the given colour
template <Color C>
Bitboard attacks_by(const Position &pos);

// Bitboard of enemy pieces checking the king
Bitboard checkers_to(const Position &pos, Color king_color);

// Bitboard of pinned pieces of specified color
Bitboard pinned_pieces(const Position &pos, Color color);

// True if square is attacked by the specified color
template <Color C>
bool is_square_attacked(const Position &pos, Square sq);

// True if king of specified color is in check
inline bool is_in_check(const Position &pos, Color color) {
    return checkers_to(pos, color) != 0;
}