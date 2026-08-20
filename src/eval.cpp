#include "eval.hpp"
#include "types.hpp"

// Piece-square tables (including base piece values)
ScorePair PSQT[PIECE_COUNT][SQUARE_COUNT]{};

// Global pawn structure hash table
PawnTable PAWN_TABLE;

// Struct holding all information needed for evaluation
struct EvalInfo {
    const Position &pos;
    const PawnEntry *pe;
    // TODO: add more fields that would otherwise be recomputed multiple times
};

EvalInfo make_eval_info(const Position &pos, const PawnEntry &pe) {
    EvalInfo info{pos, &pe};

    // TODO: compute and fill in the rest of the fields

    return info;
}

// Startup initialisation of evaluation
void init_eval() {
    // TODO: fill PSQT with piece-square tables and base piece values

    return;
}

// Static evaluation from POV of the side to move
int evaluate(const Position &pos) {
    const PawnEntry *pe = PAWN_TABLE.probe(pos);
    const EvalInfo info = make_eval_info(pos, *pe);

    // TODO: compute evaluation terms and combine them into a final score

    return 0;
}