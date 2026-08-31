#include "search.hpp"
#include "eval.hpp"
#include "makemove.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"

Score negamax(Searcher &s, const Position &pos, Score alpha, Score beta, int depth, int ply) {
    // TODO
}

Score quiescence(Searcher &s, const Position &pos, Score alpha, Score beta, int ply) {
    // TODO
}

Move search_root(Searcher &s, const Position &root, const std::vector<Key> &game_history, int depth, Score *score_out) {
    // TODO
}