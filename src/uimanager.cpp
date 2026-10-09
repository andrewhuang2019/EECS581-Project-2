#include "uimanager.hpp"
#include <iostream>

using namespace std;

constexpr int TILE_COUNT = GRID_SIZE * GRID_SIZE;
constexpr int TILE_SIZE_PX = 42;
constexpr int GRID_SPACING = 3;

UIWindow::UIWindow(){
  /* UIWindow manages the GTK UI */
  set_title("Minesweeper");
  set_resizable(false);
  //create css provider for GTK
  auto css_provider = Gtk::CssProvider::create();
  //set CSS theming for buttons
  css_provider->load_from_data(
        "window grid button.normal { background: #695f65; color: #ffffff; }"
        "window grid button.normal:hover, window grid button.normal:active { background: #695f65; color: #ffffff; }"
        "window grid button.clicked { background: #a0a09b; color: #2f2a2d; }"
        "window grid button.clicked:hover, window grid button.clicked:active, window grid button.clicked:disabled { background: #a0a09b; color: #2f2a2d; opacity: 1; }"
    ); 
  
  //override user theming for GTK, ensuring our CSS takes priority
  auto default_display = Gdk::Display::get_default();
  if (default_display) {
      Gtk::StyleProvider::add_provider_for_display( 
          default_display, 
          css_provider, 
          GTK_STYLE_PROVIDER_PRIORITY_USER 
      ); 
  }

  m_main_box.set_margin(10);
  m_main_box.set_spacing(8);
  set_child(m_main_box);
  //add game title to top-center
  m_label.set_halign(Gtk::Align::CENTER);
  m_main_box.append(m_label);
}
UIWindow::~UIWindow(){}

StartWindow::StartWindow() {
  //Set scale
  m_bomb_scale.set_range(10, 20);
  m_bomb_scale.set_digits(0);
  m_bomb_scale.set_increments(1, 5);
  m_bomb_scale.set_value(20);
  m_bomb_scale.set_draw_value(true); // Show the numeric value next to the slider
  m_bomb_scale.set_value_pos(Gtk::PositionType::TOP); // Place numeric value above slider
  m_main_box.append(m_bomb_scale);

  //Set button
  m_start_button.set_label("Start");
  m_start_button.signal_clicked().connect(sigc::mem_fun(*this, &StartWindow::on_start_button_clicked));
  m_main_box.append(m_start_button);
}

StartWindow::~StartWindow(){}

void StartWindow::on_start_button_clicked() {
  if (m_input_handler.on_start_game) {
    m_input_handler.on_start_game(static_cast<int>(m_bomb_scale.get_value()));
  }
}

GameWindow::GameWindow() {
  m_main_box.append(m_game_info);
  m_input_handler.on_start_game = [this](int num_bombs) { start_game(num_bombs); };
  m_input_handler.on_retry = [this]() { reset_game(); };
  m_input_handler.on_reveal_tile = [this](int row, int col) { return reveal_tile(row, col); };
  m_input_handler.on_flag_tile = [this](int row, int col) { return flag_tile(row, col); };

  //add button grid to bottom
  m_button_grid.set_row_spacing(GRID_SPACING);
  m_button_grid.set_column_spacing(GRID_SPACING);
  m_button_grid.set_halign(Gtk::Align::CENTER);   // center the grid
  m_button_grid.set_valign(Gtk::Align::CENTER);

  //attach buttons to grid
  for (int i = 0; i < TILE_COUNT; ++i){
    //set up css and size
    m_buttons[i].add_css_class("normal");
    m_buttons[i].set_size_request(TILE_SIZE_PX, TILE_SIZE_PX);
    m_buttons[i].set_label("");
    //add signals for left click
    m_buttons[i].signal_clicked().connect(
      sigc::bind(sigc::mem_fun(*this, &GameWindow::on_button_clicked), i + 1)
    );
    //add signals for right click
    auto right_click = Gtk::GestureClick::create();
    right_click->set_button(GDK_BUTTON_SECONDARY);
    right_click->signal_pressed().connect([this, id = i + 1](int, double, double) {
      on_button_right_clicked(id);
    });
    m_buttons[i].add_controller(right_click);
    //attach button to grid at correct point
    int col = i % GRID_SIZE;
    int row = i / GRID_SIZE;
    m_button_grid.attach(m_buttons[i], col, row, 1, 1);
  }

  for (int i = 0; i < GRID_SIZE; ++i){
		auto row_txt = Glib::ustring::compose("%1", i+1);
		auto col_txt = Glib::ustring::compose("%1", 
			static_cast<char>('A'+i));
		Gtk::Label row(row_txt);
		Gtk::Label col(col_txt);
    m_button_grid.attach(row, GRID_SIZE, i, 1, 1);
    m_button_grid.attach(col, i, GRID_SIZE, 1, 1);
  }

  m_end_box.set_halign(Gtk::Align::CENTER);
  m_end_box.set_valign(Gtk::Align::CENTER);
  m_end_box.append(m_end_label);
  m_play_again_button.signal_clicked().connect(sigc::mem_fun(*this, &GameWindow::on_play_again_clicked));
  m_end_box.append(m_play_again_button);
  m_quit_button.signal_clicked().connect(sigc::mem_fun(*this, &GameWindow::on_quit_clicked));
  m_end_box.append(m_quit_button);
  m_end_box.set_visible(false);

  m_overlay.set_child(m_button_grid);
  m_overlay.add_overlay(m_end_box);

  //add the grid to the bottom of the main box
  m_main_box.append(m_overlay);
  reset_game();
}

