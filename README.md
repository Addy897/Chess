# Chess Engine

A modern chess engine built with [Raylib](https://www.raylib.com/) for graphics and sound, featuring advanced AI algorithms like Alpha-Beta Pruning and Iterative Deepening. This engine supports standard chess rules, including castling, en passant, pawn promotion, and checkmate detection.

![Chess Engine Screenshot](https://github.com/user-attachments/assets/33b68e1d-230c-46cd-8952-b07422fbab87)

---

## Features

- **Chess Rules**: Fully implements standard chess rules, including:
  - Castling
  - En passant
  - Pawn promotion
  - Checkmate and stalemate detection(Draw by three fold repetition is not implemented)
- **AI Opponent**:
  - Minimax with Alpha-Beta Pruning for efficient move evaluation
  - Move Odering
  - Iterative Deepening(Not a complete implementation)
- **Graphics**:
  - 2D chessboard with piece sprites
  - Highlighting legal moves
  - Visual feedback for captures, checks, and promotions
- **Sound Effects**:
  - Move sounds
  - Capture sounds
  - Check sounds
- **Customizable**:
  - Load positions from FEN strings
  - Adjustable AI search depth and time limits

---

## Requirements

- **Compiler**: GCC or compatible C compiler
- **Libraries**:
  - [Raylib](https://www.raylib.com/) (for graphics and sound)

---

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/Addy897/Chess.git
   cd Chess
   ```

2. Build the project:
    ```bash
    make
    ```

3. Run the executable:
    ```bash
    ./main
    ```

4. Usage

    Play Against AI: Launch the game, and you'll automatically play as White against the AI.

    Custom Positions: Use FEN strings to set up custom positions.
    ```bash 
    ./main -f <YOUR FEN STRING>
    ```
    
    Move Search for Give depth: Search no. of possible positions for given no. of half moves.
   ```bash
   ./main -d <NUMBER OF HALF MOVES>
   ```

