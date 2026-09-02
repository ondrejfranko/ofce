#include "search.hpp"
#include "eval.hpp"
#include "makemove.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"

Score negamax(Searcher &s, const Position &pos, Score alpha, Score beta, int depth, int ply) {
    // Quiescence search
    if (depth <= 0) {
        return quiescence(s, pos, alpha, beta, ply);
    }

    // Update search statistics
    ++s.nodes;
    if (ply > s.seldepth) {
        s.seldepth = ply;
    }

    // Search depth limit check
    if (ply >= MAX_PLY_DEPTH - 2) {
        return evaluate(pos);
    }

    const Color us = pos.side_to_move;
    const bool in_check = is_in_check(pos, us);

    // Generate moves
    MoveList list;
    generate_moves(pos, list, GenType::ALL);

    int legal_count = 0;
    Score best = -SCORE_NONE;

    // Iterate over moves in the position
    for (int i = 0; i < list.size(); ++i) {
        Position next = make_move(pos, list[i]);

        // Check legality and skip illegal moves
        if (is_king_attacked(next, us)) {
            continue;
        }
        ++legal_count;

        // Search the child node
        s.path_keys.push_back(next.zobrist_key);
        Score score = -negamax(s, next, -beta, -alpha, depth - 1, ply + 1);
        s.path_keys.pop_back();

        // Compare alpha and beta bounds
        if (score > best) {
            // Better than best score so far
            best = score;
        }
        if (score > alpha) {
            // Better than alpha
            alpha = score;
            if (alpha >= beta) {
                // Beta cutoff
                break;
            }
        }
    }

    // If a position has no legal moves, it is checkmate or stalemate
    if (legal_count == 0) {
        return in_check ? mated_in(ply) : SCORE_DRAW;
    }

    return best;
}

Score quiescence(Searcher &s, const Position &pos, Score alpha, Score beta, int ply) {
    // TODO
}

Move search_root(Searcher &s, const Position &root, const std::vector<Key> &game_history, int depth, Score *score_out) {
    // TODO
}