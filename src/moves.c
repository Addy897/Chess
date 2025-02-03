#include <main.h>

unsigned int setMove(short fromSquare, short toSquare, char flag,short pieceIndex,short capturedPieceIndex)
{
   
    unsigned int moves = capturedPieceIndex<<22|pieceIndex<<16|flag << 12 | toSquare << 6 | fromSquare;
    return moves;
}

void GenerateAllLegalMovesForPiece(int pieceIndex,int Moves[64], int* n)
{
    Piece *piece = getPiece(pieceIndex);
    piece->moveIndex = 0;

    int startX = (int)(piece->pos.x / col_width);
    int startY = (int)(piece->pos.y / col_height);
    int rank=startY;
    rank=7-rank;
   
    short squareIndex = (startX + rank * 8);
    Squares sqr;

    switch (piece->type)
    {
    case 'P':
    {

        if (piece->isWhite)
        {
            sqr = whitePawnSquares[squareIndex];
        }
        else
        {
            sqr = blackPawnSquares[squareIndex];
        }

        break;
    }

    case 'R':
    {

        sqr = rookSquares[squareIndex];
        break;
    }

    case 'N':
    {
        sqr = knightSquares[squareIndex];
        break;
    }

    case 'B':
    {
        sqr = bishopSquares[squareIndex];
        break;
    }

    case 'Q':
    {

        sqr = queenSquares[squareIndex];

        break;
    }

    case 'K':
    {

        sqr = kingSquares[squareIndex];

        if (!piece->hasMoved)
        {

            if (IsValidMove(pieceIndex, (Vector2){(startX + 2) * col_width, startY * col_height}))
            {
                //AddMove(pieceIndex, (Vector2){(startX + 2) * col_width, startY * col_height});
                Moves[(*n)++]=setMove(squareIndex,squareIndex+2,FLAG_CASTLING,pieceIndex,-1);
            }

            if (IsValidMove(pieceIndex, (Vector2){(startX - 2) * col_width, startY * col_height}))
            {
                //AddMove(pieceIndex, (Vector2){(startX - 2) * col_width, startY * col_height});
                Moves[(*n)++]=setMove(squareIndex,squareIndex-2,FLAG_CASTLING,pieceIndex,-1);
            }
        }
        break;
    }

    default:
        break;
    }
    for (int i = 0; i < sqr.n; i++)
    {
        char sq = sqr.to[i];
        Vector2 newPos = squareToVector2(sq);
        if (IsValidMove(pieceIndex, newPos))
        {
            //AddMove(pieceIndex, newPos);
            
            Moves[(*n)++]=setMove(squareIndex,sq,FLAG_NONE,pieceIndex,-1);
        }
    }
    
}
int GenerateAllLegalMoves(bool forWhite,int Moves[64])
{
    int n=0;
    
    for (int i = 0; i < pieceCount; i++)
    {
        Piece *p = getPiece(i);
        if (p->isWhite == forWhite && p->canDraw)
        {
            GenerateAllLegalMovesForPiece(i,Moves,&n);
            // Piece * p=&pieces[i];
            // SortMovesByPriority(p);
           
        }
    }
    return n;
}
int GetMovePriority(Move *move)
{
    if (move->capturePieceType)
    {
        int capt = GetPieceValue(move->capturePieceType);
        int val = GetPieceValue(move->pieceType);
        return (capt - val) * 100;
    }
    if (move->isCheck)
        return 50;

    return 0;
}
void UpdateLastMove(int pieceIndex, Vector2 newPos)
{
    Piece *P = getPiece(pieceIndex);
    lastMove.startPos = P->pos;
    lastMove.endPos = newPos;
    lastMove.pieceType = P->type;
    lastMove.pieceisWhite = P->isWhite;
}
void HandleEnPassant(int pieceIndex, Vector2 newPos)
{
    Piece *piece = getPiece(pieceIndex);
    if ((piece->type == 'P') &&
        abs((newPos.x - piece->pos.x) / col_width) == 1 &&
        abs((newPos.y - piece->pos.y) / col_height) == 1)
    {
        Vector2 capturedPos = {newPos.x, lastMove.endPos.y};
        for (int i = 0; i < pieceCount; i++)
        {
            Piece *P = getPiece(i);

            if ((P->type == 'P') && IsVector2Equal(P->pos, capturedPos) &&
                P->isWhite != piece->isWhite)
            {
                P->canDraw = false;
                break;
            }
        }
    }
}
void AddMove(int pieceIndex, Vector2 endPos)
{
    Piece *piece = getPiece(pieceIndex);
    Move move;
    move.startPos = piece->pos;
    move.endPos = endPos;
    move.pieceisWhite = piece->isWhite;
    move.pieceType = piece->type;
    move.capturePieceType = IsCapture(endPos, pieceIndex);
    Vector2 kinPos = FindKingPosition(!move.pieceisWhite);
    endPos = piece->pos;
    piece->pos = move.endPos;
    move.isCheck = IsValidMoveWithoutCheck(pieceIndex, kinPos);
    piece->pos = endPos;
    piece->moves[piece->moveIndex++] = move;
}
int CompareMoves(const void *a, const void *b)
{
    Move *moveA = (Move *)a;
    Move *moveB = (Move *)b;
    return GetMovePriority(moveB) - GetMovePriority(moveA);
}
void SortMovesByPriority(Piece *piece)
{
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
