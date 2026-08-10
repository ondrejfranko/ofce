#include "makemove.hpp"
#include "bitboard.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"
#include "zobrist.hpp"
#include <array>
#include <cassert>
#include <cstdint>

// the castling rights that remain after the square has been the origin or destination of a move
constexpr std::array<int8_t, SQUARE_COUNT> build_castling_rights_mask() {
    std::array<int8_t, SQUARE_COUNT> mask{};
    mask.fill(static_cast<int8_t>(CR_ALL));
    mask[E1] = static_cast<int8_t>(CR_ALL & ~CR_WHITE);
    mask[H1] = static_cast<int8_t>(CR_ALL & ~CR_WHITE_KINGSIDE);
    mask[A1] = static_cast<int8_t>(CR_ALL & ~CR_WHITE_QUEENSIDE);
    mask[E8] = static_cast<int8_t>(CR_ALL & ~CR_BLACK);
    mask[H8] = static_cast<int8_t>(CR_ALL & ~CR_BLACK_KINGSIDE);
    mask[A8] = static_cast<int8_t>(CR_ALL & ~CR_BLACK_QUEENSIDE);
    return mask;
}

constexpr auto CASTLING_RIGHTS_MASK = build_castling_rights_mask();

bool is_king_attacked(const Position &pos, Color us) {
    const Square ksq = get_lsb(pos.piece_BB[make_piece(KING, us)]);
    return (us == WHITE) ? is_square_attacked<BLACK>(pos, ksq) : is_square_attacked<WHITE>(pos, ksq);
}

bool is_legal_move(const Position &pos, Move move) {
    return !is_king_attacked(make_move(pos, move), pos.side_to_move);
}