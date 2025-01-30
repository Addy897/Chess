
#ifndef _INC_MAIN
#define _INC_MAIN
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "preComputeMoves.h"
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
    bool isCapture;
    bool isCheck;
    char capturePieceType;
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
    Move moves[64];
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
int pawnEval[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {50, 50, 50, 50, 50, 50, 50, 50},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {5, 5, 10, 25, 25, 10, 5, 5},
    {0, 0, 0, 20, 20, 0, 0, 0},
    {5, -5, -10, 0, 0, -10, -5, 5},
    {5, 10, 10, -20, -20, 10, 10, 5},
    {0, 0, 0, 0, 0, 0, 0, 0}
};
int knightEval[8][8] = {
{-50,-40,-30,-30,-30,-30,-40,-50,},
{-40,-20,  0,  0,  0,  0,-20,-40,},
{-30,  0, 10, 15, 15, 10,  0,-30,},
{-30,  5, 15, 20, 20, 15,  5,-30,},
{-30,  0, 15, 20, 20, 15,  0,-30,},
{-30,  5, 10, 15, 15, 10,  5,-30,},
{-40,-20,  0,  5,  5,  0,-20,-40,},
{-50,-40,-30,-30,-30,-30,-40,-50,},
};
int bishopEval[8][8] = {
-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20,
};
int rookEval[8][8] = {
 0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0
};
int queenEval[8][8] = {
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20
};

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
void SortMovesByPriority(Piece *piece);

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

int IsCapture(Vector2 square, bool isWhite);
int CalulatePossiblePosition(int depth,bool isWhiteTurn);
int HasDoubledPawns(bool isWhite);
int AlphaBeta(int depth, int alpha, int beta, bool isMaximizingPlayer);
int Eval();   
int IterativeDeepening(int maxDepth, int alpha, int beta, bool isMaximizingPlayer, double timeLimitInSeconds);
int GetMovePriority(Move *move);
Vector2 FindKingPosition(bool isWhite);


void UndoMove(MoveState state);
void ApplyPromotion(MoveState *state);


MoveState ApplyMove(int pieceIndex, Vector2 newPos);
#endif