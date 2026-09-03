#include "game/game_save_state.h"

#include <algorithm>
#include <cstddef>

#include "esp_log.h"
#include "nvs.h"

namespace {
constexpr const char *kTag = "game_save_state";
constexpr const char *kNvsNamespace = "sticky2048";
constexpr const char *kStateKey = "game_state";
constexpr uint32_t kStateMagic = 0x32303438U;
constexpr uint16_t kStateVersion = 1;

struct SavedGameState {
    uint32_t magic = kStateMagic;
    uint16_t version = kStateVersion;
    uint16_t reserved = 0;
    Game2048::Board board = {};
    uint32_t score = 0;
    uint32_t best_score = 0;
    uint32_t checksum = 0;
};

uint32_t calculate_checksum(const SavedGameState &state)
{
    const auto *bytes = reinterpret_cast<const uint8_t *>(&state);
    uint32_t hash = 2166136261U;
    for (std::size_t index = 0; index < offsetof(SavedGameState, checksum); ++index) {
        hash = (hash ^ bytes[index]) * 16777619U;
    }
    return hash;
}

bool valid_board(const Game2048::Board &board)
{
    bool has_tile = false;
    for (uint16_t value : board) {
        if (value == 0) {
            continue;
        }
        has_tile = true;
        if (value < 2 || (value & (value - 1U)) != 0) {
            return false;
        }
    }
    return has_tile;
}

bool valid_state(const SavedGameState &state)
{
    return state.magic == kStateMagic &&
           state.version == kStateVersion &&
           state.checksum == calculate_checksum(state) &&
           valid_board(state.board);
}
} // namespace

bool game_save_state(const Game2048 &game, uint32_t best_score)
{
    SavedGameState state = {};
    state.board = game.board();
    state.score = game.score();
    state.best_score = best_score;
    state.checksum = calculate_checksum(state);

    nvs_handle_t handle = 0;
    esp_err_t err = nvs_open(kNvsNamespace, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        err = nvs_set_blob(handle, kStateKey, &state, sizeof(state));
    }
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    if (handle != 0) {
        nvs_close(handle);
    }
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "Game save failed: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(kTag, "Game saved: score=%lu", static_cast<unsigned long>(state.score));
    return true;
}

bool game_load_state(Game2048 &game, uint32_t random_seed, uint32_t &best_score)
{
    nvs_handle_t handle = 0;
    esp_err_t err = nvs_open(kNvsNamespace, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return false;
    }
    if (err != ESP_OK) {
        ESP_LOGW(kTag, "Game load open failed: %s", esp_err_to_name(err));
        return false;
    }

    SavedGameState state = {};
    std::size_t state_size = sizeof(state);
    err = nvs_get_blob(handle, kStateKey, &state, &state_size);
    nvs_close(handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return false;
    }
    if (err != ESP_OK || state_size != sizeof(state) || !valid_state(state)) {
        ESP_LOGW(kTag, "Saved game is missing or invalid");
        return false;
    }

    game.restore(state.board, state.score, random_seed);
    best_score = std::max(best_score, std::max(state.best_score, state.score));
    ESP_LOGI(kTag, "Game restored: score=%lu", static_cast<unsigned long>(state.score));
    return true;
}
