#include "perft.hpp"
#include "makemove.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "position.hpp"
#include "types.hpp"
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

    std::println("Nodes: {}", nodes);

    return nodes;
}