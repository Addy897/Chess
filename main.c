#include "main.h"
static Piece pieces[ROWS * COLS] = {0};
int main(int argc, char const *argv[])
{
    char fen[100];
    preComputeAll();
    strcpy(fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    int depth = 0;
    if (argc == 3)
    {
        if (!strcmp(argv[1], "-f"))
        {
            strcpy(fen, argv[2]);
        }
        else if (!strcmp(argv[1], "-d"))
        {
            depth = atoi(argv[2]);
            char *token = strtok(fen, " ");
            SetupPiecesFromFEN(token);
            clock_t begin = clock();
            for (int i = 1; i <= depth; i++)
            {
                int mvs = CalulatePossiblePosition(i,true);
                printf("%d: %d\n", i, mvs);
            }
            clock_t end = clock();
            double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
            printf("The elapsed time is %f seconds", time_spent);
            return 0;
        }
    }
    const char *title = "Chess with FEN";
    InitWindow(screenWidth + SIDE_WIDTH, screenHeight, title);
    InitAudioDevice();
    moveSound = LoadSound("C:/Users/adity/Documents/C_C++/GUi/resources/move.mp3");
    captureSound = LoadSound("C:/Users/adity/Documents/C_C++/GUi/resources/capture.mp3");
    checkSound = LoadSound("C:/Users/adity/Documents/C_C++/GUi/resources/check.mp3");
    Texture2D piecesTexture = LoadTexture("C:/Users/adity/Documents/C_C++/GUi/resources/chess-pieces.png");
    SetTextureFilter(piecesTexture, TEXTURE_FILTER_BILINEAR);

    char *token = strtok(fen, " ");
    SetupPiecesFromFEN(token);
    gameState.playingWithComputer = true;
    gameState.isWhiteTurn = strtok(NULL, " ")[0] == 'w';
    GenerateAllLegalMoves(gameState.isWhiteTurn);
    SetTargetFPS(144);
    while (!WindowShouldClose())
    {
        
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(screenWidth, 0, SIDE_WIDTH, screenHeight, BLACK);

        DrawBoard();

        if (lastSelectedPiece != -1)
        {
            HighlightLegalMoves(lastSelectedPiece);
        }
        if (gameState.isWhiteTurn)
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
        else if (IsStaleMate(!lastMove.pieceisWhite))
        {
            DrawText("Draw", screenWidth + (SIDE_WIDTH / 8), SIDE_WIDTH, 24, WHITE);
        }

        HighlightPreviousMove();

        DrawPieces(piecesTexture);
        if (promotingPieceIndex != -1)
        {
            DrawPromotionPopup(piecesTexture);
        }
        EndDrawing();
        if (gameState.playingWithComputer && (!(gameState.state & 0) || !(gameState.state & 8)))
        {
            if (gameState.isWhiteTurn)
            {
                CheckForInput();
            }
            else
            {
                PlayMove(gameState.isWhiteTurn);
            }
        }
        else
        {
            CheckForInput();
        }

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
            pieces[pieceCount].moveIndex = 0;

            pieces[pieceCount].hasMoved = false;
            pieceCount++;
            col++;
        }
    }
}
void HighlightLegalMoves(int pieceIndex)
{
    Piece piece = pieces[pieceIndex];
    for (int i = 0; i < piece.moveIndex; i++)
    {
        if (piece.canDraw)
        {

            DrawRectangle(piece.moves[i].endPos.x, piece.moves[i].endPos.y, col_width, col_height, Fade(GREEN, 0.5f));
        }
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
        GenerateAllLegalMoves(gameState.isWhiteTurn);
    }
}
void AddMove(int pieceIndex, Vector2 endPos)
{
    Piece *piece = &pieces[pieceIndex];
    Move move;
    move.startPos = piece->pos;
    move.endPos = endPos;
    move.pieceisWhite = piece->isWhite;
    move.pieceType = piece->type;
    move.capturePieceType=IsCapture(endPos,pieceIndex);
    Vector2 kinPos=FindKingPosition(!move.pieceisWhite);
    endPos=piece->pos;
    piece->pos=move.endPos;
    move.isCheck=IsValidMoveWithoutCheck(pieceIndex,kinPos);
    piece->pos=endPos;
    piece->moves[piece->moveIndex++] = move;
   
}
int CompareMoves(const void *a, const void *b) {
    Move *moveA = (Move *)a;
    Move *moveB = (Move *)b;
    return GetMovePriority(moveB) - GetMovePriority(moveA);
}
void SortMovesByPriority(Piece *piece) {
    qsort(piece->moves, piece->moveIndex, sizeof(Move), CompareMoves);
}

int GetPieceValue(char type){
    switch (type)
    {
    case 'P':
        return 100;
    case 'K':
        return 20000;
    case 'Q':
        return 900;
    case 'B':
        return 330;
    case 'R':
        return 500;
    case 'N':
        return 300;
    default:
        break;
    }
}

