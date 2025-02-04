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

            if (IsValidMove(pieceIndex, (Vector2){(startX + 2) * col_width, startY * col_height},(int*)-1))
            {
                //AddMove(pieceIndex, (Vector2){(startX + 2) * col_width, startY * col_height});
                Moves[(*n)++]=setMove(squareIndex,squareIndex+2,FLAG_CASTLING,pieceIndex,-1);
            }

            if (IsValidMove(pieceIndex, (Vector2){(startX - 2) * col_width, startY * col_height},(int *)-1))
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
        int capturedIndex=-1;
        if (IsValidMove(pieceIndex, newPos,&capturedIndex))
        {
            //AddMove(pieceIndex, newPos);
           if(IsCheckMate(!piece->isWhite)){
             Moves[(*n)++]=setMove(squareIndex,sq,FLAG_CHECKMATE,pieceIndex,capturedIndex);
           }else if(IsPromotionSquare(newPos,piece->isWhite)){
                Moves[(*n)++]=setMove(squareIndex,sq,FLAG_PROMOTION,pieceIndex,capturedIndex);
           }
           else{
            Moves[(*n)++]=setMove(squareIndex,sq,capturedIndex!=-1,pieceIndex,capturedIndex);
           }
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
        }
    }
    SortMovesByPriority(Moves,n);
    return n;
}
int GetMovePriority(int move)
{
    char Flag =getFlagFromMove(move);
    if (Flag==FLAG_CHECKMATE)
    {
       
        return (3);
    }
    if (Flag==FLAG_CAPTURE)
    {
       
        return (2);
    }
    if(Flag == FLAG_PROMOTION){
        return 1;
    }
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

int CompareMoves(const void *a, const void *b)
{
    int moveA = *(int *)a;
    int  moveB = *(int *)b;
    return GetMovePriority(moveB) - GetMovePriority(moveA);
}
void SortMovesByPriority(int Moves[64],int n)
{
    qsort(Moves, n, sizeof(int), CompareMoves);
}
int GetPieceValue(char type){
    switch (type)
    {
    case 'P':
        return 1;
    case 'K':
        return 20;
    case 'Q':
        return 9;
    case 'B':
        return 3;
    case 'R':
        return 5;
    case 'N':
        return 3;
    default:
        break;
    }
}
