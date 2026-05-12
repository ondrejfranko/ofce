#include "position.hpp"
#include "types.hpp"
#include <print>

void print_position(const Position &pos) {
    const char *ascii_pieces = " PNBRQKpnbrqk";

    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        std::println("  +---+---+---+---+---+---+---+---+");
        std::print("{} |", rank + 1);
        for (int file = FILE_A; file <= FILE_H; ++file) {
            Square sq = static_cast<Square>(rank * 8 + file);
            std::print(" {} |", ascii_pieces[pos.squares[sq]]);
        }
        std::println();
    }
    std::println("  +---+---+---+---+---+---+---+---+");
}