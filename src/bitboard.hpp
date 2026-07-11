#pragma once

#include "types.hpp"
#include <array>
#include <bit>
#include <cassert>

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
    assert(A1 <= sq && sq < SQUARE_COUNT);
    assert((bb >> sq) & 1ULL);

    bb &= ~(1ULL << sq);
}

constexpr void set_bit(Bitboard &bb, Square sq) {
    assert(A1 <= sq && sq < SQUARE_COUNT);
    assert(((bb >> sq) & 1ULL) == 0);

    bb |= (1ULL << sq);
}

constexpr bool test_bit(Bitboard bb, Square sq) {
    assert(A1 <= sq && sq < SQUARE_COUNT);

    return (bb >> sq) & 1ULL;
}

constexpr int popcount(Bitboard bb) {
    return std::popcount(bb);
}

constexpr Square get_lsb(Bitboard bb) {
    assert(bb != 0);

    return static_cast<Square>(std::countr_zero(bb));
}

constexpr Square get_msb(Bitboard bb) {
    assert(bb != 0);

    return static_cast<Square>(63 - std::countl_zero(bb));
}

constexpr Square pop_lsb(Bitboard &bb) {
    assert(bb != 0);

    Square sq = get_lsb(bb);
    bb &= bb - 1;
    return sq;
}

constexpr Square pop_msb(Bitboard &bb) {
    assert(bb != 0);

    Square sq = get_msb(bb);
    bb ^= 1ULL << sq;
    return sq;
}

constexpr Bitboard shift_N(Bitboard bb) {
    return bb << 8;
}

constexpr Bitboard shift_S(Bitboard bb) {
    return bb >> 8;
}

constexpr Bitboard shift_W(Bitboard bb) {
    return (bb >> 1) & ~FILE_BB[FILE_H];
}

constexpr Bitboard shift_E(Bitboard bb) {
    return (bb << 1) & ~FILE_BB[FILE_A];
}

constexpr Bitboard shift_NW(Bitboard bb) {
    return (bb << 7) & ~FILE_BB[FILE_H];
}

constexpr Bitboard shift_NE(Bitboard bb) {
    return (bb << 9) & ~FILE_BB[FILE_A];
}

constexpr Bitboard shift_SW(Bitboard bb) {
    return (bb >> 9) & ~FILE_BB[FILE_H];
}

constexpr Bitboard shift_SE(Bitboard bb) {
    return (bb >> 7) & ~FILE_BB[FILE_A];
}

void print_bitboard(Bitboard bb);