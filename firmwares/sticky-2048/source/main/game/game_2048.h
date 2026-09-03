#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

enum class MoveDirection {
    Up,
    Down,
    Left,
    Right,
};

struct MoveResult {
    bool changed = false;
    uint32_t score_delta = 0;
    bool game_over = false;
};

struct Game2048TestAccess;

class Game2048 {
public:
    static constexpr std::size_t kSize = 4;
    using Board = std::array<uint16_t, kSize * kSize>;

    void reset();
    void reset(uint32_t seed);
    void restore(const Board &board, uint32_t score, uint32_t seed);
    MoveResult move(MoveDirection direction);

    uint16_t tile(std::size_t row, std::size_t column) const;
    const Board &board() const;
    uint32_t score() const;
    bool is_game_over() const;

private:
    friend struct Game2048TestAccess;

    MoveResult move_internal(MoveDirection direction, bool spawn_tile);
    bool add_random_tile();
    uint32_t next_random();

    Board board_ = {};
    uint32_t score_ = 0;
    uint32_t random_state_ = 0x2048U;
};
