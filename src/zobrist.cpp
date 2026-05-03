#include "zobrist.hpp"
#include "types.hpp"
#include <random>

Key ZOBRIST_PSQ[PIECE_COUNT][SQUARE_COUNT];
Key ZOBRIST_EP[FILE_COUNT];
Key ZOBRIST_CASTLING[16];
Key ZOBRIST_SIDE_TO_MOVE;

void init_zobrist() {
    std::mt19937_64 prng(69420);
    std::uniform_int_distribution<Key> uniform_dist;

    for (int piece = 0; piece < PIECE_COUNT; ++piece) {
        for (int square = 0; square < SQUARE_COUNT; ++square) {
            ZOBRIST_PSQ[piece][square] = uniform_dist(prng);
        }
    }
    for (int file = 0; file < FILE_COUNT; ++file) {
        ZOBRIST_EP[file] = uniform_dist(prng);
    }
    for (int i = 0; i < 16; ++i) {
        ZOBRIST_CASTLING[i] = uniform_dist(prng);
    }
    ZOBRIST_SIDE_TO_MOVE = uniform_dist(prng);
}