#include "movegen.hpp"
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