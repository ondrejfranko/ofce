#pragma once

#include "move.hpp"
#include "position.hpp"
#include "types.hpp"
#include <cstdint>
#include <vector>

// Search state struct
struct Searcher {
    std::vector<Key> path_keys; // all zobrist keys on the search path for repetition detection
    uint64_t nodes = 0;         // node count
    int seldepth = 0;           // deepest ply reached (including quiescence)
    // TODO: add more as needed
};

// Main negamax alpha-beta search
Score negamax(Searcher &s, const Position &pos, Score alpha, Score beta, int depth, int ply);

// Quiescence search
Score quiescence(Searcher &s, const Position &pos, Score alpha, Score beta, int ply);

// Search start to fixed depth
Move search_root(Searcher &s, const Position &root, const std::vector<Key> &game_history, int depth, Score *score_out = nullptr);