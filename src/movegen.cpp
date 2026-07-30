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
        generate_king_moves<WHITE>(pos, list, target);
        if (gen_type != GenType::CAPTURES)
            generate_castling_moves<WHITE>(pos, list);
    } else {
        generate_pawn_moves<BLACK>(pos, list, pawn_target);
        generate_knight_moves<BLACK>(pos, list, target);
        generate_bishop_moves<BLACK>(pos, list, target);
        generate_rook_moves<BLACK>(pos, list, target);
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
    const Bitboard enemy = pos.color_BB[~C];

    Bitboard push_1_BB, push_2_BB, cap_left_BB, cap_right_BB;
    Bitboard promo_rank, double_rank;
    Direction push_dir, cap_left_dir, cap_right_dir;

    if constexpr (C == WHITE) {
        push_1_BB = shift_N(pawns) & empty;
        push_2_BB = shift_N(shift_N(pawns & RANK_BB[RANK_2]) & empty) & empty;
        cap_left_BB = shift_NW(pawns) & enemy;
        cap_right_BB = shift_NE(pawns) & enemy;
        promo_rank = RANK_BB[RANK_8];
        double_rank = RANK_BB[RANK_4];
        push_dir = NORTH;
        cap_left_dir = NORTH_WEST;
        cap_right_dir = NORTH_EAST;
    } else {
        push_1_BB = shift_S(pawns) & empty;
        push_2_BB = shift_S(shift_S(pawns & RANK_BB[RANK_7]) & empty) & empty;
        cap_left_BB = shift_SW(pawns) & enemy;
        cap_right_BB = shift_SE(pawns) & enemy;
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

// Bishop (+ diagonal queen) move generation
template <Color C>
void generate_bishop_moves(const Position &pos, MoveList &list, Bitboard target) {
    constexpr Piece BISHOP_PIECE = make_piece(BISHOP, C);
    constexpr Piece QUEEN_PIECE = make_piece(QUEEN, C);
    const Bitboard occ = pos.color_BB[WHITE] | pos.color_BB[BLACK];
    Bitboard pieces = pos.piece_BB[BISHOP_PIECE] | pos.piece_BB[QUEEN_PIECE];
    while (pieces) {
        const Square from = pop_lsb(pieces);
        Bitboard attacks = get_bishop_attacks(from, occ) & target;
        while (attacks)
            list.add_move(Move(from, pop_lsb(attacks)));
    }
}

// Rook (+ orthogonal queen) move generation
template <Color C>
void generate_rook_moves(const Position &pos, MoveList &list, Bitboard target) {
    constexpr Piece ROOK_PIECE = make_piece(ROOK, C);
    constexpr Piece QUEEN_PIECE = make_piece(QUEEN, C);
    const Bitboard occ = pos.color_BB[WHITE] | pos.color_BB[BLACK];
    Bitboard pieces = pos.piece_BB[ROOK_PIECE] | pos.piece_BB[QUEEN_PIECE];
    while (pieces) {
        const Square from = pop_lsb(pieces);
        Bitboard attacks = get_rook_attacks(from, occ) & target;
        while (attacks)
            list.add_move(Move(from, pop_lsb(attacks)));
    }
}

// King move generation
template <Color C>
void generate_king_moves(const Position &pos, MoveList &list, Bitboard target) {
    constexpr Piece KING_PIECE = make_piece(KING, C);
    const Square ksq = get_lsb(pos.piece_BB[KING_PIECE]);
    Bitboard attacks = get_king_attacks(ksq) & target;
    while (attacks)
        list.add_move(Move(ksq, pop_lsb(attacks)));
}

// Castling move generation
template <Color C>
void generate_castling_moves(const Position &pos, MoveList &list) {
    constexpr Color THEM = ~C;

    if constexpr (C == WHITE) {
        // kingside
        if ((pos.castling_rights & CR_WHITE_KINGSIDE) && pos.squares[F1] == PIECE_NONE && pos.squares[G1] == PIECE_NONE &&
            !is_square_attacked<THEM>(pos, E1) && !is_square_attacked<THEM>(pos, F1) && !is_square_attacked<THEM>(pos, G1)) {
            list.add_move(Move(E1, G1, CASTLING_MOVE));
        }
        // queenside
        if ((pos.castling_rights & CR_WHITE_QUEENSIDE) && pos.squares[D1] == PIECE_NONE && pos.squares[C1] == PIECE_NONE &&
            pos.squares[B1] == PIECE_NONE && !is_square_attacked<THEM>(pos, E1) && !is_square_attacked<THEM>(pos, D1) &&
            !is_square_attacked<THEM>(pos, C1)) {
            list.add_move(Move(E1, C1, CASTLING_MOVE));
        }
    } else {
        // kingside
        if ((pos.castling_rights & CR_BLACK_KINGSIDE) && pos.squares[F8] == PIECE_NONE && pos.squares[G8] == PIECE_NONE &&
            !is_square_attacked<THEM>(pos, E8) && !is_square_attacked<THEM>(pos, F8) && !is_square_attacked<THEM>(pos, G8)) {
            list.add_move(Move(E8, G8, CASTLING_MOVE));
        }
        // queenside
        if ((pos.castling_rights & CR_BLACK_QUEENSIDE) && pos.squares[D8] == PIECE_NONE && pos.squares[C8] == PIECE_NONE &&
            pos.squares[B8] == PIECE_NONE && !is_square_attacked<THEM>(pos, E8) && !is_square_attacked<THEM>(pos, D8) &&
            !is_square_attacked<THEM>(pos, C8)) {
            list.add_move(Move(E8, C8, CASTLING_MOVE));
        }
    }
}

// Bitboard of all squares attacked by the given color
template <Color C>
Bitboard attacks_by(const Position &pos) {
    constexpr Piece PAWN_PIECE = make_piece(PAWN, C);
    constexpr Piece KNIGHT_PIECE = make_piece(KNIGHT, C);
    constexpr Piece BISHOP_PIECE = make_piece(BISHOP, C);
    constexpr Piece ROOK_PIECE = make_piece(ROOK, C);
    constexpr Piece QUEEN_PIECE = make_piece(QUEEN, C);
    constexpr Piece KING_PIECE = make_piece(KING, C);
    const Bitboard occ = pos.color_BB[WHITE] | pos.color_BB[BLACK];
    Bitboard attacks = 0;

    // pawns
    {
        Bitboard bb = pos.piece_BB[PAWN_PIECE];
        if constexpr (C == WHITE)
            attacks |= shift_NW(bb) | shift_NE(bb);
        else
            attacks |= shift_SW(bb) | shift_SE(bb);
    }

    // knights
    {
        Bitboard bb = pos.piece_BB[KNIGHT_PIECE];
        while (bb)
            attacks |= get_knight_attacks(pop_lsb(bb));
    }

    // bishops + queens (diagonal)
    {
        Bitboard bb = pos.piece_BB[BISHOP_PIECE] | pos.piece_BB[QUEEN_PIECE];
        while (bb)
            attacks |= get_bishop_attacks(pop_lsb(bb), occ);
    }

    // rooks + queens (orthogonal)
    {
        Bitboard bb = pos.piece_BB[ROOK_PIECE] | pos.piece_BB[QUEEN_PIECE];
        while (bb)
            attacks |= get_rook_attacks(pop_lsb(bb), occ);
    }

    // king
    attacks |= get_king_attacks(get_lsb(pos.piece_BB[KING_PIECE]));

    return attacks;
}

// Bitboard of enemy pieces checking the king
Bitboard checkers_to(const Position &pos, Color king_color) {
    const Color them = ~king_color;
    const Square ksq = get_lsb(pos.piece_BB[make_piece(KING, king_color)]);
    const Bitboard occ = pos.color_BB[WHITE] | pos.color_BB[BLACK];

    return (get_pawn_attacks(king_color, ksq) & pos.piece_BB[make_piece(PAWN, them)]) |
           (get_knight_attacks(ksq) & pos.piece_BB[make_piece(KNIGHT, them)]) |
           (get_bishop_attacks(ksq, occ) & (pos.piece_BB[make_piece(BISHOP, them)] | pos.piece_BB[make_piece(QUEEN, them)])) |
           (get_rook_attacks(ksq, occ) & (pos.piece_BB[make_piece(ROOK, them)] | pos.piece_BB[make_piece(QUEEN, them)]));
}

// Check if square is attacked by a piece of colour C
template <Color C>
bool is_square_attacked(const Position &pos, Square sq) {
    constexpr Color THEM = ~C;
    const Bitboard occ = pos.color_BB[WHITE] | pos.color_BB[BLACK];

    // non-sliding: (use other color's pawn table to locate C's pawns)
    if (get_pawn_attacks(THEM, sq) & pos.piece_BB[make_piece(PAWN, C)])
        return true;
    if (get_knight_attacks(sq) & pos.piece_BB[make_piece(KNIGHT, C)])
        return true;
    if (get_king_attacks(sq) & pos.piece_BB[make_piece(KING, C)])
        return true;

    // sliding (diagonal)
    if (get_bishop_attacks(sq, occ) & (pos.piece_BB[make_piece(BISHOP, C)] | pos.piece_BB[make_piece(QUEEN, C)]))
        return true;

    // sliding (orthogonal)
    if (get_rook_attacks(sq, occ) & (pos.piece_BB[make_piece(ROOK, C)] | pos.piece_BB[make_piece(QUEEN, C)]))
        return true;

    return false;
}

// Bitboard of pinned pieces of specified color
Bitboard pinned_pieces(const Position &pos, Color color) {
    const Color them = ~color;
    const Square ksq = get_lsb(pos.piece_BB[make_piece(KING, color)]);
    const Bitboard occ = pos.color_BB[WHITE] | pos.color_BB[BLACK];
    const Bitboard us_bb = pos.color_BB[color];
    const Bitboard them_bb = pos.color_BB[them];

    // occupancy with our pieces removed reveals x-ray attackers
    const Bitboard xray_occ = occ & ~us_bb;
    Bitboard pinned = 0;

    // diagonal x-ray attackers (bishops and queens)
    {
        Bitboard snipers = get_bishop_attacks(ksq, xray_occ) & (pos.piece_BB[make_piece(BISHOP, them)] | pos.piece_BB[make_piece(QUEEN, them)]);
        while (snipers) {
            const Square sq = pop_lsb(snipers);
            const Bitboard between = BETWEEN_BB[ksq][sq] & occ;
            // exactly one friendly piece and no enemy pieces between king and x-ray attacker
            if (popcount(between & us_bb) == 1 && (between & them_bb) == 0)
                pinned |= (between & us_bb);
        }
    }

    // orthogonal x-ray attackers (rooks and queens)
    {
        Bitboard snipers = get_rook_attacks(ksq, xray_occ) & (pos.piece_BB[make_piece(ROOK, them)] | pos.piece_BB[make_piece(QUEEN, them)]);
        while (snipers) {
            const Square sq = pop_lsb(snipers);
            const Bitboard between = BETWEEN_BB[ksq][sq] & occ;
            if (popcount(between & us_bb) == 1 && (between & them_bb) == 0)
                pinned |= (between & us_bb);
        }
    }

    return pinned;
}