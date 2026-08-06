#include "game/game_2048.h"

#include <algorithm>

namespace {
using Line = std::array<uint16_t, Game2048::kSize>;

Line read_line(const Game2048::Board &board, MoveDirection direction, std::size_t line)
{
    Line values = {};
    for (std::size_t index = 0; index < Game2048::kSize; ++index) {
        switch (direction) {
        case MoveDirection::Left:
            values[index] = board[line * Game2048::kSize + index];
            break;
        case MoveDirection::Right:
            values[index] = board[line * Game2048::kSize + (Game2048::kSize - 1 - index)];
            break;
        case MoveDirection::Up:
            values[index] = board[index * Game2048::kSize + line];
            break;
        case MoveDirection::Down:
            values[index] = board[(Game2048::kSize - 1 - index) * Game2048::kSize + line];
            break;
        }
    }
    return values;
}

void write_line(Game2048::Board &board,
                MoveDirection direction,
                std::size_t line,
                const Line &values)
{
    for (std::size_t index = 0; index < Game2048::kSize; ++index) {
        switch (direction) {
        case MoveDirection::Left:
            board[line * Game2048::kSize + index] = values[index];
            break;
        case MoveDirection::Right:
            board[line * Game2048::kSize + (Game2048::kSize - 1 - index)] = values[index];
            break;
        case MoveDirection::Up:
            board[index * Game2048::kSize + line] = values[index];
            break;
        case MoveDirection::Down:
            board[(Game2048::kSize - 1 - index) * Game2048::kSize + line] = values[index];
            break;
        }
    }
}

Line collapse_line(const Line &input, uint32_t &score_delta)
{
    Line compact = {};
    std::size_t compact_size = 0;
    for (uint16_t value : input) {
        if (value != 0) {
            compact[compact_size++] = value;
        }
    }

    Line output = {};
    std::size_t output_index = 0;
    for (std::size_t index = 0; index < compact_size;) {
        if (index + 1 < compact_size && compact[index] == compact[index + 1]) {
            const uint16_t merged = static_cast<uint16_t>(compact[index] * 2U);
            output[output_index++] = merged;
            score_delta += merged;
            index += 2;
        } else {
            output[output_index++] = compact[index++];
        }
    }
    return output;
}
} // namespace

void Game2048::reset()
{
    board_.fill(0);
    score_ = 0;
    add_random_tile();
    add_random_tile();
}

void Game2048::reset(uint32_t seed)
{
    random_state_ = seed == 0 ? 0x2048U : seed;
    reset();
}

void Game2048::restore(const Board &board, uint32_t score, uint32_t seed)
{
    board_ = board;
    score_ = score;
    random_state_ = seed == 0 ? 0x2048U : seed;
}

MoveResult Game2048::move(MoveDirection direction)
{
    return move_internal(direction, true);
}

uint16_t Game2048::tile(std::size_t row, std::size_t column) const
{
    if (row >= kSize || column >= kSize) {
        return 0;
    }
    return board_[row * kSize + column];
}

const Game2048::Board &Game2048::board() const
{
    return board_;
}

uint32_t Game2048::score() const
{
    return score_;
}

bool Game2048::is_game_over() const
{
    if (std::find(board_.begin(), board_.end(), 0) != board_.end()) {
        return false;
    }

    for (std::size_t row = 0; row < kSize; ++row) {
        for (std::size_t column = 0; column < kSize; ++column) {
            const uint16_t value = tile(row, column);
            if (column + 1 < kSize && value == tile(row, column + 1)) {
                return false;
            }
            if (row + 1 < kSize && value == tile(row + 1, column)) {
                return false;
            }
        }
    }
    return true;
}

MoveResult Game2048::move_internal(MoveDirection direction, bool spawn_tile)
{
    const Board before = board_;
    uint32_t score_delta = 0;

    for (std::size_t line = 0; line < kSize; ++line) {
        const Line moved = collapse_line(read_line(board_, direction, line), score_delta);
        write_line(board_, direction, line, moved);
    }

    const bool changed = board_ != before;
    if (changed) {
        score_ += score_delta;
        if (spawn_tile) {
            add_random_tile();
        }
    }

    return {
        changed,
        changed ? score_delta : 0,
        is_game_over(),
    };
}

bool Game2048::add_random_tile()
{
    std::array<std::size_t, kSize * kSize> empty_cells = {};
    std::size_t empty_count = 0;
    for (std::size_t index = 0; index < board_.size(); ++index) {
        if (board_[index] == 0) {
            empty_cells[empty_count++] = index;
        }
    }
    if (empty_count == 0) {
        return false;
    }

    const std::size_t selected = empty_cells[next_random() % empty_count];
    board_[selected] = (next_random() % 10U == 0U) ? 4 : 2;
    return true;
}

uint32_t Game2048::next_random()
{
    random_state_ ^= random_state_ << 13U;
    random_state_ ^= random_state_ >> 17U;
    random_state_ ^= random_state_ << 5U;
    return random_state_;
}