void GenerateRookMoves(int pieceIndex,int startX,int startY){
     for (int i = startX-1; i>=0; i--)
        {
            
            Vector2 move = {(i) * col_width, startY * col_height};
            if (IsValidMove(pieceIndex, move))
            {
                AddMove(pieceIndex, move);
            }else{
                break;
            }
            
        }
        for (int i = startX+1; i < COLS; i++)
        {
           
                Vector2 move = {(i) * col_width, startY * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }
        for (int i = startY+1; i < ROWS; i++)
        {
            
                Vector2 move = {startX * col_width, ( i) * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }
        for (int i = startY - 1; i >=0; i--)
        {
           
                Vector2 move = {startX * col_width, (i) * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }

}
void GenerateBishopMoves(int pieceIndex,int startX,int startY){
    for (int i = startX+1,j = startY + 1; i < COLS&&j<ROWS; i++,j++)
        {
            
                Vector2 move = {(i) * col_width, (j) * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }
        for (int i = startX-1,j=startY-1; i >=0&&j>=0; i--,j--)
        {
            
                Vector2 move = {(i) * col_width, (j) * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }
        for (int i = startX+1,j=startY-1; i < COLS&&j>=0; i++,j--)
        {
            
                Vector2 move = {(i) * col_width, (j) * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }
        for (int i = startX-1,j=startY+1; j < COLS&&i>=0; i--,j++)
        {
           
                Vector2 move = {(i) * col_width, (j) * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }else{
                    break;
                }
            
        }
        
}
void GenerateAllLegalMovesForPiece(int pieceIndex)
{
    Piece piece = pieces[pieceIndex];
    pieces[pieceIndex].moveIndex = 0;

    int startX = (int)(piece.pos.x / col_width);
    int startY = (int)(piece.pos.y / col_height);

    switch (piece.type)
    {
    case 'P':
    {
        int t_dx = piece.isWhite ? -1 : 1;
        int t_dx_2 = piece.isWhite ? -2 : 2;

        Vector2 singleMove = {startX * col_width, (startY + t_dx) * col_height};
        if (IsValidMove(pieceIndex, singleMove))
        {
            AddMove(pieceIndex, singleMove);
        }

        if ((piece.isWhite && startY == 6) || (!piece.isWhite && startY == 1))
        {
            Vector2 doubleMove = {startX * col_width, (startY + t_dx_2) * col_height};
            if (IsValidMove(pieceIndex, doubleMove))
            {
                AddMove(pieceIndex, doubleMove);
            }
        }

        Vector2 captureLeft = {(startX - 1) * col_width, (startY + t_dx) * col_height};
        if (startX > 0 && IsValidMove(pieceIndex, captureLeft))
        {
            AddMove(pieceIndex, captureLeft);
        }

        Vector2 captureRight = {(startX + 1) * col_width, (startY + t_dx) * col_height};
        if (startX < COLS - 1 && IsValidMove(pieceIndex, captureRight))
        {
            AddMove(pieceIndex, captureRight);
        }
        break;
    }

    case 'R':
    {
       
        GenerateRookMoves(pieceIndex,startX,startY);      
        break;
    }

    case 'N':
    {
        int knightMoves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
        for (int i = 0; i < 8; i++)
        {
            int x = startX + knightMoves[i][0];
            int y = startY + knightMoves[i][1];
            if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
            {
                Vector2 move = {x * col_width, y * col_height};
                if (IsValidMove(pieceIndex, move))
                {
                    AddMove(pieceIndex, move);
                }
            }
        }
        break;
    }

    case 'B':
    {
        GenerateBishopMoves(pieceIndex,startX,startY);
        break;
        
    }

    case 'Q':
    {

        GenerateRookMoves(pieceIndex,startX,startY);
        GenerateBishopMoves(pieceIndex,startX,startY);
        break;
    }

    case 'K':
    {

        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0)
                    continue;
                int x = startX + i;
                int y = startY + j;
                if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
                {
                    Vector2 move = {x * col_width, y * col_height};
                    if (IsValidMove(pieceIndex, move))
                    {
                        AddMove(pieceIndex, move);
                    }
                }
            }
        }

        if (!piece.hasMoved)
        {

            if (IsValidMove(pieceIndex, (Vector2){(startX + 2) * col_width, startY * col_height}))
            {
                AddMove(pieceIndex, (Vector2){(startX + 2) * col_width, startY * col_height});
            }

            if (IsValidMove(pieceIndex, (Vector2){(startX - 2) * col_width, startY * col_height}))
            {
                AddMove(pieceIndex, (Vector2){(startX - 2) * col_width, startY * col_height});
            }
        }
        break;
    }

    default:
        break;
    }
}
void GenerateAllLegalMoves(bool forWhite)
{
    currentMovesIndex = -1;
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].isWhite == forWhite && pieces[i].canDraw)
        {
            GenerateAllLegalMovesForPiece(i);
            Piece * p=&pieces[i];
            SortMovesByPriority(p);
            currentMovesIndex += pieces[i].moveIndex;
        }
    }
    
}
void CheckForInput()
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePos = GetMousePosition();
        for (int i = 0; i < pieceCount; i++)
        {
            Rectangle pieceRect = {pieces[i].pos.x, pieces[i].pos.y, col_width, col_height};

            if (CheckCollisionPointRec(mousePos, pieceRect) && pieces[i].canDraw)
            {
                if (pieces[i].isWhite != gameState.isWhiteTurn)
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
                Vector2 newPos = (Vector2){col * col_width, row * col_height};
                PlayMoveOnBoard(newPos);
            }
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
    if (IsVector2Equal(pieces[lastSelectedPiece].pos, newPos))
    {
        pieces[lastSelectedPiece].isDragging = false;
        lastSelectedPiece = -1;
        return;
    }
    if (IsLegalMove(lastSelectedPiece, newPos))
    {
        bool canMove = true;
        bool isCapture = false;

        for (int i = 0; i < pieceCount; i++)
        {
            if (!pieces[i].canDraw || i == lastSelectedPiece)
            {
                continue;
            }
            if (IsVector2Equal(pieces[i].pos, newPos))
            {
                if (pieces[lastSelectedPiece].isWhite == pieces[i].isWhite || pieces[i].type == 'K')
                {
                    canMove = false;
                    break;
                }
                else
                {
                    pieces[i].canDraw = false;
                    PlaySound(captureSound);

                    break;
                }
            }
        }
        if (canMove)
        {
            if (pieces[lastSelectedPiece].type == 'K' && abs((newPos.x - pieces[lastSelectedPiece].pos.x) / col_width) == 2)
            {
                int direction = (newPos.x > pieces[lastSelectedPiece].pos.x) ? 1 : -1;
                Vector2 rookNewPos = {newPos.x - direction * col_width, newPos.y};

                for (int i = 0; i < pieceCount; i++)
                {
                    if (pieces[i].type == 'R' && pieces[i].pos.y == newPos.y)
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
            HandleEnPassant(lastSelectedPiece, newPos);

            UpdateLastMove(lastSelectedPiece, newPos);
            if (pieces[lastSelectedPiece].pos.x != newPos.x || pieces[lastSelectedPiece].pos.y != newPos.y)
            {
                pieces[lastSelectedPiece].hasMoved = true;
            }
            pieces[lastSelectedPiece].pos = newPos;
            if (IsKingInCheck(!pieces[lastSelectedPiece].isWhite))
            {
                PlaySound(checkSound);
            }
            else if (isCapture)
            {
            }
            else
            {
                PlaySound(moveSound);
            }
            if (pieces[lastSelectedPiece].type == 'P' && IsPromotionSquare(newPos, pieces[lastSelectedPiece].isWhite))
            {
                promotingPieceIndex = lastSelectedPiece;
            }

            if (promotingPieceIndex == -1)
            {
                gameState.isWhiteTurn = !gameState.isWhiteTurn;
                GenerateAllLegalMoves(gameState.isWhiteTurn);
            }
        }
    }
    pieces[lastSelectedPiece].isDragging = false;
    lastSelectedPiece = -1;
}
void PlayMove(bool withWhite)
{
    int score = INT_MAX;
    clock_t begin =clock();
    if(gameState.state!=0)
    {
        return;
    }
    GenerateAllLegalMoves(withWhite);
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].canDraw&&pieces[i].isWhite == withWhite)
        {
            Piece *piece = &pieces[i];
            
            
            for (int j = 0; j < pieces[i].moveIndex; j++)
            {
                Vector2 newPos = piece->moves[j].endPos;

                if(IsValidMove(i,newPos)){
              
                    MoveState state = ApplyMove(i, newPos);

                    int eval = AlphaBeta(3, INT_MIN, INT_MAX, !withWhite);

                    UndoMove(state);
                    if (eval < score)
                    {
                        gameState.computerMove = piece->moves[j];
                        gameState.computerPieceIndex = i;
                        score = eval;
                    }
                }else{
                    printf("Invalid: %c --> %d %d\n",piece->type,newPos.x,newPos.y);
                }
                
            }
        }
    }
    clock_t end=clock();
    double timeSpent=(end-begin)/CLOCKS_PER_SEC;
    printf("TOOK: %f Score: %d\n",timeSpent,score);
    lastSelectedPiece = gameState.computerPieceIndex;
    PlayMoveOnBoard(gameState.computerMove.endPos);
}
bool IsLegalMove(int pieceIndex, Vector2 newPos)
{
    Piece piece = pieces[pieceIndex];
    for (int i = 0; i < piece.moveIndex; i++)
    {
        if (IsVector2Equal(piece.moves[i].endPos, newPos))
        {
            return true;
        }
    }
    return false;
}
bool IsPieceInValidOrSame(int p1, int p2)
{
    return p1 == p2 || pieces[p1].canDraw || pieces[p1].isWhite == pieces[p2].isWhite;
}
bool IsWithinBoard(Vector2 pos)
{
    return pos.x >= 0 && pos.x < screenWidth && pos.y >= 0 && pos.y < screenHeight;
}

