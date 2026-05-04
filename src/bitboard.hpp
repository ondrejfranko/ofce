#pragma once

#include "types.hpp"
#include <array>
#include <bit>

inline constexpr std::array<Bitboard, FILE_COUNT> FILE_BB = []() {
    std::array<Bitboard, FILE_COUNT> arr{};
    for (int file = 0; file < FILE_COUNT; ++file) {
        arr[file] = 0x0101010101010101ULL << file;
    }
    return arr;
}();

inline constexpr std::array<Bitboard, RANK_COUNT> RANK_BB = []() {
    std::array<Bitboard, RANK_COUNT> arr{};
    for (int rank = 0; rank < RANK_COUNT; ++rank) {
        arr[rank] = 0x00000000000000FFULL << (rank * 8);
    }
    return arr;
}();

constexpr Bitboard bit(Square sq) {
    return 1ULL << sq;
}

constexpr void clear_bit(Bitboard &bb, Square sq) {
    bb &= ~(1ULL << sq);
}

constexpr void set_bit(Bitboard &bb, Square sq) {
    bb |= (1ULL << sq);
}

constexpr bool test_bit(Bitboard bb, Square sq) {
    return (bb >> sq) & 1ULL;
}

constexpr int popcount(Bitboard bb) {
    return std::popcount(bb);
}

constexpr Square get_lsb(Bitboard bb) {
    return static_cast<Square>(std::countr_zero(bb));
}

constexpr Square get_msb(Bitboard bb) {
    return static_cast<Square>(63 - std::countl_zero(bb));
}

constexpr Square pop_lsb(Bitboard &bb) {
    Square sq = get_lsb(bb);
    bb &= bb - 1;
    return sq;
}

constexpr Square pop_msb(Bitboard &bb) {
    Square sq = get_msb(bb);
    clear_bit(bb, sq);
    return sq;
}

constexpr void shift_N(Bitboard &bb) {
    bb <<= 8;
}

constexpr void shift_S(Bitboard &bb) {
    bb >>= 8;
}

constexpr void shift_W(Bitboard &bb) {
    bb = (bb >> 1) & ~FILE_BB[FILE_H];
}

constexpr void shift_E(Bitboard &bb) {
    bb = (bb << 1) & ~FILE_BB[FILE_A];
}

constexpr void shift_NW(Bitboard &bb) {
    bb = (bb << 7) & ~FILE_BB[FILE_H];
}

constexpr void shift_NE(Bitboard &bb) {
    bb = (bb << 9) & ~FILE_BB[FILE_A];
}

constexpr void shift_SW(Bitboard &bb) {
    bb = (bb >> 9) & ~FILE_BB[FILE_H];
}

constexpr void shift_SE(Bitboard &bb) {
    bb = (bb >> 7) & ~FILE_BB[FILE_A];
}

void print_bitboard(Bitboard bb);