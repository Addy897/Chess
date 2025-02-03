#include <main.h>

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
            if(j==7){
                char text[24];
                sprintf(text,"%c",97+i);
                DrawText(text,i * col_width+5,j * col_height+80,16,BLACK);
            }
            if(i==7){
                char text[24];
                sprintf(text,"%d",8-j);
                DrawText(text,i * col_width+80,j * col_height+5,16,BLACK);
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
         Piece * p = getPiece(i);
        if (!p->canDraw)
        {
            continue;
        }
        Rectangle srcRec;
        int isBlack = !p->isWhite;
        switch (p->type)
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

        Vector2 pos = p->pos;
        if (p->isDragging)
        {
            pos = GetMousePosition();
            pos.x -= col_width / 2;
            pos.y -= col_height / 2;
        }

        Rectangle dstRec = {pos.x, pos.y, col_width, col_height};
        DrawTexturePro(piecesTexture, srcRec, dstRec, (Vector2){0, 0}, 0, WHITE);
    }
}
void DrawPromotionPopup(Texture2D piecesTexture)
{
    if (promotingPieceIndex == -1)
        return;
    if (!gameState.playingWithComputer || gameState.isWhiteTurn)
    {
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
    }
    else
    {
        pendingPromotion = PROMOTION_QUEEN;
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

        gameState.isWhiteTurn = !gameState.isWhiteTurn;
        currentMovesIndex=GenerateAllLegalMoves(gameState.isWhiteTurn,BoardMoves);
    }
}
void HighlightLegalMoves(int pieceIndex)
{
    for (int i = 0; i <currentMovesIndex; i++)
    {
        char index=getMovePieceIndex(BoardMoves[i]);
        if (pieceIndex==index)
        {
            Vector2 newPos =moveToVector2(BoardMoves[i]);


            DrawRectangle(newPos.x,newPos.y, col_width, col_height, Fade(GREEN, 0.5f));
        }
    }
}
void HighlightPreviousMove()
{
    DrawRectangle(lastMove.startPos.x, lastMove.startPos.y, col_width, col_height, Fade((Color){0xb9, 0xca, 0x42, 155}, 0.5f));
    DrawRectangle(lastMove.endPos.x, lastMove.endPos.y, col_width, col_height, Fade((Color){0xb9, 0xca, 0x42, 155}, 0.5f));
}
void PlayMoveOnBoard(Vector2 newPos)
{
    Piece * lastPiece =getPiece(lastSelectedPiece);
    if (IsVector2Equal(lastPiece->pos, newPos))
    {
        lastPiece->isDragging = false;
        lastSelectedPiece = -1;
        return;
    }
    if (IsLegalMove(lastSelectedPiece, newPos))
    {
        bool canMove = true;
        bool isCapture = false;

        for (int i = 0; i < pieceCount; i++)
        {
            Piece * p = getPiece(i);

            if (!p->canDraw || i == lastSelectedPiece)
            {
                continue;
            }
            if (IsVector2Equal(p->pos, newPos))
            {
                if (lastPiece->isWhite == p->isWhite || p->type == 'K')
                {
                    canMove = false;
                    break;
                }
                else
                {
                    p->canDraw = false;
                    PlaySound(captureSound);

                    break;
                }
            }
        }
        if (canMove)
        {
            if (lastPiece->type == 'K' && abs((newPos.x - lastPiece->pos.x) / col_width) == 2)
            {
                int direction = (newPos.x > lastPiece->pos.x) ? 1 : -1;
                Vector2 rookNewPos = {newPos.x - direction * col_width, newPos.y};

                for (int i = 0; i < pieceCount; i++)
                {
                    Piece * p = getPiece(i);

                    if (p->type == 'R' && p->pos.y == newPos.y)
                    {
                        if ((direction == 1 && p->pos.x == 7 * col_width) ||
                            (direction == -1 && p->pos.x == 0 * col_width))
                        {
                            p->pos = rookNewPos;
                            p->hasMoved = true;
                            break;
                        }
                    }
                }
            }
            HandleEnPassant(lastSelectedPiece, newPos);
            UpdateLastMove(lastSelectedPiece, newPos);
            if (lastPiece->pos.x != newPos.x || lastPiece->pos.y != newPos.y)
            {
                lastPiece->hasMoved = true;
            }
            lastPiece->pos = newPos;
            if (IsKingInCheck(!lastPiece->isWhite))
            {
                PlaySound(checkSound);
            }
            else
            {
                PlaySound(moveSound);
            }
            if (lastPiece->type == 'P' && IsPromotionSquare(newPos, lastPiece->isWhite))
            {
                promotingPieceIndex = lastSelectedPiece;
            }

            if (promotingPieceIndex == -1)
            {
                gameState.isWhiteTurn = !gameState.isWhiteTurn;
                currentMovesIndex=GenerateAllLegalMoves(gameState.isWhiteTurn,BoardMoves);
            }
        }
    }
    lastPiece->isDragging = false;
    lastSelectedPiece = -1;
}
