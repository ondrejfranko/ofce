#pragma once

#include "types.hpp"
#include <cstdint>
#include <string>

// 16-bit move encoding:
// Bits 0-5:   from square (0-63)
// Bits 6-11:  to square (0-63)
// Bits 12-13: promotion piece type (knight, bishop, rook, queen)
// Bits 14-15: move type flag (normal, promotion, en passant, castling)

enum MoveTypeFlag : uint16_t {
    NORMAL_MOVE = 0,
    PROMO_MOVE = 1 << 14,
    EP_MOVE = 2 << 14,
    CASTLING_MOVE = 3 << 14
};

class Move {
  public:
    // Constructors
    constexpr Move() : data(0) {}
    constexpr Move(uint16_t d) : data(d) {}
    constexpr Move(Square from, Square to, MoveTypeFlag flag = NORMAL_MOVE) : data((from & 0x3F) | ((to & 0x3F) << 6) | flag) {}
    constexpr Move(Square from, Square to, PieceType promo_piece_type)
        : data((from & 0x3F) | ((to & 0x3F) << 6) | (((promo_piece_type - KNIGHT) & 0x3) << 12) | PROMO_MOVE) {}

    // Getters
    constexpr Square from() const {
        return static_cast<Square>(data & 0x3F);
    }
    constexpr Square to() const {
        return static_cast<Square>((data >> 6) & 0x3F);
    }
    constexpr PieceType promo_piece_type() const {
        return static_cast<PieceType>(((data >> 12) & 0x3) + KNIGHT);
    }
    constexpr MoveTypeFlag move_type() const {
        return static_cast<MoveTypeFlag>(data & 0xC000);
    }
    constexpr uint16_t raw() const {
        return data;
    }

    // Move type checkers
    constexpr bool is_normal() const {
        return move_type() == NORMAL_MOVE;
    }
    constexpr bool is_promo() const {
        return move_type() == PROMO_MOVE;
    }
    constexpr bool is_ep() const {
        return move_type() == EP_MOVE;
    }
    constexpr bool is_castling() const {
        return move_type() == CASTLING_MOVE;
    }
    constexpr bool is_null() const {
        return data == 0;
    }

    // Operators
    constexpr bool operator==(const Move &other) const {
        return data == other.data;
    }
    constexpr bool operator!=(const Move &other) const {
        return data != other.data;
    }

    // UCI string representation
    std::string to_string() const;

  private:
    uint16_t data;
};

inline constexpr Move MOVE_NULL = Move();