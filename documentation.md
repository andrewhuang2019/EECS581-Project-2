## Person Hours: Estimate vs. Actual
How it is measured: estimate how long a task would take in person hours based on estimated workload and then whoever had said task would record how many actual hours the task took.

### John Rader: 
- Estimation: 4 hours

Oct 11th, 2026

- 4 hours

### Peter Barybin: 
- Estimation: 3 hours

Sept 28th, 2026: 
- 30 minutes

Oct 5th, 2026
- 2 hours

Oct 11th, 2026

- 1 hour
### Cooper Wright:
- Estimation: 3 hours

Sept 28th, 2026: 
- 30 minutes

Oct 11th, 2026
- 3 hours


### Andrew Huang:
- Estimation: 4 hours

Oct 11th, 2026

- 5 hours

### Emily Farley:
- Estimation: 3 hours

Sept 28th, 2026: 
- 30 minutes

Oct 5th, 2026
- 2 hours

Oct 11th, 2026

- 1 hour

### Evan Noeth:
- Estimation: 3 hours

Oct 11th, 2026

- 4 hours

### Sina Asheghalishahi:
- Estimation: 3 hours

Oct 11th, 2026

- 3 hours


## System Architecture Extensions

### Purpose
Extends the original C++ Minesweeper game with AI-assisted gameplay and sound effects. Players are able to select an AI difficulty and either alternate turns with the AI (on the same board) or have the AI fully solve a game.

### Components
1. AI Solver - Implemented within GameWindow. Easy reveals random covered, unflagged tiles. Medium applies neighboring-cell rules before falling back to Easy. Hard applies all the rules from Medium plus the 1-2-1 pattern rule (and also falls back to Easy).
2. Mode and Difficulty Selector - Dropdown in StartWindow that provides Easy, Medium, and Hard modes. Selector for choosing between Traditional mode and an AI mode (either interactive or fully AI).
3. Audio Manager - Uses miniaudio and files found in the assets folder to provide sounds for starting, revealing tiles, flagging, hitting mines, and win/loss.
4. Existing Game Components - AI actions reuse the existing Board, Tile, reveal_tile(), and flag_tile() components.

### Data Flow
1. User selects mine count and AI difficulty, then starts game.
2. User click passes through InputHandler to the appropriate reveal or flag function.
3. The game updates the board, affected buttons, and game status.
4. After a valid player action, ai_turn() calls the selected AI difficulty. Traditional mode performs no AI action.
5. The AI evaluates the board and executes its moves through the existing reveal and flag functions.
6. In Medium mode, all actions from one matching rule form one AI turn. If no rule applies, the AI makes an Easy random move.
7. Control returns to the player unless the game has ended.

### Key Data Structures
1. AIDifficulty enum — Represents Traditional, Easy, Medium, and Hard.
2. m_ai_difficulty — Stores selected difficulty inside GameWindow.
3. std::vector<std::pair<int, int>> — Stores temporary coordinate lists for hidden neighbors and possible random moves.
4. Hard Pattern Arrays — Store temporary 3×3 board region and templates for identifying the 1-2-1 pattern.
5. Audio Resources — A miniaudio engine and sound objects managed by AudioManager.
6. Existing Board and Tile-Change Structures — Continue storing the shared board and report changes caused by player or AI actions.

### Assumptions
1. The original fixed 10×10 grid, 10–20 mine selection, and first-click protection remain unchanged.
2. The user and AI operate on the same board.
3. A valid player reveal, flag, or unflag triggers one AI turn when an AI difficulty is selected.
4. Medium uses revealed numbers and treats existing flags as correct.
5. One Medium rule can affect multiple neighboring tiles within a single turn.
6. Random fallback moves may reveal mines; the AI is not guaranteed to win.
