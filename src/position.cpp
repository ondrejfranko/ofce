#include "position.hpp"
#include "types.hpp"
#include <print>
#include <string_view>

void clear_square(Position &pos, Square square) {
    pos.squares[square] = PIECE_NONE;
}

void set_square(Position &pos, Square square, Piece piece) {
    pos.squares[square] = piece;
}

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