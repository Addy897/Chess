#include <raylib.h>
#ifndef TYPES_H
#define TYPES_H
typedef unsigned char uc;
typedef struct{
    uc to[28];
    uc n;
}Squares;
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
typedef enum{
    FLAG_NONE,
    FLAG_CAPTURE,
    FLAG_CASTLING,
}FLAGS;


#endif