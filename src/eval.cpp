#include "eval.hpp"
#include "bitboard.hpp"
#include "types.hpp"

// Piece-square tables (including base piece values)
ScorePair PSQT[PIECE_COUNT][SQUARE_COUNT]{};

// clang-format off
// Piece type material values
inline constexpr std::array<ScorePair, PIECE_TYPE_COUNT> PIECE_VALUE = {
    S(0, 0), S(100, 100), S(300, 300), S(300, 300), S(500, 500), S(900, 900), S(0, 0)
};

// Piece value square placement adjustments
constexpr std::array<int16_t, 64> PAWN_TABLE_MG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> PAWN_TABLE_EG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> KNIGHT_TABLE_MG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> KNIGHT_TABLE_EG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> BISHOP_TABLE_MG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> BISHOP_TABLE_EG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> ROOK_TABLE_MG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> ROOK_TABLE_EG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> QUEEN_TABLE_MG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> QUEEN_TABLE_EG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> KING_TABLE_MG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr std::array<int16_t, 64> KING_TABLE_EG = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
// clang-format on

constexpr std::array<std::array<int16_t, 64>, PIECE_TYPE_COUNT> PSQT_MG = [] {
    std::array<std::array<int16_t, 64>, PIECE_TYPE_COUNT> a{};
    a[PAWN] = PAWN_TABLE_MG;
    a[KNIGHT] = KNIGHT_TABLE_MG;
    a[BISHOP] = BISHOP_TABLE_MG;
    a[ROOK] = ROOK_TABLE_MG;
    a[QUEEN] = QUEEN_TABLE_MG;
    a[KING] = KING_TABLE_MG;
    return a;
}();
constexpr std::array<std::array<int16_t, 64>, PIECE_TYPE_COUNT> PSQT_EG = [] {
    std::array<std::array<int16_t, 64>, PIECE_TYPE_COUNT> a{};
    a[PAWN] = PAWN_TABLE_EG;
    a[KNIGHT] = KNIGHT_TABLE_EG;
    a[BISHOP] = BISHOP_TABLE_EG;
    a[ROOK] = ROOK_TABLE_EG;
    a[QUEEN] = QUEEN_TABLE_EG;
    a[KING] = KING_TABLE_EG;
    return a;
}();

// Struct holding all information needed for evaluation
struct EvalInfo {
    const Position &pos;
    // TODO: add more fields that would otherwise be recomputed multiple times
};

EvalInfo make_eval_info(const Position &pos) {
    EvalInfo info{pos};

    // TODO: compute and fill in the rest of the fields

    return info;
}

// Startup initialisation of evaluation
void init_eval() {
    // fill PSQT tables with base piece values and square placement adjustments
    for (int pt = PAWN; pt <= KING; ++pt) {
        for (int sq = A1; sq < SQUARE_COUNT; ++sq) {
            const ScorePair w = PIECE_VALUE[pt] + S(PSQT_MG[pt][sq], PSQT_EG[pt][sq]);
            PSQT[make_piece(static_cast<PieceType>(pt), WHITE)][sq] = w;
            PSQT[make_piece(static_cast<PieceType>(pt), BLACK)][sq ^ 56] = w;
        }
    }

    return;
}

// Calculate phase based on non-pawn material
int game_phase(const Position &pos) {
    int p = 0;
    p += popcount(pos.piece_BB[WHITE_KNIGHT] | pos.piece_BB[BLACK_KNIGHT]) * PHASE_WEIGHT[KNIGHT];
    p += popcount(pos.piece_BB[WHITE_BISHOP] | pos.piece_BB[BLACK_BISHOP]) * PHASE_WEIGHT[BISHOP];
    p += popcount(pos.piece_BB[WHITE_ROOK] | pos.piece_BB[BLACK_ROOK]) * PHASE_WEIGHT[ROOK];
    p += popcount(pos.piece_BB[WHITE_QUEEN] | pos.piece_BB[BLACK_QUEEN]) * PHASE_WEIGHT[QUEEN];
    return p;
}

template <Color C>
ScorePair evaluate_psqt(const EvalInfo &info) {
    ScorePair v = S(0, 0);

    for (int sq = A1; sq < SQUARE_COUNT; ++sq) {
        const Piece p = info.pos.squares[sq];

        if (p != PIECE_NONE && piece_color(p) == C) {
            v += PSQT[p][sq];
        }
    }

    return v;
}

// Static evaluation from POV of the side to move
int evaluate(const Position &pos) {
    const EvalInfo info = make_eval_info(pos);

    // compute evaluation terms and combine them into a final score
    ScorePair sp = evaluate_psqt<WHITE>(info) - evaluate_psqt<BLACK>(info);

    int eval = interpolate(sp, game_phase(pos));

    return eval;
}