bool IsPieceBetween(Vector2 start, Vector2 end, int skipIndex)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (i == skipIndex || !pieces[i].canDraw)
            continue;

        if (start.y == end.y && start.y == pieces[i].pos.y)
        {
            if ((start.x < pieces[i].pos.x && pieces[i].pos.x < end.x) ||
                (start.x > pieces[i].pos.x && pieces[i].pos.x > end.x))
            {
                return true;
            }
        }

        if (start.x == end.x && start.x == pieces[i].pos.x)
        {
            if ((start.y < pieces[i].pos.y && pieces[i].pos.y < end.y) ||
                (start.y > pieces[i].pos.y && pieces[i].pos.y > end.y))
            {
                return true;
            }
        }
    }
    return false;
}

bool IsPieceBetweenDiagonal(Vector2 start, Vector2 end, int skipIndex)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (i == skipIndex || !pieces[i].canDraw)
            continue;

        int new_dx = abs((end.x - pieces[i].pos.x) / col_width);
        int new_dy = abs((end.y - pieces[i].pos.y) / col_height);
        if (new_dx != new_dy)
            continue;

        if ((start.x < pieces[i].pos.x && pieces[i].pos.x < end.x) &&
            (start.y < pieces[i].pos.y && pieces[i].pos.y < end.y))
            return true;
        if ((start.x > pieces[i].pos.x && pieces[i].pos.x > end.x) &&
            (start.y > pieces[i].pos.y && pieces[i].pos.y > end.y))
            return true;
        if ((start.x > pieces[i].pos.x && pieces[i].pos.x > end.x) &&
            (start.y < pieces[i].pos.y && pieces[i].pos.y < end.y))
            return true;
        if ((start.x < pieces[i].pos.x && pieces[i].pos.x < end.x) &&
            (start.y > pieces[i].pos.y && pieces[i].pos.y > end.y))
            return true;
    }
    return false;
}

