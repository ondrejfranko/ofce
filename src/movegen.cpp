#include "movegen.hpp"
#include "attacks.hpp"
#include "bitboard.hpp"
#include "position.hpp"
#include "types.hpp"

// get valid landing squares
static Bitboard target_mask(const Position &pos, Color us, GenType gt) {
    const Color them = static_cast<Color>(us ^ 1);
    const Bitboard enemy = pos.color_BB[them];
    const Bitboard empty = ~(pos.color_BB[WHITE] | pos.color_BB[BLACK]);
    switch (gt) {
        case GenType::CAPTURES: return enemy;
        case GenType::QUIETS: return empty;
        default: return ~pos.color_BB[us];
    }
}

// Pseudo-legal move generation
void generate_moves(const Position &pos, MoveList &list, GenType gen_type) {
    const Color us = pos.side_to_move;
    const Bitboard target = target_mask(pos, us, gen_type);

    // Possibly add the (empty) EP square to pawn targets
    Bitboard pawn_target = target;
    if (gen_type != GenType::QUIETS && pos.en_passant_square != SQUARE_NONE)
        pawn_target |= bit(pos.en_passant_square);

    if (us == WHITE) {
        generate_pawn_moves<WHITE>(pos, list, pawn_target);
        generate_knight_moves<WHITE>(pos, list, target);
        generate_bishop_moves<WHITE>(pos, list, target);
        generate_rook_moves<WHITE>(pos, list, target);
        generate_queen_moves<WHITE>(pos, list, target);
        generate_king_moves<WHITE>(pos, list, target);
        if (gen_type != GenType::CAPTURES)
            generate_castling_moves<WHITE>(pos, list);
    } else {
        generate_pawn_moves<BLACK>(pos, list, pawn_target);
        generate_knight_moves<BLACK>(pos, list, target);
        generate_bishop_moves<BLACK>(pos, list, target);
        generate_rook_moves<BLACK>(pos, list, target);
        generate_queen_moves<BLACK>(pos, list, target);
        generate_king_moves<BLACK>(pos, list, target);
        if (gen_type != GenType::CAPTURES)
            generate_castling_moves<BLACK>(pos, list);
    }
}

// Pawn move generation
template <Color C>
void generate_pawn_moves(const Position &pos, MoveList &list, Bitboard target) {
    constexpr Piece PAWN_PIECE = make_piece(PAWN, C);
    const Bitboard pawns = pos.piece_BB[PAWN_PIECE];
    const Bitboard empty = ~(pos.color_BB[WHITE] | pos.color_BB[BLACK]);

    Bitboard push_1_BB, push_2_BB, cap_left_BB, cap_right_BB;
    Bitboard promo_rank, double_rank;
    Direction push_dir, cap_left_dir, cap_right_dir;

    if constexpr (C == WHITE) {
        push_1_BB = shift_N(pawns) & empty;
        push_2_BB = shift_N(shift_N(pawns & RANK_BB[RANK_2]) & empty) & empty;
        cap_left_BB = shift_NW(pawns);
        cap_right_BB = shift_NE(pawns);
        promo_rank = RANK_BB[RANK_8];
        double_rank = RANK_BB[RANK_4];
        push_dir = NORTH;
        cap_left_dir = NORTH_WEST;
        cap_right_dir = NORTH_EAST;
    } else {
        push_1_BB = shift_S(pawns) & empty;
        push_2_BB = shift_S(shift_S(pawns & RANK_BB[RANK_7]) & empty) & empty;
        cap_left_BB = shift_SW(pawns);
        cap_right_BB = shift_SE(pawns);
        promo_rank = RANK_BB[RANK_1];
        double_rank = RANK_BB[RANK_5];
        push_dir = SOUTH;
        cap_left_dir = SOUTH_WEST;
        cap_right_dir = SOUTH_EAST;
    }

    // regular (non-promotion) pushes
    {
        Bitboard bb = push_1_BB & target & ~promo_rank;
        while (bb) {
            const Square to = pop_lsb(bb);
            list.add_move(Move(static_cast<Square>(to - push_dir), to));
        }
    }

    // double pushes
    {
        Bitboard bb = push_2_BB & target;
        while (bb) {
            const Square to = pop_lsb(bb);
            list.add_move(Move(static_cast<Square>(to - push_dir - push_dir), to));
        }
    }

    // promotion pushes
    {
        Bitboard bb = push_1_BB & target & promo_rank;
        while (bb) {
            const Square to = pop_lsb(bb);
            const Square from = static_cast<Square>(to - push_dir);
            for (const PieceType pt : {KNIGHT, BISHOP, ROOK, QUEEN})
                list.add_move(Move(from, to, pt));
        }
    }

    // regular (non-promotion) captures (including en passant)
    {
        Bitboard bb = cap_left_BB & target & ~promo_rank;
        while (bb) {
            const Square to = pop_lsb(bb);
            const Square from = static_cast<Square>(to - cap_left_dir);
            if (to == pos.en_passant_square)
                list.add_move(Move(from, to, EP_MOVE));
            else
                list.add_move(Move(from, to));
        }
    }
    {
        Bitboard bb = cap_right_BB & target & ~promo_rank;
        while (bb) {
            const Square to = pop_lsb(bb);
            const Square from = static_cast<Square>(to - cap_right_dir);
            if (to == pos.en_passant_square)
                list.add_move(Move(from, to, EP_MOVE));
            else
                list.add_move(Move(from, to));
        }
    }

    // promotion captures
    {
        Bitboard bb = cap_left_BB & target & promo_rank;
        while (bb) {
            const Square to = pop_lsb(bb);
            const Square from = static_cast<Square>(to - cap_left_dir);
            for (const PieceType pt : {KNIGHT, BISHOP, ROOK, QUEEN})
                list.add_move(Move(from, to, pt));
        }
    }
    {
        Bitboard bb = cap_right_BB & target & promo_rank;
        while (bb) {
            const Square to = pop_lsb(bb);
            const Square from = static_cast<Square>(to - cap_right_dir);
            for (const PieceType pt : {KNIGHT, BISHOP, ROOK, QUEEN})
                list.add_move(Move(from, to, pt));
        }
    }
}

// Knight move generation
template <Color C>
void generate_knight_moves(const Position &pos, MoveList &list, Bitboard target) {
    constexpr Piece KNIGHT_PIECE = make_piece(KNIGHT, C);
    Bitboard pieces = pos.piece_BB[KNIGHT_PIECE];
    while (pieces) {
        const Square from = pop_lsb(pieces);
        Bitboard attacks = get_knight_attacks(from) & target;
        while (attacks)
            list.add_move(Move(from, pop_lsb(attacks)));
    }
}