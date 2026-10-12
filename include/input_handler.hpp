#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <functional>
#include <utility>
#include <vector>

// The two actions supported by a board tile.
enum class ClickType {
    LEFT,
    RIGHT
};

// Game mode picked on the start screen. Traditional is player-only (no AI).
enum class AIDifficulty {
    Traditional,
    Easy,
    Medium,
    Hard,
    // AI plays the whole game by itself
    EasyAuto,
    MediumAuto,
    HardAuto
};

// Describes one tile change returned after an input event.
enum class TileAction {
    Reveal,
    Flag,
    Unflag,
    RevealCascade,
    MineHit
};

struct TileChange {
    int row = 0;
    int col = 0;
    TileAction action;
};

class InputHandler {
    public:
        // The board size is used to convert a 1-based button ID into a row
        // and column coordinate.
        explicit InputHandler(int board_size);

        std::pair<int, int> get_tile(int button_id) const;

        // handle a tile click and return all changes produced by the move
        std::vector<TileChange> handle_tile_click(int button_id, ClickType type);

        // Game logic callbacks. Tile callbacks return the changes
        // caused by the move, lifecycle callbacks perform window actions
        std::function<std::vector<TileChange>(int row, int col)> on_reveal_tile;
        std::function<std::vector<TileChange>(int row, int col)> on_flag_tile;
        std::function<void(int num_bombs, AIDifficulty difficulty)> on_start_game;
        std::function<void()> on_retry;
        std::function<void()> on_close;

    private:
        int m_board_size;
};

#endif // INPUT_HANDLER_H