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

Position make_move(const Position &pos, Move move) {
    Position next = pos;

    const Color us = pos.side_to_move;
    const Square from = move.from();
    const Square to = move.to();
    const Piece moving = pos.squares[from];
    const Piece captured = pos.squares[to];

    assert(!move.is_null());
    assert(moving != PIECE_NONE && piece_color(moving) == us);
    assert(get_piece_type(captured) != KING);

    // Remove information from the zobrist key that will change (piece-square will be updated later)
    next.zobrist_key ^= ZOBRIST_CASTLING[pos.castling_rights & 0xF];
    if (pos.en_passant_square != SQUARE_NONE) {
        next.zobrist_key ^= ZOBRIST_EP[get_file(pos.en_passant_square)];
    }

    // Update castling rights
    next.castling_rights &= CASTLING_RIGHTS_MASK[from];
    next.castling_rights &= CASTLING_RIGHTS_MASK[to];
    next.zobrist_key ^= ZOBRIST_CASTLING[next.castling_rights & 0xF];

    // Update en passant square
    next.en_passant_square = SQUARE_NONE;
    if (get_piece_type(moving) == PAWN && (to - from == 2 * NORTH || from - to == 2 * NORTH)) {
        next.en_passant_square = from + (us == WHITE ? NORTH : SOUTH);
        next.zobrist_key ^= ZOBRIST_EP[get_file(next.en_passant_square)];
    }

    // Update 50 move rule clock
    if (get_piece_type(moving) == PAWN || captured != PIECE_NONE || move.is_ep()) {
        next.fifty_move_rule_ply = 0;
    } else {
        ++next.fifty_move_rule_ply;
    }

    // Move the piece to the target square (removes captured piece if any)
    clear_square(next, from);
    if (move.is_ep()) {
        clear_square(next, to + (us == WHITE ? SOUTH : NORTH));
    }
    set_square(next, to, move.is_promo() ? make_piece(move.promo_piece_type(), us) : moving);

    // Handle castling moves (move the rook)
    if (move.is_castling()) {
        Square rook_from;
        Square rook_to;
        switch (to) {
            case G1:
                rook_from = H1;
                rook_to = F1;
                break;
            case C1:
                rook_from = A1;
                rook_to = D1;
                break;
            case G8:
                rook_from = H8;
                rook_to = F8;
                break;
            default:
                assert(to == C8);
                rook_from = A8;
                rook_to = D8;
                break;
        }
        const Piece rook = pos.squares[rook_from];
        assert(get_piece_type(rook) == ROOK && piece_color(rook) == us);
        clear_square(next, rook_from);
        set_square(next, rook_to, rook);
    }

    // Update side to move, game ply, and side to move zobrist
    next.side_to_move = ~us;
    ++next.game_ply;
    next.zobrist_key ^= ZOBRIST_SIDE_TO_MOVE;

    return next;
}

bool is_king_attacked(const Position &pos, Color us) {
    const Square ksq = get_lsb(pos.piece_BB[make_piece(KING, us)]);
    return (us == WHITE) ? is_square_attacked<BLACK>(pos, ksq) : is_square_attacked<WHITE>(pos, ksq);
}

bool is_legal_move(const Position &pos, Move move) {
    return !is_king_attacked(make_move(pos, move), pos.side_to_move);
}