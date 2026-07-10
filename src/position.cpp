#include "position.hpp"
#include "bitboard.hpp"
#include "types.hpp"
#include "zobrist.hpp"
#include <cassert>
#include <cctype>
#include <charconv>
#include <format>
#include <print>
#include <string_view>

std::string get_fen(const Position &pos) {
    std::string fen;
    fen.reserve(90);

    constexpr std::string_view ascii_pieces = " PNBRQKpnbrqk";

    // 1. Piece placement
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        int empty_count = 0;

        for (int file = FILE_A; file <= FILE_H; ++file) {
            Square sq = static_cast<Square>(rank * 8 + file);
            Piece p = pos.squares[sq];

            if (p == PIECE_NONE) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen += static_cast<char>('0' + empty_count);
                    empty_count = 0;
                }
                fen += ascii_pieces[p];
            }
        }
        if (empty_count > 0) {
            fen += static_cast<char>('0' + empty_count);
        }
        if (rank > RANK_1) {
            fen += '/';
        }
    }

    // 2. Side to move
    fen += (pos.side_to_move == WHITE) ? " w " : " b ";

    // 3. Castling rights
    if (pos.castling_rights == CR_NONE) {
        fen += "-";
    } else {
        if (pos.castling_rights & CR_WHITE_KINGSIDE)
            fen += 'K';
        if (pos.castling_rights & CR_WHITE_QUEENSIDE)
            fen += 'Q';
        if (pos.castling_rights & CR_BLACK_KINGSIDE)
            fen += 'k';
        if (pos.castling_rights & CR_BLACK_QUEENSIDE)
            fen += 'q';
    }
    fen += ' ';

    // 4. En passant square
    if (pos.en_passant_square != SQUARE_NONE) {
        fen += ('a' + (pos.en_passant_square % 8));
        fen += ('1' + (pos.en_passant_square / 8));
    } else {
        fen += "-";
    }
    fen += ' ';

    // 5. and 6. Halfmove clock and Fullmove number
    fen += std::format("{} {}", pos.fifty_move_rule_ply, pos.game_ply / 2 + 1);

    return fen;
}

void clear_square(Position &pos, Square square) {
    assert(A1 <= square && square < SQUARE_COUNT);

    Piece p = pos.squares[square];

    if (p == PIECE_NONE) {
        return;
    }

    // Clear bitboards
    clear_bit(pos.piece_BB[p], square);
    clear_bit(pos.color_BB[piece_color(p)], square);

    // Clear mailbox square
    pos.squares[square] = PIECE_NONE;

    // Update zobrist key
    pos.zobrist_key ^= ZOBRIST_PSQ[p][square];
}

void set_square(Position &pos, Square square, Piece piece) {
    assert(A1 <= square && square < SQUARE_COUNT);
    assert(WHITE_PAWN <= piece && piece < PIECE_COUNT);

    // Clear mailbox square
    if (pos.squares[square] != PIECE_NONE) {
        clear_square(pos, square);
    }

    // Set mailbox square
    pos.squares[square] = piece;

    // Set bitboards
    if (piece != PIECE_NONE) {
        set_bit(pos.piece_BB[piece], square);
        set_bit(pos.color_BB[piece_color(piece)], square);
    }

    // Update zobrist key
    pos.zobrist_key ^= ZOBRIST_PSQ[piece][square];
}

void clear_position(Position &pos) {
    pos = Position{};
}

void set_position(Position &pos, std::string_view fen) {
    clear_position(pos);

    int sq = A8;
    size_t i = 0;

    // 1. Parse piece placement
    for (; i < fen.length() && fen[i] != ' '; ++i) {
        char c = fen[i];
        if (c == '/') {
            sq -= 16;
        } else if (std::isdigit(c)) {
            sq += (c - '0');
        } else {
            assert(A1 <= sq && sq < SQUARE_COUNT);

            set_square(pos, static_cast<Square>(sq++), char_to_piece(c));
        }
    }

    // Skip whitespace
    while (i < fen.length() && fen[i] == ' ') {
        ++i;
    }

    // 2. Parse side to move
    if (i < fen.length()) {
        pos.side_to_move = (fen[i] == 'w') ? WHITE : BLACK;
        ++i;

        // Update side to move zobrist key
        if (pos.side_to_move == BLACK) {
            pos.zobrist_key ^= ZOBRIST_SIDE_TO_MOVE;
        }
    }

    // Skip whitespace
    while (i < fen.length() && fen[i] == ' ') {
        ++i;
    }

    // 3. Parse castling rights
    pos.castling_rights = CR_NONE;
    for (; i < fen.length() && fen[i] != ' '; ++i) {
        char c = fen[i];
        switch (c) {
            case 'K': pos.castling_rights |= CR_WHITE_KINGSIDE; break;
            case 'Q': pos.castling_rights |= CR_WHITE_QUEENSIDE; break;
            case 'k': pos.castling_rights |= CR_BLACK_KINGSIDE; break;
            case 'q': pos.castling_rights |= CR_BLACK_QUEENSIDE; break;
        }
    }

    // Update castling rights zobrist key
    pos.zobrist_key ^= ZOBRIST_CASTLING[pos.castling_rights];

    // Skip whitespace
    while (i < fen.length() && fen[i] == ' ') {
        ++i;
    }

    // 4. Parse en passant square
    if (i + 1 < fen.length() && fen[i] != '-') {
        char file = fen[i];
        char rank = fen[i + 1];
        if (file >= 'a' && file <= 'h' && rank >= '1' && rank <= '8') {
            pos.en_passant_square = static_cast<Square>((rank - '1') * 8 + (file - 'a'));

            // Update en passant zobrist key
            pos.zobrist_key ^= ZOBRIST_EP[file - 'a'];
        }
        i += 2;
    } else {
        pos.en_passant_square = SQUARE_NONE;
        i += 1;
    }

    // Skip whitespace
    while (i < fen.length() && fen[i] == ' ') {
        ++i;
    }

    // 5. Parse halfmove clock
    pos.fifty_move_rule_ply = 0;
    if (i < fen.length()) {
        auto [ptr, _] = std::from_chars(fen.data() + i, fen.data() + fen.length(), pos.fifty_move_rule_ply);
        i = ptr - fen.data();
    }

    // Skip whitespace
    while (i < fen.length() && fen[i] == ' ') {
        ++i;
    }

    // 6. Parse fullmove number
    int fullmove = 1;
    if (i < fen.length()) {
        std::from_chars(fen.data() + i, fen.data() + fen.length(), fullmove);
        pos.game_ply = (fullmove - 1) * 2 + (pos.side_to_move == BLACK ? 1 : 0);
    }
}

void print_position(const Position &pos) {
    constexpr std::string_view ascii_pieces = " PNBRQKpnbrqk";
    std::string fen = get_fen(pos);

    // Print board
    for (int rank = RANK_8; rank >= RANK_1; --rank) {
        std::println("  +---+---+---+---+---+---+---+---+");
        std::print("{} |", rank + 1);
        for (int file = FILE_A; file <= FILE_H; ++file) {
            Square sq = static_cast<Square>(rank * 8 + file);
            std::print(" {} |", ascii_pieces[pos.squares[sq]]);
        }
        std::println();
    }
    std::println("  +---+---+---+---+---+---+---+---+");
    std::println("    a   b   c   d   e   f   g   h\n");

    // Print FEN
    std::println("FEN: {}", fen);

    // Print Zobrist key
    std::println("Zobrist Key: {:#018x}", pos.zobrist_key);

    // Flush stdout
    std::fflush(stdout);
}