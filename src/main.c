#include <main.h>
const int screenHeight = 800;
const int screenWidth = 800;
const int col_width = screenWidth / COLS;
const int col_height = screenHeight / ROWS;
Piece pieces[ROWS * COLS] = {0};
int BoardMoves[64]={0};
PromotionChoice pendingPromotion = PROMOTION_NONE;
Move lastMove = {0};
GameState gameState = {0};
int pieceCount = 0;
int lastSelectedPiece = -1;
int promotingPieceIndex = -1;
int currentMovesIndex=-1;
Sound moveSound;
Sound captureSound;
Sound checkSound;

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
    gameState.state=0;
    
    currentMovesIndex=GenerateAllLegalMoves(gameState.isWhiteTurn,BoardMoves);
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
            if (lastMove.pieceisWhite){
                strncpy(winner, "White",5);
                gameState.state=1<<3;
            }
            else{
                strncpy(winner, "Black",5);
                gameState.state=1<<2;
            }

            char result[100];
            sprintf(result, "GAME OVER\n%s Won by Checkmate", winner);
            DrawText(result, screenWidth + (SIDE_WIDTH / 8), SIDE_WIDTH, 24, WHITE);
            
        }
        else if (IsStaleMate(!lastMove.pieceisWhite))
        {
            DrawText("Draw", screenWidth + (SIDE_WIDTH / 8), SIDE_WIDTH, 24, WHITE);
            gameState.state=1<<1;
        }

        HighlightPreviousMove();

        DrawPieces(piecesTexture);
        if (promotingPieceIndex != -1)
        {
            DrawPromotionPopup(piecesTexture);
        }
        EndDrawing();
        if (gameState.playingWithComputer && (gameState.state==0))
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
