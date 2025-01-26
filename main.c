#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLS 8
#define ROWS 8
#define SIDE_WIDTH 400

const int screenHeight = 800;
const int screenWidth = 800;
const int col_width = screenWidth / COLS;
const int col_height = screenHeight / ROWS;

typedef struct
{
    char type;
    Vector2 pos;
    bool isDragging;
    bool isWhite;
    bool canDraw;
    bool hasMoved;
} Piece;
typedef struct
{
    Vector2 startPos;
    Vector2 endPos;
    char pieceType;
    bool pieceisWhite;
} Move;
typedef enum
{
    PROMOTION_NONE,
    PROMOTION_QUEEN,
    PROMOTION_ROOK,
    PROMOTION_BISHOP,
    PROMOTION_KNIGHT
} PromotionChoice;

PromotionChoice pendingPromotion = PROMOTION_NONE;
Move lastMove = {0};
Piece pieces[ROWS * COLS] = {0};
int pieceCount = 0;
int lastSelectedPiece = -1;
int promotingPieceIndex = -1;

bool IsValidMove(int pieceIndex, Vector2 newPos);
bool IsSquareUnderAttack(Vector2 square, bool isWhite);
void DrawBoard();
void DrawPieces(Texture2D piecesTexture);
void SetupPiecesFromFEN(const char *fen);
void HighlightLegalMoves(int pieceIndex);
bool IsVector2Equal(Vector2 a, Vector2 b);
void UpdateLastMove(int pieceIndex, Vector2 newPos);
void HandleEnPassant(int pieceIndex, Vector2 newPos);
Vector2 FindKingPosition(bool isWhite);
bool IsValidMoveWithoutCheck(int pieceIndex, Vector2 newPos);
bool IsKingInCheck(bool isWhite);
bool IsValidMove(int pieceIndex, Vector2 newPos);
bool IsSquareUnderAttack(Vector2 square, bool isWhite);
bool IsCheckMate(bool isWhite);
bool IsPromotionSquare(Vector2 pos, bool isWhite)
{
    return (isWhite && pos.y == 0) || (!isWhite && pos.y == screenHeight - col_height);
}
void DrawPromotionPopup(Texture2D piecesTexture)
{
    if (promotingPieceIndex == -1)
        return;

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));

    Rectangle popupRect = {
        screenWidth / 2 - 200,
        screenHeight / 2 - 100,
        400,
        200};
    DrawRectangleRec(popupRect, LIGHTGRAY);

    
    DrawText("Promote Pawn To:", popupRect.x + 20, popupRect.y + 10, 20, DARKGRAY);

    
    float pieceSize = 80;
    float spacing = 20;
    float startX = popupRect.x + (popupRect.width - (4 * pieceSize + 3 * spacing)) / 2;
    float y = popupRect.y + 50;

    
    Rectangle queenRect = {startX, y, pieceSize, pieceSize};
    DrawTexturePro(piecesTexture,
                   (Rectangle){1.0f * piecesTexture.width / 6, pieces[promotingPieceIndex].isWhite ? 0 : piecesTexture.height / 2,
                               (float)piecesTexture.width / 6, (float)piecesTexture.height / 2},
                   queenRect, (Vector2){0}, 0, WHITE);

    
    Rectangle rookRect = {startX + pieceSize + spacing, y, pieceSize, pieceSize};
    DrawTexturePro(piecesTexture,
                   (Rectangle){4.0f * piecesTexture.width / 6, pieces[promotingPieceIndex].isWhite ? 0 : piecesTexture.height / 2,
                               (float)piecesTexture.width / 6, (float)piecesTexture.height / 2},
                   rookRect, (Vector2){0}, 0, WHITE);

    
    Rectangle bishopRect = {startX + 2 * (pieceSize + spacing), y, pieceSize, pieceSize};
    DrawTexturePro(piecesTexture,
                   (Rectangle){2.0f * piecesTexture.width / 6, pieces[promotingPieceIndex].isWhite ? 0 : piecesTexture.height / 2,
                               (float)piecesTexture.width / 6, (float)piecesTexture.height / 2},
                   bishopRect, (Vector2){0}, 0, WHITE);

    
    Rectangle knightRect = {startX + 3 * (pieceSize + spacing), y, pieceSize, pieceSize};
    DrawTexturePro(piecesTexture,
                   (Rectangle){3.0f * piecesTexture.width / 6, pieces[promotingPieceIndex].isWhite ? 0 : piecesTexture.height / 2,
                               (float)piecesTexture.width / 6, (float)piecesTexture.height / 2},
                   knightRect, (Vector2){0}, 0, WHITE);
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (CheckCollisionPointRec(GetMousePosition(), queenRect))
        {
            DrawRectangleLinesEx(queenRect, 2, YELLOW);
            pendingPromotion = PROMOTION_QUEEN;
        }
        if (CheckCollisionPointRec(GetMousePosition(), rookRect))
        {
            DrawRectangleLinesEx(rookRect, 2, YELLOW);
                pendingPromotion = PROMOTION_ROOK;
        }
        if (CheckCollisionPointRec(GetMousePosition(), bishopRect))
        {
            DrawRectangleLinesEx(bishopRect, 2, YELLOW);
                pendingPromotion = PROMOTION_BISHOP;
        }
        if (CheckCollisionPointRec(GetMousePosition(), knightRect))
        {
            DrawRectangleLinesEx(knightRect, 2, YELLOW);
                pendingPromotion = PROMOTION_KNIGHT;
        }
    }
    
    if (pendingPromotion != PROMOTION_NONE)
    {
        switch (pendingPromotion)
        {
        case PROMOTION_QUEEN:
            pieces[promotingPieceIndex].type = 'Q';
            break;
        case PROMOTION_ROOK:
            pieces[promotingPieceIndex].type = 'R';
            break;
        case PROMOTION_BISHOP:
            pieces[promotingPieceIndex].type = 'B';
            break;
        case PROMOTION_KNIGHT:
            pieces[promotingPieceIndex].type = 'N';
            break;
        default:
            break;
        }
        promotingPieceIndex = -1;
        pendingPromotion = PROMOTION_NONE;
    }
}
int main(int argc, char const *argv[])
{
    const char *title = "Chess with FEN";
    InitWindow(screenWidth + SIDE_WIDTH, screenHeight, title);
    InitAudioDevice();
    Sound moveSound = LoadSound("./resources/move.mp3");
    Sound captureSound = LoadSound("./resources/capture.mp3");
    Sound checkSound = LoadSound("./resources/check.mp3");
    Texture2D piecesTexture = LoadTexture("./resources/chess-pieces.png");
    SetTextureFilter(piecesTexture, TEXTURE_FILTER_BILINEAR);
    char fen[100];
    if (argc > 1)
    {
        strcpy(fen, argv[1]);
    }
    else
    {
        strcpy(fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
    char *token = strtok(fen, " ");
    SetupPiecesFromFEN(token);

    bool isWhiteTurn = strtok(NULL, " ")[0] == 'w';

    SetTargetFPS(144);

    while (!WindowShouldClose())
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos = GetMousePosition();
            for (int i = 0; i < pieceCount; i++)
            {
                Rectangle pieceRect = {pieces[i].pos.x, pieces[i].pos.y, col_width, col_height};

                if (CheckCollisionPointRec(mousePos, pieceRect) && pieces[i].canDraw)
                {
                    if (pieces[i].isWhite != isWhiteTurn)
                    {
                        break;
                    }
                    pieces[i].isDragging = true;
                    lastSelectedPiece = i;

                    break;
                }
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if (lastSelectedPiece != -1)
            {
                if (pieces[lastSelectedPiece].isDragging)
                {
                    Vector2 mousePos = GetMousePosition();
                    int col = (int)(mousePos.x / col_width);
                    int row = (int)(mousePos.y / col_height);
                    Vector2 new_pos = (Vector2){col * col_width, row * col_height};

                    if (IsValidMove(lastSelectedPiece, new_pos))
                    {
                        bool canMove = true;
                        bool isCapture = false;

                        for (int i = 0; i < pieceCount; i++)
                        {
                            if (!pieces[i].canDraw || i == lastSelectedPiece)
                            {
                                continue;
                            }
                            if (pieces[i].pos.x == new_pos.x && pieces[i].pos.y == new_pos.y)
                            {
                                if (pieces[lastSelectedPiece].isWhite == pieces[i].isWhite || pieces[i].type == 'K')
                                {
                                    canMove = false;
                                    break;
                                }
                                else
                                {
                                    pieces[i].canDraw = false;
                                    isCapture = true;
                                    break;
                                }
                            }
                        }
                        if (canMove)
                        {
                            if (pieces[lastSelectedPiece].type == 'K' && abs((new_pos.x - pieces[lastSelectedPiece].pos.x) / col_width) == 2)
                            {
                                int direction = (new_pos.x > pieces[lastSelectedPiece].pos.x) ? 1 : -1;
                                Vector2 rookNewPos = {new_pos.x - direction * col_width, new_pos.y};

                                for (int i = 0; i < pieceCount; i++)
                                {
                                    if (pieces[i].type == 'R' && pieces[i].pos.y == new_pos.y)
                                    {
                                        if ((direction == 1 && pieces[i].pos.x == 7 * col_width) ||
                                            (direction == -1 && pieces[i].pos.x == 0 * col_width))
                                        {
                                            pieces[i].pos = rookNewPos;
                                            pieces[i].hasMoved = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            HandleEnPassant(lastSelectedPiece, new_pos);

                            UpdateLastMove(lastSelectedPiece, new_pos);
                            if (pieces[lastSelectedPiece].pos.x != new_pos.x || pieces[lastSelectedPiece].pos.y != new_pos.y)
                            {
                                pieces[lastSelectedPiece].hasMoved = true;
                            }
                            pieces[lastSelectedPiece].pos = new_pos;
                            if (IsKingInCheck(!pieces[lastSelectedPiece].isWhite))
                            {
                                PlaySound(checkSound);
                            }
                            else if (isCapture)
                            {
                                PlaySound(captureSound);
                            }
                            else
                            {
                                PlaySound(moveSound);
                            }
                            if (pieces[lastSelectedPiece].type == 'P' && IsPromotionSquare(new_pos, pieces[lastSelectedPiece].isWhite))
                            {
                                promotingPieceIndex = lastSelectedPiece;
                            }

                            isWhiteTurn = !isWhiteTurn;
                        }
                    }
                    pieces[lastSelectedPiece].isDragging = false;
                }
                lastSelectedPiece = -1;
            }
        }

        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(screenWidth, 0, SIDE_WIDTH, screenHeight, BLACK);

        DrawBoard();
        if (promotingPieceIndex != -1)
        {
            DrawPromotionPopup(piecesTexture);
        }
        if (lastSelectedPiece != -1)
        {
            HighlightLegalMoves(lastSelectedPiece);
        }
        if (isWhiteTurn)
        {
            DrawText("White Turn", screenWidth + SIDE_WIDTH / 4, 200, 24, WHITE);
        }
        else
        {
            DrawText("Black Turn", screenWidth + SIDE_WIDTH / 4, 200, 24, WHITE);
        }
        if (IsCheckMate(!lastMove.pieceisWhite))
        {
            char winner[6];
            if (lastMove.pieceisWhite)
                strcpy(winner, "White");
            else
                strcpy(winner, "Black");

            char result[100];
            sprintf(result, "GAME OVER\n%s Won by Checkmate", winner);
            DrawText(result, screenWidth + (SIDE_WIDTH / 8), SIDE_WIDTH, 24, WHITE);
        }
        DrawPieces(piecesTexture);

        EndDrawing();
    }

    UnloadTexture(piecesTexture);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void DrawBoard()
{
    bool black = false;
    Color dark = {184, 139, 74, 255};
    Color light = {227, 193, 111, 125};

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (black)
            {
                DrawRectangle(i * col_width, j * col_height, col_width, col_height, dark);
            }
            else
            {
                DrawRectangle(i * col_width, j * col_height, col_width, col_height, light);
            }
            black = !black;
        }
        black = !black;
    }
}
void DrawPieces(Texture2D piecesTexture)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (!pieces[i].canDraw)
        {
            continue;
        }
        Rectangle srcRec;
        int isBlack = !pieces[i].isWhite;
        switch (pieces[i].type)
        {
        case 'P':
            srcRec = (Rectangle){5.0f * (float)piecesTexture.width / 6, isBlack * (float)piecesTexture.height / 2, (float)piecesTexture.width / 6, (float)piecesTexture.height / 2};
            break;
        case 'R':
            srcRec = (Rectangle){4.0f * (float)piecesTexture.width / 6, isBlack * (float)piecesTexture.height / 2, (float)piecesTexture.width / 6, (float)piecesTexture.height / 2};
            break;
        case 'K':
            srcRec = (Rectangle){0.0f, isBlack * (float)piecesTexture.height / 2, (float)piecesTexture.width / 6, (float)piecesTexture.height / 2};
            break;
        case 'Q':
            srcRec = (Rectangle){1.0f * (float)piecesTexture.width / 6, isBlack * (float)piecesTexture.height / 2, (float)piecesTexture.width / 6, (float)piecesTexture.height / 2};
            break;
        case 'B':
            srcRec = (Rectangle){2.0f * (float)piecesTexture.width / 6, isBlack * (float)piecesTexture.height / 2, (float)piecesTexture.width / 6, (float)piecesTexture.height / 2};
            break;
        case 'N':
            srcRec = (Rectangle){3.0f * (float)piecesTexture.width / 6, isBlack * (float)piecesTexture.height / 2, (float)piecesTexture.width / 6, (float)piecesTexture.height / 2};
            break;
        default:
            continue;
        }

        Vector2 pos = pieces[i].pos;
        if (pieces[i].isDragging)
        {
            pos = GetMousePosition();
            pos.x -= col_width / 2;
            pos.y -= col_height / 2;
        }

        Rectangle dstRec = {pos.x, pos.y, col_width, col_height};
        DrawTexturePro(piecesTexture, srcRec, dstRec, (Vector2){0, 0}, 0, WHITE);
    }
}
void SetupPiecesFromFEN(const char *fen)
{
    int row = 0;
    int col = 0;
    pieceCount = 0;
    for (int i = 0; fen[i] != '\0'; i++)
    {
        char c = fen[i];

        if (c == '/')
        {
            row++;
            col = 0;
        }
        else if (c >= '1' && c <= '8')
        {
            col += c - '0';
        }
        else
        {
            pieces[pieceCount].type = c >= 'a' ? 'A' + (c - 'a') : c;
            pieces[pieceCount].pos = (Vector2){col * col_width, row * col_height};
            pieces[pieceCount].isDragging = false;
            pieces[pieceCount].isWhite = c < 96;
            pieces[pieceCount].canDraw = true;
            pieces[pieceCount].hasMoved = false;
            pieceCount++;
            col++;
        }
    }
}
void HighlightLegalMoves(int pieceIndex)
{
    Piece piece = pieces[pieceIndex];
    int startX = (int)(piece.pos.x / col_width);
    int startY = (int)(piece.pos.y / col_height);

    switch (piece.type)
    {

    case 'P':
        if (piece.isWhite)
        {
            if (IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY - 1) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY - 1) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }

            if (IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY - 2) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY - 2) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }

            if (startX > 0 && IsValidMove(pieceIndex, (Vector2){(startX - 1) * col_width, (startY - 1) * col_height}))
            {
                DrawRectangle((startX - 1) * col_width, (startY - 1) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX < COLS - 1 && IsValidMove(pieceIndex, (Vector2){(startX + 1) * col_width, (startY - 1) * col_height}))
            {
                DrawRectangle((startX + 1) * col_width, (startY - 1) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            break;
        }
        else
        {
            if (IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY + 1) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY + 1) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startY == 1 && IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY + 2) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY + 2) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }

            if (startX > 0 && IsValidMove(pieceIndex, (Vector2){(startX - 1) * col_width, (startY + 1) * col_height}))
            {
                DrawRectangle((startX - 1) * col_width, (startY + 1) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX < COLS - 1 && IsValidMove(pieceIndex, (Vector2){(startX + 1) * col_width, (startY + 1) * col_height}))
            {
                DrawRectangle((startX + 1) * col_width, (startY + 1) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            break;
        }

    case 'R':
        for (int i = 1; i < COLS; i++)
        {
            if (startX + i < COLS && IsValidMove(pieceIndex, (Vector2){(startX + i) * col_width, startY * col_height}))
            {
                DrawRectangle((startX + i) * col_width, startY * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX - i >= 0 && IsValidMove(pieceIndex, (Vector2){(startX - i) * col_width, startY * col_height}))
            {
                DrawRectangle((startX - i) * col_width, startY * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startY + i < ROWS && IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY + i) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY + i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startY - i >= 0 && IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY - i) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY - i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
        }
        break;

    case 'N':
        int knightMoves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
        for (int i = 0; i < 8; i++)
        {
            int x = startX + knightMoves[i][0];
            int y = startY + knightMoves[i][1];
            if (x >= 0 && x < COLS && y >= 0 && y < ROWS && IsValidMove(pieceIndex, (Vector2){x * col_width, y * col_height}))
            {
                DrawRectangle(x * col_width, y * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
        }
        break;

    case 'B':
        for (int i = 1; i < COLS; i++)
        {
            if (startX + i < COLS && startY + i < ROWS && IsValidMove(pieceIndex, (Vector2){(startX + i) * col_width, (startY + i) * col_height}))
            {
                DrawRectangle((startX + i) * col_width, (startY + i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX - i >= 0 && startY - i >= 0 && IsValidMove(pieceIndex, (Vector2){(startX - i) * col_width, (startY - i) * col_height}))
            {
                DrawRectangle((startX - i) * col_width, (startY - i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX + i < COLS && startY - i >= 0 && IsValidMove(pieceIndex, (Vector2){(startX + i) * col_width, (startY - i) * col_height}))
            {
                DrawRectangle((startX + i) * col_width, (startY - i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX - i >= 0 && startY + i < ROWS && IsValidMove(pieceIndex, (Vector2){(startX - i) * col_width, (startY + i) * col_height}))
            {
                DrawRectangle((startX - i) * col_width, (startY + i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
        }
        break;

    case 'Q':
        for (int i = 1; i < COLS; i++)
        {
            if (startX + i < COLS && IsValidMove(pieceIndex, (Vector2){(startX + i) * col_width, startY * col_height}))
            {
                DrawRectangle((startX + i) * col_width, startY * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX - i >= 0 && IsValidMove(pieceIndex, (Vector2){(startX - i) * col_width, startY * col_height}))
            {
                DrawRectangle((startX - i) * col_width, startY * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startY + i < ROWS && IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY + i) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY + i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startY - i >= 0 && IsValidMove(pieceIndex, (Vector2){startX * col_width, (startY - i) * col_height}))
            {
                DrawRectangle(startX * col_width, (startY - i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX + i < COLS && startY + i < ROWS && IsValidMove(pieceIndex, (Vector2){(startX + i) * col_width, (startY + i) * col_height}))
            {
                DrawRectangle((startX + i) * col_width, (startY + i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX - i >= 0 && startY - i >= 0 && IsValidMove(pieceIndex, (Vector2){(startX - i) * col_width, (startY - i) * col_height}))
            {
                DrawRectangle((startX - i) * col_width, (startY - i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX + i < COLS && startY - i >= 0 && IsValidMove(pieceIndex, (Vector2){(startX + i) * col_width, (startY - i) * col_height}))
            {
                DrawRectangle((startX + i) * col_width, (startY - i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
            if (startX - i >= 0 && startY + i < ROWS && IsValidMove(pieceIndex, (Vector2){(startX - i) * col_width, (startY + i) * col_height}))
            {
                DrawRectangle((startX - i) * col_width, (startY + i) * col_height, col_width, col_height, Fade(GREEN, 0.5f));
            }
        }
        break;

    case 'K':
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0)
                    continue;
                int x = startX + i;
                int y = startY + j;
                if (x >= 0 && x < COLS && y >= 0 && y < ROWS && IsValidMove(pieceIndex, (Vector2){x * col_width, y * col_height}))
                {
                    DrawRectangle(x * col_width, y * col_height, col_width, col_height, Fade(GREEN, 0.5f));
                }
            }
        }
        if (!piece.hasMoved)
        {

            Vector2 kingsidePos = {(startX + 2) * col_width, startY * col_height};
            if (IsValidMove(pieceIndex, kingsidePos))
            {
                DrawRectangle(kingsidePos.x, kingsidePos.y,
                              col_width, col_height, Fade(GREEN, 0.5f));
            }

            Vector2 queensidePos = {(startX - 2) * col_width, startY * col_height};
            if (IsValidMove(pieceIndex, queensidePos))
            {
                DrawRectangle(queensidePos.x, queensidePos.y,
                              col_width, col_height, Fade(GREEN, 0.5f));
            }
        }

    default:
        break;
    }
}
void UpdateLastMove(int pieceIndex, Vector2 newPos)
{
    lastMove.startPos = pieces[pieceIndex].pos;
    lastMove.endPos = newPos;
    lastMove.pieceType = pieces[pieceIndex].type;
    lastMove.pieceisWhite = pieces[pieceIndex].isWhite;
}
void HandleEnPassant(int pieceIndex, Vector2 newPos)
{
    Piece piece = pieces[pieceIndex];
    if ((piece.type == 'P') &&
        abs((newPos.x - piece.pos.x) / col_width) == 1 &&
        abs((newPos.y - piece.pos.y) / col_height) == 1)
    {
        Vector2 capturedPos = {newPos.x, lastMove.endPos.y};
        for (int i = 0; i < pieceCount; i++)
        {
            if (pieces[i].pos.x == capturedPos.x &&
                pieces[i].pos.y == capturedPos.y &&
                pieces[i].isWhite != piece.isWhite &&
                (pieces[i].type == 'P'))
            {
                pieces[i].canDraw = false;
                break;
            }
        }
    }
}

bool IsValidMoveWithoutCheck(int pieceIndex, Vector2 newPos)
{
    Piece piece = pieces[pieceIndex];
    if (!piece.canDraw)
    {
        return false;
    }

    int dx = (newPos.x - piece.pos.x) / col_width;
    int dy = (newPos.y - piece.pos.y) / col_height;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    if (newPos.x < 0 || newPos.x >= screenWidth || newPos.y < 0 || newPos.y >= screenHeight)
    {
        return false;
    }

    for (int i = 0; i < pieceCount; i++)
    {
        if (i == pieceIndex || !pieces[i].canDraw)
            continue;
        if (IsVector2Equal(pieces[i].pos, newPos) && pieces[i].isWhite == piece.isWhite)
        {
            return false;
        }
    }

    switch (piece.type)
    {
    case 'P':
        if (piece.isWhite)
        {
            if (dx == 0)
            {
                if (dy == -1)
                {

                    for (int i = 0; i < pieceCount; i++)
                    {
                        if (i == pieceIndex || !pieces[i].canDraw)
                            continue;
                        if (IsVector2Equal(pieces[i].pos, newPos))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                else if (dy == -2 && piece.pos.y == screenHeight - 2 * col_height)
                {

                    for (int i = 0; i < pieceCount; i++)
                    {
                        if (i == pieceIndex || !pieces[i].canDraw)
                            continue;
                        if (pieces[i].pos.x == newPos.x && (pieces[i].pos.y == newPos.y || pieces[i].pos.y == newPos.y + col_height))
                        {
                            return false;
                        }
                    }
                    return true;
                }
            }
            else if (abs_dx == 1 && dy == -1)
            {

                for (int i = 0; i < pieceCount; i++)
                {
                    if (i == pieceIndex || !pieces[i].canDraw)
                        continue;
                    if (IsVector2Equal(pieces[i].pos, newPos) && !pieces[i].isWhite)
                    {
                        return true;
                    }
                }

                if (lastMove.pieceType == 'P' && lastMove.endPos.y == piece.pos.y &&
                    abs((lastMove.endPos.x - piece.pos.x) / col_width) == 1 &&
                    (lastMove.endPos.y - lastMove.startPos.y) / col_height == 2)
                {
                    if (newPos.x == lastMove.endPos.x && newPos.y == lastMove.endPos.y - col_height)
                    {
                        return true;
                    }
                }
            }
            return false;
        }
        else
        {
            if (dx == 0)
            {
                if (dy == 1)
                {

                    for (int i = 0; i < pieceCount; i++)
                    {
                        if (i == pieceIndex || !pieces[i].canDraw)
                            continue;
                        if (IsVector2Equal(pieces[i].pos, newPos))
                        {
                            return false;
                        }
                    }
                    return true;
                }
                else if (dy == 2 && piece.pos.y == col_height)
                {

                    for (int i = 0; i < pieceCount; i++)
                    {
                        if (i == pieceIndex || !pieces[i].canDraw)
                            continue;
                        if (IsVector2Equal(pieces[i].pos, newPos))
                        {

                            return false;
                        }
                    }
                    return true;
                }
            }
            else if (abs_dx == 1 && dy == 1)
            {

                for (int i = 0; i < pieceCount; i++)
                {
                    if (i == pieceIndex || !pieces[i].canDraw)
                        continue;
                    if (IsVector2Equal(pieces[i].pos, newPos) && pieces[i].isWhite)
                    {
                        return true;
                    }
                }

                if (lastMove.pieceType == 'P' && lastMove.endPos.y == piece.pos.y &&
                    abs((lastMove.endPos.x - piece.pos.x) / col_width) == 1 &&
                    (lastMove.endPos.y - lastMove.startPos.y) / col_height == -2)
                {

                    if (newPos.x == lastMove.endPos.x && newPos.y == lastMove.endPos.y + col_height)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

    case 'R':

        if (dy == 0)
        {

            for (int i = 0; i < pieceCount; i++)
            {
                if (i == pieceIndex || !pieces[i].canDraw)
                    continue;
                if ((pieces[pieceIndex].pos.x < pieces[i].pos.x && pieces[i].pos.x < newPos.x) && pieces[i].pos.y == pieces[pieceIndex].pos.y)
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x > pieces[i].pos.x && pieces[i].pos.x > newPos.x) && pieces[i].pos.y == pieces[pieceIndex].pos.y)
                {
                    return false;
                }
            }
            return true;
        }
        else if (dx == 0)
        {
            for (int i = 0; i < pieceCount; i++)
            {
                if (i == pieceIndex || !pieces[i].canDraw)
                    continue;
                if ((pieces[pieceIndex].pos.y < pieces[i].pos.y && pieces[i].pos.y < newPos.y) && pieces[i].pos.x == pieces[pieceIndex].pos.x)
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.y > pieces[i].pos.y && pieces[i].pos.y > newPos.y) && pieces[i].pos.x == pieces[pieceIndex].pos.x)
                {
                    return false;
                }
            }
            return true;
        }
        return false;

    case 'N':
        if ((abs_dx == 2 && abs_dy == 1) || (abs_dx == 1 && abs_dy == 2))
        {
            return true;
        }
        return false;

    case 'B':
        if (abs_dx == abs_dy)
        {
            for (int i = 0; i < pieceCount; i++)
            {
                if (i == pieceIndex || !pieces[i].canDraw)
                    continue;
                int new_dx = abs((newPos.x - pieces[i].pos.x) / col_width);
                int new_dy = abs((newPos.y - pieces[i].pos.y) / col_height);
                if (new_dx != new_dy)
                {
                    continue;
                }
                if ((pieces[pieceIndex].pos.x < pieces[i].pos.x && pieces[i].pos.x < newPos.x) && (pieces[pieceIndex].pos.y < pieces[i].pos.y && pieces[i].pos.y < newPos.y))
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x > pieces[i].pos.x && pieces[i].pos.x > newPos.x) && (pieces[pieceIndex].pos.y > pieces[i].pos.y && pieces[i].pos.y > newPos.y))
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x > pieces[i].pos.x && pieces[i].pos.x > newPos.x) && (pieces[pieceIndex].pos.y < pieces[i].pos.y && pieces[i].pos.y < newPos.y))
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x < pieces[i].pos.x && pieces[i].pos.x < newPos.x) && (pieces[pieceIndex].pos.y > pieces[i].pos.y && pieces[i].pos.y > newPos.y))
                {
                    return false;
                }
            }
            return true;
        }
        return false;

    case 'Q':
        if (abs_dx == abs_dy)
        {
            for (int i = 0; i < pieceCount; i++)
            {
                if (i == pieceIndex || !pieces[i].canDraw)
                    continue;
                int new_dx = abs((newPos.x - pieces[i].pos.x) / col_width);
                int new_dy = abs((newPos.y - pieces[i].pos.y) / col_height);
                if (new_dx != new_dy)
                {
                    continue;
                }
                if ((pieces[pieceIndex].pos.x < pieces[i].pos.x && pieces[i].pos.x < newPos.x) && (pieces[pieceIndex].pos.y < pieces[i].pos.y && pieces[i].pos.y < newPos.y))
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x > pieces[i].pos.x && pieces[i].pos.x > newPos.x) && (pieces[pieceIndex].pos.y > pieces[i].pos.y && pieces[i].pos.y > newPos.y))
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x > pieces[i].pos.x && pieces[i].pos.x > newPos.x) && (pieces[pieceIndex].pos.y < pieces[i].pos.y && pieces[i].pos.y < newPos.y))
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x < pieces[i].pos.x && pieces[i].pos.x < newPos.x) && (pieces[pieceIndex].pos.y > pieces[i].pos.y && pieces[i].pos.y > newPos.y))
                {
                    return false;
                }
            }
            return true;
        }
        if (dy == 0)
        {

            for (int i = 0; i < pieceCount; i++)
            {
                if (i == pieceIndex || !pieces[i].canDraw)
                    continue;
                if ((pieces[pieceIndex].pos.x < pieces[i].pos.x && pieces[i].pos.x < newPos.x) && pieces[i].pos.y == pieces[pieceIndex].pos.y)
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.x > pieces[i].pos.x && pieces[i].pos.x > newPos.x) && pieces[i].pos.y == pieces[pieceIndex].pos.y)
                {
                    return false;
                }
            }
            return true;
        }
        else if (dx == 0)
        {
            for (int i = 0; i < pieceCount; i++)
            {
                if (i == pieceIndex || !pieces[i].canDraw)
                    continue;
                if ((pieces[pieceIndex].pos.y < pieces[i].pos.y && pieces[i].pos.y < newPos.y) && pieces[i].pos.x == pieces[pieceIndex].pos.x)
                {
                    return false;
                }
                if ((pieces[pieceIndex].pos.y > pieces[i].pos.y && pieces[i].pos.y > newPos.y) && pieces[i].pos.x == pieces[pieceIndex].pos.x)
                {
                    return false;
                }
            }
            return true;
        }
        return false;

    case 'K':
        int dx = (newPos.x - piece.pos.x) / col_width;
        int dy = (newPos.y - piece.pos.y) / col_height;
        int abs_dx = abs(dx);
        int abs_dy = abs(dy);

        if (abs_dx == 2 && abs_dy == 0 && !piece.hasMoved)
        {
            int direction = dx > 0 ? 1 : -1;
            Vector2 rookStartPos = {
                (direction == 1) ? 7 * col_width : 0 * col_width,
                piece.pos.y};

            bool rookFound = false;
            for (int i = 0; i < pieceCount; i++)
            {
                if (pieces[i].type == 'R' &&
                    pieces[i].pos.x == rookStartPos.x &&
                    pieces[i].pos.y == rookStartPos.y &&
                    !pieces[i].hasMoved)
                {
                    rookFound = true;
                    break;
                }
            }
            if (!rookFound)
                return false;

            int step = direction;
            for (int x = 1; x <= 2; x++)
            {
                Vector2 checkPos = {
                    piece.pos.x + (x * step) * col_width,
                    piece.pos.y};

                if (x == 2)
                    continue;

                for (int i = 0; i < pieceCount; i++)
                {
                    if (pieces[i].canDraw &&
                        pieces[i].pos.x == checkPos.x &&
                        pieces[i].pos.y == checkPos.y)
                    {
                        return false;
                    }
                }
            }

            Vector2 kingPath1 = {piece.pos.x + direction * col_width, piece.pos.y};
            Vector2 kingPath2 = {piece.pos.x + 2 * direction * col_width, piece.pos.y};
            if (IsSquareUnderAttack(piece.pos, piece.isWhite) ||
                IsSquareUnderAttack(kingPath1, piece.isWhite) ||
                IsSquareUnderAttack(kingPath2, piece.isWhite))
            {
                return false;
            }

            return true;
        }
        if (abs_dx <= 1 && abs_dy <= 1)
        {
            return true;
        }

    default:
        return false;
    }
}
bool IsKingInCheck(bool isWhite)
{
    Vector2 kingPos = FindKingPosition(isWhite);
    if (kingPos.x == -1)
    {
        return false;
    }
    return IsSquareUnderAttack(kingPos, isWhite);
}
bool IsValidMove(int pieceIndex, Vector2 newPos)
{

    if (pieceIndex < 0 || pieceIndex >= pieceCount)
        return false;
    Piece originalPiece = pieces[pieceIndex];

    if (IsVector2Equal(originalPiece.pos, newPos))
        return false;
    if (!IsValidMoveWithoutCheck(pieceIndex, newPos))
        return false;

    int capturedIndex = -1;
    bool capturedCanDraw = false;
    for (int i = 0; i < pieceCount; i++)
    {
        if (i != pieceIndex && pieces[i].canDraw &&
            pieces[i].pos.x == newPos.x &&
            pieces[i].pos.y == newPos.y)
        {
            capturedIndex = i;
            capturedCanDraw = pieces[i].canDraw;
            pieces[i].canDraw = false;
            break;
        }
    }
    Vector2 originalPos = pieces[pieceIndex].pos;
    pieces[pieceIndex].pos = newPos;
    bool inCheck = IsKingInCheck(originalPiece.isWhite);

    pieces[pieceIndex].pos = originalPos;
    if (capturedIndex != -1)
    {
        pieces[capturedIndex].canDraw = capturedCanDraw;
    }

    return !inCheck;
}
bool IsSquareUnderAttack(Vector2 square, bool isWhite)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (!pieces[i].canDraw)
            continue;
        if (pieces[i].isWhite != isWhite)
        {
            if (IsValidMoveWithoutCheck(i, square))
            {
                return true;
            }
        }
    }
    return false;
}
bool IsVector2Equal(Vector2 a, Vector2 b)
{
    return a.x == b.x && a.y == b.y;
}
bool IsCheckMate(bool isWhite)
{
    bool isCheck = IsKingInCheck(isWhite);
    if (!isCheck)
    {
        return false;
    }
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].isWhite == isWhite && pieces[i].canDraw)
        {
            for (int x = 0; x < COLS; x++)
            {
                for (int y = 0; y < ROWS; y++)
                {
                    Vector2 newPos = {x * col_width, y * col_height};
                    if (IsValidMove(i, newPos))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

Vector2 FindKingPosition(bool isWhite)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].type == 'K' && pieces[i].isWhite == isWhite && pieces[i].canDraw)
        {
            return pieces[i].pos;
        }
    }
    return (Vector2){-1, -1};
}