bool ValidatePawnMove(Piece piece, Vector2 newPos, int dx, int dy)
{
    bool isWhitePawn = piece.isWhite;
    int moveDirection = isWhitePawn ? -1 : 1;
    int startRank = isWhitePawn ? 6 : 1;

    if (dx == 0)
    {

        if (dy == moveDirection && !IsSquareOccupied(newPos))
        {
            return true;
        }

        if (dy == 2 * moveDirection &&
            piece.pos.y / col_height == startRank &&
            !IsSquareOccupied(newPos) &&
            !IsPieceBetween(piece.pos, newPos, -1))
        {
            return true;
        }
    }

    else if (abs(dx) == 1 && dy == moveDirection)
    {

        if (IsSquareOccupied(newPos))
        {
            for (int i = 0; i < pieceCount; i++)
            {
                if (IsVector2Equal(pieces[i].pos, newPos) &&
                    pieces[i].canDraw &&
                    pieces[i].isWhite != isWhitePawn)
                {
                    return true;
                }
            }
        }

        if (lastMove.pieceType == 'P' &&
            abs((lastMove.endPos.y - lastMove.startPos.y) / col_height) == 2 &&
            lastMove.endPos.y == piece.pos.y &&
            abs((lastMove.endPos.x - piece.pos.x) / col_width) == 1)
        {
            Vector2 enPassantSquare = {lastMove.endPos.x,
                                       lastMove.endPos.y + moveDirection * col_height};
            if (IsVector2Equal(newPos, enPassantSquare))
            {
                return true;
            }
        }
    }
    return false;
}