GameWindow::~GameWindow(){}

void GameWindow::start_game(int bomb_count) {
  m_bomb_count = bomb_count;
  m_game_state = GameState::NotStarted;
  m_end_box.set_visible(false);
  for (int i = 0; i < TILE_COUNT; ++i) {
    m_buttons[i].remove_css_class("clicked");
    m_buttons[i].add_css_class("normal");
    m_buttons[i].set_label("");
    m_buttons[i].set_sensitive(true);
  }

  m_board = Board{};
  auto txt = Glib::ustring::compose("Flags Placed: %1 | (Bombs/Flags) left: %2", m_flag_count, m_bomb_count - m_flag_count);
	m_game_info.set_text(txt);
}

void GameWindow::reset_game() {
  start_game(m_bomb_count);
}

void GameWindow::update_button_display(int row, int col) {
  const int idx = row * GRID_SIZE + col;
  auto& button = m_buttons[idx];
  auto state = m_board.get_state(col, row);

  if (state == Tile::TileState::Flagged) {
    button.set_label("🚩");
    button.set_sensitive(true);
    return;
  }

  if (state != Tile::TileState::Uncovered) {
    button.set_label("");
    button.set_sensitive(true);
    return;
  }

  button.remove_css_class("normal");
  button.add_css_class("clicked");
  button.set_sensitive(false);

  if (m_board.is_bomb(col, row)) {
    m_audio.play_bomb();
    button.set_label("💣");
    return;
  }

  const auto value = m_board.get_tile_value(col, row);
  if (value == 0) {
    button.set_label("");
    return;
  }

  m_audio.play_click();
  button.set_label(std::to_string(value));
}

void GameWindow::update_all_displays() {
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int col = 0; col < GRID_SIZE; ++col) {
      update_button_display(row, col);
    }
  }
}

