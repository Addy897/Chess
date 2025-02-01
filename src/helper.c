#include <main.h>


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
            Piece p;
            p.type = c >= 'a' ? 'A' + (c - 'a') : c;
            p.pos = (Vector2){col * col_width, row * col_height};
            p.isDragging = false;
            p.isWhite = c < 96;
            p.canDraw = true;
            p.moveIndex = 0;

            p.hasMoved = false;
            setPiece(pieceCount,p);
            pieceCount++;
            col++;
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
            Piece * p = getPiece(i);
            Rectangle pieceRect = {p->pos.x, p->pos.y, col_width, col_height};

            if (CheckCollisionPointRec(mousePos, pieceRect) && p->canDraw)
            {
                if (p->isWhite != gameState.isWhiteTurn)
                {
                    break;
                }
                p->isDragging = true;
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
void PlayMove(bool withWhite)
{
    double score = INT_MAX;
   
    clock_t begin =clock();
    
    for (int i = 0; i < pieceCount; i++)
    {
         Piece * p = getPiece(i);
        if (p->canDraw&&p->isWhite == withWhite)
        {
          
            
            
            for (int j = 0; j < p->moveIndex; j++)
            {
                 
                Vector2 newPos = p->moves[j].endPos;

                if(IsValidMove(i,newPos)){
              
                    MoveState state = ApplyMove(i, newPos);

                    double eval = AlphaBeta(3, INT_MIN, INT_MAX, !withWhite);

                    UndoMove(state);
                    if (eval < score)
                    {
                        gameState.computerMove = p->moves[j];
                        gameState.computerPieceIndex = i;
                        score = eval;
                    }
                }else{
                    printf("Invalid: %c --> %d %d\n",p->type,newPos.x,newPos.y);
                }
                
            }
        }
    }
    clock_t end=clock();
    double timeSpent=(end-begin)/CLOCKS_PER_SEC;
    lastSelectedPiece = gameState.computerPieceIndex;
    char not[3];
    squareToNotation((gameState.computerMove.endPos.y/col_height)*8+gameState.computerMove.endPos.x/col_width,not);
    char move[5]={pieces[lastSelectedPiece].type+32};
    strcat(move,not);
    //printf("TOOK: %f Score: %f Move: %s\n",timeSpent,score,move);
    PlayMoveOnBoard(gameState.computerMove.endPos);
}




Vector2 FindKingPosition(bool isWhite)
{
    for (int i = 0; i < pieceCount; i++)
    {
         Piece * p = getPiece(i);
        if (p->type == 'K' && p->isWhite == isWhite && p->canDraw)
        {
            return p->pos;
        }
    }
    return (Vector2){-1, -1};
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
         Piece * p = getPiece(i);
        if (i != pieceIndex && p->canDraw && IsVector2Equal(p->pos, newPos))
        {
            state.capturedIndex = i;
            state.capturedOriginalPos = p->pos;
            p->canDraw = false;
            break;
        }
    }

    if (pieces[pieceIndex].type == 'P' && abs(newPos.x - pieces[pieceIndex].pos.x) == col_width &&
        !IsSquareOccupied(newPos))
    {
        Vector2 enPassantPos = {newPos.x, pieces[pieceIndex].pos.y};
        for (int i = 0; i < pieceCount; i++)
        {
             Piece * p = getPiece(i);
            if (p->canDraw && IsVector2Equal(p->pos, enPassantPos) &&
                p->type == 'P' && p->isWhite != pieces[pieceIndex].isWhite)
            {
                state.enPassantCapturedIndex = i;
                p->canDraw = false;
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
             Piece * p = getPiece(i);
            if (p->type == 'R' && IsVector2Equal(p->pos, rookStartPos) && !p->hasMoved)
            {
                state.rookIndex = i;
                state.rookOriginalPos = p->pos;
                p->pos = rookNewPos;
                break;
            }
        }
    }
    pieces[pieceIndex].pos = newPos;
    return state;
}