bool IsValidMoveWithoutCheck(int pieceIndex, Vector2 newPos)
{
    Piece piece = pieces[pieceIndex];
    if (!piece.canDraw || !IsWithinBoard(newPos))
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

    int dx = (newPos.x - piece.pos.x) / col_width;
    int dy = (newPos.y - piece.pos.y) / col_height;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    switch (piece.type)
    {
    case 'P':
        return ValidatePawnMove(piece, newPos, dx, dy);

    case 'R':
        return ((dx == 0 && dy != 0) || (dy == 0 && dx != 0)) &&
               !IsPieceBetween(piece.pos, newPos, pieceIndex);

    case 'N':
        return (abs_dx == 2 && abs_dy == 1) || (abs_dx == 1 && abs_dy == 2);

    case 'B':
        return abs_dx == abs_dy && !IsPieceBetweenDiagonal(piece.pos, newPos, pieceIndex);

    case 'Q':
        return ((abs_dx == abs_dy) || (dx == 0 && dy != 0) || (dy == 0 && dx != 0)) &&
               !IsPieceBetween(piece.pos, newPos, pieceIndex) &&
               !IsPieceBetweenDiagonal(piece.pos, newPos, pieceIndex);

    case 'K':
    {

        if (abs_dx <= 1 && abs_dy <= 1)
            return true;

        if (abs_dx == 2 && dy == 0 && !piece.hasMoved)
        {
            int direction = dx > 0 ? 1 : -1;
            Vector2 rookStartPos = {direction > 0 ? 7 * col_width : 0, piece.pos.y};

            bool rookFound = false;
            for (int i = 0; i < pieceCount; i++)
            {
                if (pieces[i].type == 'R' &&
                    IsVector2Equal(pieces[i].pos, rookStartPos) &&
                    !pieces[i].hasMoved)
                {
                    rookFound = true;
                    break;
                }
            }
            if (!rookFound)
                return false;

            if (IsPieceBetween(piece.pos, newPos, pieceIndex))
                return false;

            Vector2 intermediate = {piece.pos.x + direction * col_width, piece.pos.y};
            return !IsSquareUnderAttack(piece.pos, piece.isWhite) &&
                   !IsSquareUnderAttack(intermediate, piece.isWhite) &&
                   !IsSquareUnderAttack(newPos, piece.isWhite);
        }
        return false;
    }
    }
    return false;
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

    Vector2 originalPos = pieces[pieceIndex].pos;
    int capturedIndex = -1;
    int enpassantCapturedIndex = -1;

    MoveState Move = ApplyMove(pieceIndex, newPos);
    pieces[pieceIndex].pos = newPos;

    bool stillInCheck = IsKingInCheck(originalPiece.isWhite);
    UndoMove(Move);

    return !stillInCheck;
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
int IsCapture(Vector2 square, bool isWhite)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (!pieces[i].canDraw)
            continue;
        if (pieces[i].isWhite != isWhite)
        {
            if (IsVector2Equal(pieces[i].pos, square))
            {
                return pieces[i].type;
            }
        }
    }
    return -1;
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

    if (isWhite)
    {
        gameState.state = 1 << 2;
    }
    else
    {
        gameState.state = 1 << 1;
    }
    gameState.state |= 1;
    return true;
}
bool IsPromotionSquare(Vector2 pos, bool isWhite)
{
    return (isWhite && pos.y == 0) || (!isWhite && pos.y == screenHeight - col_height);
}
bool IsStaleMate(bool isWhite)
{
    bool isCheck = IsKingInCheck(isWhite);
    if (isCheck)
    {
        return false;
    }
    int totalPieceCount = 0;
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].canDraw)
        {
            totalPieceCount++;
        }
        if (totalPieceCount > 2)
            break;
    }
    if (totalPieceCount == 2)
    {
        gameState.state = 1 << 3;
        gameState.state |= 1;
        return true;
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

    gameState.state = 1 << 3;
    gameState.state |= 1;
    return true;
}
int GetMovePriority(Move *move)
{
    if (move->capturePieceType){
            int capt=GetPieceValue(move->capturePieceType);
            int val=GetPieceValue(move->pieceType);
            return (capt-val)*100;
    }
    if (move->isCheck)
        return 50;
       
    return 0;
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
bool IsSquareOccupied(Vector2 pos)
{
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].canDraw && IsVector2Equal(pieces[i].pos, pos))
        {
            return true;
        }
    }
    return false;
}

bool IsIsolatedPawn(int pawnIndex)
{
    Piece pawn = pieces[pawnIndex];
    int file = pawn.pos.x / col_width;

    bool hasLeftPawn = false, hasRightPawn = false;
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].type == 'P' && pieces[i].isWhite == pawn.isWhite && pieces[i].canDraw)
        {
            int otherFile = pieces[i].pos.x / col_width;
            if (otherFile == file - 1)
                hasLeftPawn = true;
            if (otherFile == file + 1)
                hasRightPawn = true;
        }
    }

    return !hasLeftPawn && !hasRightPawn;
}