std::vector<TileChange> GameWindow::reveal_tile(int row, int col) {
  if (m_game_state == GameState::Won || m_game_state == GameState::Lost) return {};
  if (m_game_state == GameState::NotStarted) {
    m_board.initialize(static_cast<std::uint8_t>(m_bomb_count), static_cast<std::uint8_t>(col), static_cast<std::uint8_t>(row));
    m_game_state = GameState::Playing;
  }

  if (m_board.get_state(col, row) == Tile::TileState::Flagged) {
    return {};
  }

  std::vector<Tile::TileState> previous_states;
  previous_states.reserve(TILE_COUNT);
  for (int r = 0; r < GRID_SIZE; ++r) {
    for (int c = 0; c < GRID_SIZE; ++c) {
      previous_states.push_back(m_board.get_state(c, r));
    }
  }

  const bool hit_bomb = m_board.uncover(col, row);
  std::vector<TileChange> changes;
  for (int r = 0; r < GRID_SIZE; ++r) {
    for (int c = 0; c < GRID_SIZE; ++c) {
      const int index = r * GRID_SIZE + c;
      if (previous_states[index] != Tile::TileState::Uncovered &&
          m_board.get_state(c, r) == Tile::TileState::Uncovered) {
        const auto action = m_board.is_bomb(c, r)
          ? TileAction::MineHit
          : (r == row && c == col ? TileAction::Reveal : TileAction::RevealCascade);
        changes.push_back({r, c, action});
      }
    }
  }

  if (hit_bomb) {
    m_game_state = GameState::Lost;
  } else {
    bool all_safe_tiles_uncovered = true;
    for (int r = 0; r < GRID_SIZE && all_safe_tiles_uncovered; ++r) {
      for (int c = 0; c < GRID_SIZE; ++c) {
        if (!m_board.is_bomb(c, r) &&
            m_board.get_state(c, r) != Tile::TileState::Uncovered) {
          all_safe_tiles_uncovered = false;
          break;
        }
      }
    }

    if (all_safe_tiles_uncovered) {
      m_game_state = GameState::Won;
    }
  }

  return changes;
}

std::vector<TileChange> GameWindow::flag_tile(int row, int col) {
  if (m_game_state != GameState::Playing) {
    return {};
  }

  const auto state = m_board.get_state(col, row);
  if (state == Tile::TileState::Uncovered) {
    return {};
  }

  m_board.toggle_flag(col, row);
  const auto action = state == Tile::TileState::Flagged
    ? TileAction::Unflag
    : TileAction::Flag;


	if (action == TileAction::Flag) {
		if ( m_flag_count >= m_bomb_count ) {
			//undo toggle;
  		m_board.toggle_flag(col, row);
			return {};
		}
		m_flag_count++;
	} else {
		m_flag_count--;
	}
	auto txt = Glib::ustring::compose("Flags Placed: %1 | (Bombs/Flags) left: %2", m_flag_count, m_bomb_count - m_flag_count);
	m_game_info.set_text(txt);

  return {{row, col, action}};
}

void GameWindow::on_button_clicked(int id){
  /* Handler for left button clicks */
  // auto changes = m_input_handler.handle_tile_click(id, ClickType::LEFT);
  // for (const auto& change : changes) {
  //   if (change.action == TileAction::Reveal ||
  //       change.action == TileAction::RevealCascade ||
  //       change.action == TileAction::MineHit) {
  //     update_button_display(change.row, change.col);
  //   }
  // }

  // if (m_game_state == GameState::Won) {
  //   show_end_screen(true);
  // } else if (m_game_state == GameState::Lost) {
  //   show_end_screen(false);
  // }
  ai_easy_turn();
}

void GameWindow::on_button_right_clicked(int id){
  /*Handler for right clicks on button*/
  auto changes = m_input_handler.handle_tile_click(id, ClickType::RIGHT);
  for (const auto& change : changes) {
    if (change.action == TileAction::Flag || change.action == TileAction::Unflag) {
      update_button_display(change.row, change.col);
    }
  }
}

void GameWindow::show_end_screen(bool won){
  m_end_label.set_label(won ? "Winner!" : "Defeat");
  m_end_box.set_visible(true);
  for (int row = 0; row < GRID_SIZE; ++row) {
    for (int col = 0; col < GRID_SIZE; ++col) {
	  	const int idx = row * GRID_SIZE + col;
  		auto& button = m_buttons[idx];
  		button.set_sensitive(false);
  		auto state = m_board.get_state(col, row);
      if (m_board.is_bomb(col, row)) {
				if (state != Tile::TileState::Uncovered) {
					button.remove_css_class("normal");
  				button.add_css_class("clicked");
				}
    		button.set_label("💣");
  		} else if (state == Tile::TileState::Flagged) {
    		button.set_label("");
			}
    }
  }
	m_flag_count = 0;
}

void GameWindow::on_play_again_clicked(){
  m_end_box.set_visible(false);
  if (m_input_handler.on_retry) {
    m_input_handler.on_retry();
  }
}

void GameWindow::on_quit_clicked(){
  if (m_input_handler.on_close) {
    m_input_handler.on_close();
  }
}

