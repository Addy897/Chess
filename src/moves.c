#include <main.h>
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
    Piece * piece = getPiece(pieceIndex);
    piece->moveIndex = 0;

    int startX = (int)(piece->pos.x / col_width);
    int startY = (int)(piece->pos.y / col_height);
    int squareIndex=63-(startX+startY*8);
    Squares sqr=knightSquares[squareIndex];
        
    switch (piece->type)
    {
    case 'P':
    {
        
        if(piece->isWhite){
            sqr=whitePawnSquares[squareIndex];
        }else{
            sqr=blackPawnSquares[squareIndex];
        }
       
        break;
    }

    case 'R':
    {
       
       sqr=rookSquares[squareIndex];
       // GenerateRookMoves(pieceIndex,startX,startY);      
        break;
    }

    case 'N':
    {
        sqr=knightSquares[squareIndex];
        break;
    }

    case 'B':
    {
        sqr=bishopSquares[squareIndex];
       // GenerateBishopMoves(pieceIndex,startX,startY);
        break;
        
    }

    case 'Q':
    {

       sqr=queenSquares[squareIndex];
        
        break;
    }

    case 'K':
    {

        sqr=kingSquares[squareIndex];

        if (!piece->hasMoved)
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
    for (int i = 0; i < sqr.n; i++)
        {
            char sq = 63-sqr.to[i];
            Vector2 newPos=(Vector2){.x=(sq % 8)*col_width,.y=(sq/8)*col_height};
           
            if (IsValidMove(pieceIndex, newPos))
            {
                AddMove(pieceIndex, newPos);
            }
            
        }
    
}
void GenerateAllLegalMoves(bool forWhite)
{
    currentMovesIndex = -1;
    for (int i = 0; i < pieceCount; i++)
    {
        Piece * p=getPiece(i);
        if (p->isWhite == forWhite && p->canDraw)
        {
            GenerateAllLegalMovesForPiece(i);
            // Piece * p=&pieces[i];
            // SortMovesByPriority(p);
            currentMovesIndex += p->moveIndex;
        }
    }
    
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
void UpdateLastMove(int pieceIndex, Vector2 newPos)
{
    Piece * P=getPiece(pieceIndex);
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
            Piece *P=getPiece(i);

            if ((P->type == 'P')&&IsVector2Equal(P->pos,capturedPos)&&
                P->isWhite != piece->isWhite 
                )
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
