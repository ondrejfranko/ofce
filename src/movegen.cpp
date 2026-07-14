#include "movegen.hpp"
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