bool GameWindow::try_medium_rule(){
  if (m_game_state != GameState::Playing){
    return false;
  }

  for (int row=0; row < GRID_SIZE; row++){
    for (int col=0; col < GRID_SIZE; col++){
      if (m_board.get_state(col, row) != Tile::Uncovered){
        continue;
      }

      const int number = m_board.get_tile_value(col, row);
      if (number == 0 || number == 9) {
        continue;
      }

      int flagged_count = 0;
      std::vector<std::pair<int, int>> hidden_neighbors;

      for (int i=-1; i <= 1; i++){
        for (int j=-1; j <= 1; j++){
          if (i == 0 && j == 0){
            continue;
          }

          const int neighbor_row = row + i;
          const int neighbor_col = col + j;

          if (neighbor_row < 0 || neighbor_row >= GRID_SIZE || neighbor_col < 0 || neighbor_col >= GRID_SIZE){
            continue;
          }

          const auto state = m_board.get_state(neighbor_col, neighbor_row);

          if (state == Tile::Flagged){
            flagged_count++;
          } else if (state == Tile::Covered){
            hidden_neighbors.push_back({neighbor_row, neighbor_col});
          }
        }
      }

      if (hidden_neighbors.empty()){
        continue;
      }

      const int hidden_count = static_cast<int>(hidden_neighbors.size());

      //rule 1 - number of hidden neighbors of a revealed cell equals that cell’s number (all hidden neighbors are mines)
      const bool should_flag = hidden_count == (number - flagged_count);

      //rule 2 - number of flagged neighbors of a revealed cell equals that cell’s number (all hidden neighbors are safe)
      const bool should_reveal = flagged_count == number;

      if (!should_flag && !should_reveal){
        continue;
      }

      bool acted = false;

      for (const auto& neighbor : hidden_neighbors){
        const int neighbor_row = neighbor.first;
        const int neighbor_col = neighbor.second;

        if (m_board.get_state(neighbor_col, neighbor_row) != Tile::Covered){
          continue;
        }

        //flag or reveal according to rule
        const auto changes = should_flag ? flag_tile(neighbor_row, neighbor_col) : reveal_tile(neighbor_row, neighbor_col);

        if (!changes.empty()){
          acted = true;
        }

        for (const auto& change : changes){
          update_button_display(change.row, change.col);
        }

        if (m_game_state == GameState::Won || m_game_state == GameState::Lost){
          show_end_screen(m_game_state == GameState::Won);
          return acted;
        }
      }

      //if medium AI rule was found and followed, end function and report the finding
      if (acted){
        return true;
      }
    }
  }
  return false; //returns if neither rule is able to be applied
}

void GameWindow::ai_medium_turn(){
  if (m_game_state == GameState::Won || m_game_state == GameState::Lost){
    return;
  }

  //if a medium rule was not performed by the AI
  if (!try_medium_rule()){
    // IMPLEMENT: call the easy AI function
    ai_easy_turn();
  }
}

bool GameWindow::try_easy_rule() {
  if (m_game_state != GameState::Playing) {
    return false;
  }

  bool covered_tile_exists = false;
  for (int r = 0; r < GRID_SIZE; ++r) {
    for (int c = 0; c < GRID_SIZE; ++c) {
      if (m_board.get_state(c, r) == Tile::TileState::Covered) {
        covered_tile_exists = true;
        break;
      }
    }
  }

  if (!covered_tile_exists) {
    return false;
  }

  uint8_t target_x;
  uint8_t target_y;
  bool valid_tile_found = false;

  while (!valid_tile_found) {
    target_x = rand() % GRID_SIZE; 
    target_y = rand() % GRID_SIZE; 
    if (m_board.get_state(target_x, target_y) == Tile::TileState::Covered) {
      bool hit_bomb = m_board.uncover(target_x, target_y); // Fixed capitalization
      valid_tile_found = true;
      if (hit_bomb) {
        m_game_state = GameState::Lost;
      }
    }
  }

  update_all_displays();
  return true;
}

void GameWindow::ai_easy_turn(){
  if (m_game_state == GameState::Won || m_game_state == GameState::Lost){
    return;
  }
  
  if (!try_easy_rule()){
    std::cerr << "FAILED TO TAKE AI EASY TURN\n";  
    return;
  }
}