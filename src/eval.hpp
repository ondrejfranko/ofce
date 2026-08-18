#pragma once

#include "position.hpp"
#include "types.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

// Score pair used in evaluation terms
struct ScorePair {
    int16_t mg;
    int16_t eg;
};

constexpr ScorePair S(int mg, int eg) {
    return ScorePair{static_cast<int16_t>(mg), static_cast<int16_t>(eg)};
}

constexpr ScorePair operator+(ScorePair a, ScorePair b) {
    return S(a.mg + b.mg, a.eg + b.eg);
}
constexpr ScorePair operator-(ScorePair a, ScorePair b) {
    return S(a.mg - b.mg, a.eg - b.eg);
}
constexpr ScorePair operator-(ScorePair a) {
    return S(-a.mg, -a.eg);
}
constexpr ScorePair operator*(ScorePair a, int n) {
    return S(a.mg * n, a.eg * n);
}
constexpr ScorePair &operator+=(ScorePair &a, ScorePair b) {
    return a = a + b;
}
constexpr ScorePair &operator-=(ScorePair &a, ScorePair b) {
    return a = a - b;
}

// Phase (non-pawn) material weights
inline constexpr std::array<int, PIECE_TYPE_COUNT> PHASE_WEIGHT = {0, 0, 1, 1, 2, 4, 0};
inline constexpr int PHASE_MAX = 24;

// Interpolate between MG and EG scores based on the game phase
constexpr int interpolate(ScorePair s, int phase) {
    return (s.mg * phase + s.eg * (PHASE_MAX - phase)) / PHASE_MAX;
}

// Piece-square tables (including base piece values)
extern ScorePair PSQT[PIECE_COUNT][SQUARE_COUNT];

// Pawn configuration cache entry
struct PawnEntry {
    Key key;         // key (from zobrist pawn hashes)
    ScorePair score; // pawn structure score (white's perspective)
    // TODO: add pawn structure features
};

// Pawn hash table
struct PawnTable {
    static constexpr std::size_t SIZE = 1 << 15; // number of entries (32768)
    std::array<PawnEntry, SIZE> entries{};       // entries

    // Get entry for pawn configuration of the position
    const PawnEntry *probe(const Position &pos);

    // Clear the table
    void clear();
};

// Global pawn hash table
extern PawnTable PAWN_TABLE;

// Startup initialisation of evaluation
void init_eval();

// Static evaluation from POV of the side to move
int evaluate(const Position &pos);