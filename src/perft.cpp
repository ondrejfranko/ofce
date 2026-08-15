#include "perft.hpp"
#include "makemove.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"
#include <cassert>
#include <cstdint>
#include <print>

// perft (leaf node count of the position tree)
uint64_t perft(const Position &pos, int depth) {
    if (depth <= 0) {
        return 1ULL;
    }

    MoveList list;
    generate_moves(pos, list, GenType::ALL);

    const Color us = pos.side_to_move;
    uint64_t nodes = 0;

    for (int i = 0; i < list.size(); ++i) {
        const Position next = make_move(pos, list[i]);

        // Check for move legality
        if (is_king_attacked(next, us)) {
            continue;
        }

        // Recursive calling, bulk count leaves at depth 1
        nodes += (depth == 1) ? 1ULL : perft(next, depth - 1);
    }

    return nodes;
}

// perft divide (leaf node count of every legal root move)
uint64_t perft_divide(const Position &pos, int depth) {
    assert(depth >= 1);

    MoveList list;
    generate_moves(pos, list, GenType::ALL);

    const Color us = pos.side_to_move;
    uint64_t total = 0;
    int legal_moves = 0;

    for (int i = 0; i < list.size(); ++i) {
        const Move move = list[i];
        const Position next = make_move(pos, move);

        // Check for move legality
        if (is_king_attacked(next, us)) {
            continue;
        }

        const uint64_t nodes = (depth == 1) ? 1ULL : perft(next, depth - 1);
        std::println("{}: {}", move.to_string(), nodes);
        total += nodes;
        ++legal_moves;
    }

    std::println("\nMoves: {}", legal_moves);
    std::println("Nodes: {}", total);

    return total;
}