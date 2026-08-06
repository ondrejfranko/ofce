#include "makemove.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>

// the castling rights that remain after the square has been the origin or destination of a move
constexpr std::array<int8_t, SQUARE_COUNT> build_castling_rights_mask() {
    std::array<int8_t, SQUARE_COUNT> mask{};
    mask.fill(static_cast<int8_t>(CR_ALL));
    mask[E1] = static_cast<int8_t>(CR_ALL & ~CR_WHITE);
    mask[H1] = static_cast<int8_t>(CR_ALL & ~CR_WHITE_KINGSIDE);
    mask[A1] = static_cast<int8_t>(CR_ALL & ~CR_WHITE_QUEENSIDE);
    mask[E8] = static_cast<int8_t>(CR_ALL & ~CR_BLACK);
    mask[H8] = static_cast<int8_t>(CR_ALL & ~CR_BLACK_KINGSIDE);
    mask[A8] = static_cast<int8_t>(CR_ALL & ~CR_BLACK_QUEENSIDE);
    return mask;
}