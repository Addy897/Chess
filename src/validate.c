#include <main.h>


bool IsLegalMove(int pieceIndex, Vector2 newPos)
{
    int rank = 7-(int)(newPos.y/col_height);
    int file = (int)(newPos.x/col_width);
    char sqr=rank*8+file;
    for (int i = 0; i < currentMovesIndex; i++)
    {
        char sq=getMoveTo(BoardMoves[i]);
        char currentPieceIndex=getMovePieceIndex(BoardMoves[i]);
        if (currentPieceIndex==pieceIndex && sq==sqr)
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
         Piece * p = getPiece(i);
        if (i == skipIndex || !p->canDraw)
            continue;

        if (start.y == end.y && start.y == p->pos.y)
        {
            if ((start.x < p->pos.x && p->pos.x < end.x) ||
                (start.x > p->pos.x && p->pos.x > end.x))
            {
                return true;
            }
        }

        if (start.x == end.x && start.x == p->pos.x)
        {
            if ((start.y < p->pos.y && p->pos.y < end.y) ||
                (start.y > p->pos.y && p->pos.y > end.y))
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
        Piece * p = getPiece(i);
        if (i == skipIndex || !p->canDraw)
            continue;

        int new_dx = abs((end.x - p->pos.x) / col_width);
        int new_dy = abs((end.y - p->pos.y) / col_height);
        if (new_dx != new_dy)
            continue;

        if ((start.x < p->pos.x && p->pos.x < end.x) &&
            (start.y < p->pos.y && p->pos.y < end.y))
            return true;
        if ((start.x > p->pos.x && p->pos.x > end.x) &&
            (start.y > p->pos.y && p->pos.y > end.y))
            return true;
        if ((start.x > p->pos.x && p->pos.x > end.x) &&
            (start.y < p->pos.y && p->pos.y < end.y))
            return true;
        if ((start.x < p->pos.x && p->pos.x < end.x) &&
            (start.y > p->pos.y && p->pos.y > end.y))
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
                 Piece * p = getPiece(i);
                if (IsVector2Equal(p->pos, newPos) &&
                    p->canDraw &&
                    p->isWhite != isWhitePawn)
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
    Piece*piece = getPiece(pieceIndex);
    if (!piece->canDraw || !IsWithinBoard(newPos))
    {
        return false;
    }

    for (int i = 0; i < pieceCount; i++)
    {
         Piece * p = getPiece(i);
        if (i == pieceIndex || !p->canDraw)
            continue;
        if (IsVector2Equal(p->pos, newPos) && p->isWhite == piece->isWhite)
        {
            return false;
        }
    }

    int dx = (newPos.x - piece->pos.x) / col_width;
    int dy = (newPos.y - piece->pos.y) / col_height;
    int abs_dx = abs(dx);
    int abs_dy = abs(dy);

    switch (piece->type)
    {
    case 'P':
        return ValidatePawnMove(*piece, newPos, dx, dy);

    case 'R':
        return ((dx == 0 && dy != 0) || (dy == 0 && dx != 0)) &&
               !IsPieceBetween(piece->pos, newPos, pieceIndex);

    case 'N':
        return (abs_dx == 2 && abs_dy == 1) || (abs_dx == 1 && abs_dy == 2);

    case 'B':
        return abs_dx == abs_dy && !IsPieceBetweenDiagonal(piece->pos, newPos, pieceIndex);

    case 'Q':
        return ((abs_dx == abs_dy) || (dx == 0 && dy != 0) || (dy == 0 && dx != 0)) &&
               !IsPieceBetween(piece->pos, newPos, pieceIndex) &&
               !IsPieceBetweenDiagonal(piece->pos, newPos, pieceIndex);

    case 'K':
    {

        if (abs_dx <= 1 && abs_dy <= 1)
            return true;

        if (abs_dx == 2 && dy == 0 && !piece->hasMoved)
        {
            int direction = dx > 0 ? 1 : -1;
            Vector2 rookStartPos = {direction > 0 ? 7 * col_width : 0, piece->pos.y};

            bool rookFound = false;
            for (int i = 0; i < pieceCount; i++)
            {
                 Piece * p = getPiece(i);
                if (p->type == 'R' &&
                    IsVector2Equal(p->pos, rookStartPos) &&
                    !p->hasMoved)
                {
                    rookFound = true;
                    break;
                }
            }
            if (!rookFound)
                return false;

            if (IsPieceBetween(piece->pos, newPos, pieceIndex))
                return false;

            Vector2 intermediate = {piece->pos.x + direction * col_width, piece->pos.y};
            return !IsSquareUnderAttack(piece->pos, piece->isWhite) &&
                   !IsSquareUnderAttack(intermediate, piece->isWhite) &&
                   !IsSquareUnderAttack(newPos, piece->isWhite);
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
bool IsValidMove(int pieceIndex, Vector2 newPos,int* capturedIndex)
{
    if (pieceIndex < 0 || pieceIndex >= pieceCount)
        return false;
    Piece * originalPiece = getPiece(pieceIndex);
    Vector2 originalPos = originalPiece->pos;


    if (IsVector2Equal(originalPos, newPos))
        return false;
    if (!IsValidMoveWithoutCheck(pieceIndex, newPos))
        return false;


    MoveState Move = ApplyMove(pieceIndex, newPos);
    originalPiece->pos = newPos;
    if(capturedIndex!=(int *)-1){
        if(Move.capturedIndex!=-1){
            *capturedIndex=Move.capturedIndex;
        }else if(Move.enPassantCapturedIndex!=-1){
            *capturedIndex=Move.enPassantCapturedIndex;
        }
    }
    bool stillInCheck = IsKingInCheck(originalPiece->isWhite);
    UndoMove(Move);
    
    return !stillInCheck;
   
}
bool IsSquareUnderAttack(Vector2 square, bool isWhite)
{
    for (int i = 0; i < pieceCount; i++)
    {
         Piece * p = getPiece(i);
        if (!p->canDraw)
            continue;
        if (p->isWhite != isWhite)
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
         Piece * p = getPiece(i);
        if (!p->canDraw)
            continue;
        if (p->isWhite != isWhite)
        {
            if (IsVector2Equal(p->pos, square))
            {
                return p->type;
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
         Piece * p = getPiece(i);
        if (p->isWhite == isWhite && p->canDraw)
        {
            Squares possibleMoves;
            int rank=(p->pos.y/col_height)*8;
            int file=(p->pos.x/col_width);
            int index=63-(rank+file);
            switch (p->type) {
                case 'P':
                    possibleMoves = p->isWhite ? whitePawnSquares[index] : blackPawnSquares[index];
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
                    if (IsValidMove(i, newPos,(int*)-1))
                    {
                        return false;
                    }
                }
            }
        }
    }

    
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
         Piece * p = getPiece(i);
        if (p->canDraw)
        {
            totalPieceCount++;
        }
        if (totalPieceCount > 2)
            break;
    }
    if (totalPieceCount == 2)
    {
        return true;
    }

    for (int i = 0; i < pieceCount; i++)
    {
         Piece * p = getPiece(i);
        if (p->isWhite == isWhite && p->canDraw)
        {
            for (int x = 0; x < COLS; x++)
            {
                for (int y = 0; y < ROWS; y++)
                {
                    Vector2 newPos = {x * col_width, y * col_height};
                    if (IsValidMove(i, newPos,(int*)-1))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
