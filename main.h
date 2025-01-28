
#ifndef _INC_MAIN
#define _INC_MAIN
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COLS 8
#define ROWS 8
#define SIDE_WIDTH 400

static const int screenHeight = 800;
static const int screenWidth = 800;
static const int col_width = screenWidth / COLS;
static const int col_height = screenHeight / ROWS;
typedef struct
{
    Vector2 startPos;
    Vector2 endPos;
    char pieceType;
    bool pieceisWhite;
} Move;
typedef struct {
    int promoteTo;
        bool castlingPerformed;

    int enPassantCapturedIndex;
    int pieceIndex;
    int capturedIndex;
    int rookIndex;
    Vector2 rookOriginalPos;
    Vector2 originalPos;
    Vector2 capturedOriginalPos;

} MoveState;
typedef struct
{
    char type;
    Vector2 pos;
    bool isDragging;
    bool isWhite;
    bool canDraw;
    bool hasMoved;
    Move moves[27];
    int moveIndex;
} Piece;
typedef struct
{
    bool isWhiteTurn;
    bool playingWithComputer;
    char state;
    Move computerMove;
    int computerPieceIndex;
}GameState;

typedef enum
{
    PROMOTION_NONE,
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_BISHOP,
    PROMOTION_KNIGHT
} PromotionChoice;

static PromotionChoice pendingPromotion = PROMOTION_NONE;
static Move lastMove = {0};
static GameState gameState = {0};
static int pieceCount = 0;
static int lastSelectedPiece = -1;
static int promotingPieceIndex = -1;
static int currentMovesIndex=-1;

Sound moveSound;
Sound captureSound;
Sound checkSound;

void AddMove(int pieceIndex,Vector2 endPos);
void GenerateAllLegalMovesForPiece(int pieceIndex);
void GenerateAllLegalMoves(bool forWhite);
void DrawBoard();
void DrawPieces(Texture2D piecesTexture);
void SetupPiecesFromFEN(const char *fen);
void HighlightLegalMoves(int pieceIndex);
void UpdateLastMove(int pieceIndex, Vector2 newPos);
void HandleEnPassant(int pieceIndex, Vector2 newPos);
void DrawPromotionPopup(Texture2D piecesTexture);
void CheckForInput();
void PlayMove(bool withWhite);
void PlayMoveOnBoard(Vector2 new_pos);
void HighlightPreviousMove();


bool IsValidMoveWithoutCheck(int pieceIndex, Vector2 newPos);
bool IsKingInCheck(bool isWhite);
bool IsValidMove(int pieceIndex, Vector2 newPos);
bool IsSquareUnderAttack(Vector2 square, bool isWhite);
bool IsCheckMate(bool isWhite);
bool IsStaleMate(bool isWhite);
bool IsPromotionSquare(Vector2 pos, bool isWhite);
bool IsVector2Equal(Vector2 a, Vector2 b);
bool IsValidMove(int pieceIndex, Vector2 newPos);
bool IsSquareUnderAttack(Vector2 square, bool isWhite);
bool IsLegalMove(int pieceIndex, Vector2 newPos);
bool IsSquareOccupied(Vector2 pos);
bool IsIsolatedPawn(int pawnIndex);
bool IsBlockedPawn(int pawnIndex);



int CalulatePossiblePosition(int depth,bool isWhiteTurn);
int HasDoubledPawns(bool isWhite);
Vector2 FindKingPosition(bool isWhite);

int Eval();   
int Minimax(int depth, bool isMaximizingPlayer);

void UndoMove(MoveState state);
void ApplyPromotion(MoveState *state);
MoveState ApplyMove(int pieceIndex, Vector2 newPos);
int AlphaBeta(int depth, int alpha, int beta, bool isMaximizingPlayer);
#endif