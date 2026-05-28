#include "move.hpp"
#include "types.hpp"
#include <cassert>
#include <string>

std::string Move::to_string() const {
    std::string str;

    str += static_cast<char>('a' + (from() % 8));
    str += static_cast<char>('1' + (from() / 8));
    str += static_cast<char>('a' + (to() % 8));
    str += static_cast<char>('1' + (to() / 8));

    if (is_promo()) {
        switch (promo_piece_type()) {
            case KNIGHT: str += 'n'; break;
            case BISHOP: str += 'b'; break;
            case ROOK: str += 'r'; break;
            case QUEEN: str += 'q'; break;
            default: assert(false); break;
        }
    }

    return str;
}