int HasDoubledPawns(bool isWhite)
{
    int pawnCount[8] = {0};
    int count = 0;
    for (int i = 0; i < pieceCount; i++)
    {
        if (pieces[i].type == 'P' && pieces[i].isWhite == isWhite && pieces[i].canDraw)
        {
            int file = pieces[i].pos.x / col_width;
            pawnCount[file]++;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if (pawnCount[i] >= 2)
        {
            count += 1;
        }
    }
    return count;
}

bool IsBlockedPawn(int pawnIndex)
{
    Piece pawn = pieces[pawnIndex];
    Vector2 squareInFront = {pawn.pos.x, pawn.pos.y + (pawn.isWhite ? -col_height : col_height)};
    return IsSquareOccupied(squareInFront);
}


int numOfPieces(){
    int n=0;
    for (int i = 0; i < pieceCount; i++)
    {
        if(pieces[i].canDraw){
            n++;
        }
    }
    return n;
    
}
bool IsEdgeSquare(Vector2 pos) {
    int x = pos.x / col_width;
    int y = pos.y / col_height;
    return x == 0 || x == 7 || y == 0 || y == 7;
}
bool IsEndgame() {
    int total_pieces = 0;
    bool has_queen = false;
    
    for (int i = 0; i < pieceCount; i++) {
        if (pieces[i].canDraw) {
            total_pieces++;
            if (pieces[i].type == 'Q') has_queen = true;
        }
    }
    
    return (total_pieces <= 6 && !has_queen) || (total_pieces <= 3);
}
int EvaluateKingCornering(bool isWhite, Vector2 kingPos) {
    Vector2 corners[4] = {
        {0, 7*col_height},          
        {7*col_width, 7*col_height}, 
        {0, 0},                     
        {7*col_width, 0}                
        };

    int min_distance = INT_MAX;
    for (int i = 0; i < 4; i++) {
        int dx = abs((kingPos.x - corners[i].x)/col_width);
        int dy = abs((kingPos.y - corners[i].y)/col_height);
        int distance = dx + dy;
        if (distance < min_distance) min_distance = distance;
    }

    return isWhite ? -(60 * min_distance) : (60 * min_distance);
}
int Eval()
{
    bool endGame = IsEndgame();  

    int C = 0, C_ = 0;
    int K = 0, K_ = 0, Q = 0, Q_ = 0, P = 0, P_ = 0, B = 0, B_ = 0, N = 0, N_ = 0, R = 0, R_ = 0;
    int CheckScore = 0;
    int D = 0, D_ = 0, S = 0, S_ = 0, I = 0, I_ = 0;
    int centerFileBonus = 0;  
    int promotionBonus = 0;   
    int eval = 0; 
    int cornerBonus=0;
    Vector2 blackKing;
    Vector2 whiteKing;
    for (int i = 0; i < pieceCount; i++)
    {
        if (!pieces[i].canDraw) continue;

        if (pieces[i].isWhite)
        {
            K += pieces[i].type == 'K';
            Q += pieces[i].type == 'Q';
            P += pieces[i].type == 'P';
            R += pieces[i].type == 'R';
            N += pieces[i].type == 'N';
            B += pieces[i].type == 'B';
        }
        else
        {
            K_ += pieces[i].type == 'K';
            Q_ += pieces[i].type == 'Q';
            P_ += pieces[i].type == 'P';
            R_ += pieces[i].type == 'R';
            N_ += pieces[i].type == 'N';
            B_ += pieces[i].type == 'B';
        }
        
        if(pieces[i].type=='K'){
           if(pieces[i].isWhite){
            whiteKing=pieces[i].pos;

           }else{
            blackKing=pieces[i].pos;
           }
        }
        if (pieces[i].type == 'P')
        {
            int x=pieces[i].pos.x,y=pieces[i].pos.y;
          
            
            if (pieces[i].isWhite)
            {
                
                D += HasDoubledPawns(true);
                I += IsIsolatedPawn(i);
                S += IsBlockedPawn(i);

                if (endGame && pieces[i].pos.y <=1)  
                    promotionBonus += 1000;  
            }
            else
            {
               
                D_ += HasDoubledPawns(false);
                I_ += IsIsolatedPawn(i);
                S_ += IsBlockedPawn(i);
                
                if (endGame && pieces[i].pos.y >= 6)  
                    promotionBonus -= 1000;  
            }
        }
    }

    
    if (IsCheckMate(true)) K = 0;
    else if (IsCheckMate(false)) K_ = 0;
    
    if(endGame){
        cornerBonus += EvaluateKingCornering(true, blackKing);  
        cornerBonus -= EvaluateKingCornering(false, whiteKing);
    }
    eval+=cornerBonus;
    eval += 900 * (Q - Q_);    
    eval += 500 * (R - R_);    
    eval += 330 * (B - B_);    
    eval += 320 * (N - N_);    
    eval += 100 * (P - P_);    
    eval += 20000 * (K - K_);   
    
    
    eval -= 20 * (D - D_);     
    eval -= 30 * (I - I_);     
    eval -= 10 * (S - S_);     

    
    eval += centerFileBonus;   
    eval += promotionBonus;    

    return eval;
}
int GetEndgameMovePriority(Move *move) {
    Vector2 opponentKing = FindKingPosition(!move->pieceisWhite);
    
    int dx = abs(move->endPos.x/col_width - opponentKing.x/col_width);
    int dy = abs(move->endPos.y/col_height - opponentKing.y/col_height);
    int distance = dx + dy;

    int priority = 0;
    
    if (move->isCheck) priority += 1000;
    if (move->pieceType == 'K') priority += 500 - distance;
    if (IsEdgeSquare(move->endPos)) priority += 300;
    
    return priority;
}
int CalulatePossiblePosition(int depth, bool isWhiteTurn) {
    if (depth == 0) {
        return 1;  
    }

    int totalMoves = 0;

    
    for (int i = 0; i < pieceCount; i++) {
        
        if (pieces[i].isWhite == isWhiteTurn && pieces[i].canDraw) {

            Squares possibleMoves;
            int index=63-((pieces[i].pos.y/col_height)*8+(pieces[i].pos.x/col_width));
            
            switch (pieces[i].type) {
                case 'P':
                    possibleMoves = pieces[i].isWhite ? whitePawnSquares[index] : blackPawnSquares[index];
                    break;
                case 'N':
                    possibleMoves = knightSquares[index];
                    break;
                case 'B':
                    possibleMoves = bishopSquares[index];
                    break;
                case 'R':
                    possibleMoves = rookSquares[index];
                    break;
                case 'Q':
                    possibleMoves = queenSquares[index];
                    break;
                case 'K':
                    possibleMoves = kingSquares[index];
                    break;
                default:
                    break;
            }
            if (possibleMoves.n != -1) {
                for (int j = 0; j < possibleMoves.n; j++) {  
                    char sq = 63-possibleMoves.to[j];
                    Vector2 newPos=(Vector2){.x=(sq % 8)*col_width,.y=(sq/8)*col_height};
                    // char from[3];
                    // char to[3];
                    // squareToNotation(index,from);
                    // squareToNotation(possibleMoves.to[j],to);
                    // printf("%c %c %s %s\n",pieces[i].isWhite?'W':'B',pieces[i].type,from,to)  ;

                    
                    if (IsValidMove(i, newPos)) {
                        
                        if (pieces[i].type == 'P' && IsPromotionSquare(newPos, pieces[i].isWhite)) {
                            char promotions[] = "QRNB";

                            
                            MoveState state = ApplyMove(i, newPos);
                            state.promoteTo = 'P';  

                            
                            for (int k = 0; k < 4; k++) {
                                pieces[i].type = promotions[k];  
                                
                                
                                totalMoves += CalulatePossiblePosition(depth - 1, !isWhiteTurn);
                            }

                            
                            pieces[i].type = 'P';
                            UndoMove(state);
                        }
                        else {
                            
                            MoveState state = ApplyMove(i, newPos);
                            
                            
                            totalMoves += CalulatePossiblePosition(depth - 1, !isWhiteTurn);

                            
                            UndoMove(state);
                        }
                    }

                }
            }
        }
    }

    return totalMoves;  
}
int AlphaBeta(int depth, int alpha, int beta, bool isMaximizingPlayer)
{
    if (depth == 0 || IsCheckMate(isMaximizingPlayer) || IsStaleMate(isMaximizingPlayer))
    {
        return Eval();
    }
         
   

   

    if (isMaximizingPlayer)
    {
        int maxEval = INT_MIN;
        for (int i = 0; i < pieceCount; i++)
        {
            if (pieces[i].canDraw && pieces[i].isWhite)
            {
                for (int x = 0; x < COLS; x++)
                {
                    for (int y = 0; y < ROWS; y++)
                    {
                        Vector2 newPos = {x * col_width, y * col_height};

                        if (IsValidMove(i, newPos))
                        {
                            MoveState state = ApplyMove(i, newPos);

                            if (pieces[i].type == 'P' && IsPromotionSquare(newPos, pieces[i].isWhite))
                            {
                                char promotions[] = "QRNB";

                                state.promoteTo ='P';
                                for (int j = 0; j < 4; j++)
                                {
                                    pieces[i].type = promotions[j];
                                    int eval = AlphaBeta(depth - 1, alpha, beta, false);
                                    maxEval = (eval > maxEval) ? eval : maxEval;
                                    
                                }
                                pieces[i].type= 'P';

                                
                            }
                            else
                            {
                                int eval = AlphaBeta(depth - 1, alpha, beta, false);
                                maxEval = (eval > maxEval) ? eval : maxEval;

                               
                            }
                                UndoMove(state);
                                alpha = (alpha > maxEval) ? alpha : maxEval;
                                if (beta <= alpha)
                                {
                                    return maxEval;
                                }
                        }
                    }
                }
            }
        }
        return maxEval;
    }
    else
    {
        int minEval = INT_MAX;
        for (int i = 0; i < pieceCount; i++)
        {
            if (pieces[i].canDraw && !pieces[i].isWhite)
            {
                for (int x = 0; x < COLS; x++)
                {
                    for (int y = 0; y < ROWS; y++)
                    {
                        Vector2 newPos = {x * col_width, y * col_height};
                        if (IsValidMove(i, newPos))
                        {
                            MoveState state = ApplyMove(i, newPos);

                            if (pieces[i].type == 'P' && IsPromotionSquare(newPos, pieces[i].isWhite))
                            {
                                char promotions[] = "QRNB";

                                state.promoteTo = 'P';
                                for (int j = 0; j < 4; j++)
                                {
                                    pieces[i].type = promotions[j];
                                    int eval = AlphaBeta(depth - 1, alpha, beta, true);
                                    minEval = (eval < minEval) ? eval : minEval;
                                    
                                }
                                pieces[i].type= 'P';

                                

                                
                            }
                            else
                            {

                                int eval = AlphaBeta(depth - 1, alpha, beta, true);
                                minEval = (eval < minEval) ? eval : minEval;


                                
                            }
                            UndoMove(state);

                            beta = (beta < minEval) ? beta : minEval;
                            if (beta <= alpha)
                            {
                                return minEval;
                            }
                        }
                    }
                }
            }
        }
        return minEval;
    }
}


int IterativeDeepening(int maxDepth, int alpha, int beta, bool isMaximizingPlayer, double timeLimitInSeconds)
{
    clock_t begin = clock();
    double time_spent;
    int bestEval = 0;
    int depth = 1;
    for (; depth <= maxDepth; depth++)
    {
        bestEval = AlphaBeta(depth, alpha, beta, isMaximizingPlayer);

        clock_t end = clock();
        time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        if (time_spent >= timeLimitInSeconds)
        {
            break;
        }
    }
    return bestEval;
}

void UndoMove(MoveState state)
{

    pieces[state.pieceIndex].pos = state.originalPos;
    if (state.promoteTo != -1)
    {
        pieces[state.pieceIndex].type = state.promoteTo;
    }

    if (state.capturedIndex != -1)
    {
        pieces[state.capturedIndex].canDraw = true;
        pieces[state.capturedIndex].pos = state.capturedOriginalPos;
    }

    if (state.castlingPerformed && state.rookIndex != -1)
    {
        pieces[state.rookIndex].pos = state.rookOriginalPos;
    }

    if (state.enPassantCapturedIndex != -1)
    {
        pieces[state.enPassantCapturedIndex].canDraw = true;
    }
}
MoveState ApplyMove(int pieceIndex, Vector2 newPos)
{
    MoveState state = {
        .originalPos = pieces[pieceIndex].pos,
        .capturedIndex = -1,
        .capturedOriginalPos = {-1, -1},
        .rookIndex = -1,
        .rookOriginalPos = {-1, -1},
        .castlingPerformed = false,
        .enPassantCapturedIndex = -1,
        .promoteTo = -1};

    state.pieceIndex = pieceIndex;
    for (int i = 0; i < pieceCount; i++)
    {
        if (i != pieceIndex && pieces[i].canDraw && IsVector2Equal(pieces[i].pos, newPos))
        {
            state.capturedIndex = i;
            state.capturedOriginalPos = pieces[i].pos;
            pieces[i].canDraw = false;
            break;
        }
    }

    if (pieces[pieceIndex].type == 'P' && abs(newPos.x - pieces[pieceIndex].pos.x) == col_width &&
        !IsSquareOccupied(newPos))
    {
        Vector2 enPassantPos = {newPos.x, pieces[pieceIndex].pos.y};
        for (int i = 0; i < pieceCount; i++)
        {
            if (pieces[i].canDraw && IsVector2Equal(pieces[i].pos, enPassantPos) &&
                pieces[i].type == 'P' && pieces[i].isWhite != pieces[pieceIndex].isWhite)
            {
                state.enPassantCapturedIndex = i;
                pieces[i].canDraw = false;
                break;
            }
        }
    }

    if (pieces[pieceIndex].type == 'K' && abs(newPos.x - pieces[pieceIndex].pos.x) == 2 * col_width)
    {
        state.castlingPerformed = true;
        int direction = (newPos.x > pieces[pieceIndex].pos.x) ? 1 : -1;
        Vector2 rookStartPos = {(direction == 1 ? 7 : 0) * col_width, pieces[pieceIndex].pos.y};
        Vector2 rookNewPos = {newPos.x - direction * col_width, pieces[pieceIndex].pos.y};

        for (int i = 0; i < pieceCount; i++)
        {
            if (pieces[i].type == 'R' && IsVector2Equal(pieces[i].pos, rookStartPos) && !pieces[i].hasMoved)
            {
                state.rookIndex = i;
                state.rookOriginalPos = pieces[i].pos;
                pieces[i].pos = rookNewPos;
                break;
            }
        }
    }
    pieces[pieceIndex].pos = newPos;
    return state;
}
