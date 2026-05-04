#include "bitboard.hpp"
#include "types.hpp"
#include <print>

void print_bitboard(Bitboard bb) {
    std::println("  A B C D E F G H");
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        std::print("{} ", rank + 1);
        for (int file = FILE_A; file <= FILE_H; ++file) {
            Square sq = static_cast<Square>(rank * 8 + file);
            std::print("{} ", test_bit(bb, sq) ? "X" : ".");
        }
        std::println("{}", rank + 1);
    }
    std::println("  A B C D E F G H");

    std::println("{:#018x}\n", bb);
}