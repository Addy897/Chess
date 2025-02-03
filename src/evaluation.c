#include "main.h"

bool IsSquareOccupied(Vector2 pos)
{
    for (int i = 0; i < pieceCount; i++)
    {
        Piece * p = getPiece(i);

        if (p->canDraw && IsVector2Equal(p->pos, pos))
        {
            return true;
        }
    }
    return false;
}

bool IsIsolatedPawn(int pawnIndex)
{
    Piece * pawn = getPiece(pawnIndex);
    int file = pawn->pos.x / col_width;

    bool hasLeftPawn = false, hasRightPawn = false;
    for (int i = 0; i < pieceCount; i++)
    {
        Piece * p = getPiece(i);

        if (p->type == 'P' && p->isWhite == pawn->isWhite && p->canDraw)
        {
            int otherFile = p->pos.x / col_width;
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
            Piece * p = getPiece(i);

        if (p->type == 'P' && p->isWhite == isWhite && p->canDraw)
        {
            int file = p->pos.x / col_width;
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
    Piece *pawn = getPiece(pawnIndex);
    Vector2 squareInFront = {pawn->pos.x, pawn->pos.y + (pawn->isWhite ? -col_height : col_height)};
    return IsSquareOccupied(squareInFront);
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
        Piece * p=getPiece(i);
        if (p->canDraw) {
            total_pieces++;
            if (p->type == 'Q') has_queen = true;
        }
    }
    
    return (total_pieces <= 6 && !has_queen) || (total_pieces <= 3);
}

float EndgamePhaseWeight (int materialCountWithoutPawns) {
			float val=materialCountWithoutPawns/1650;
			return 1-fmin(1,val);
}
int countMaterial(int Q,int B,int R,int N){
    return  900 * (Q ) + 500 * (R) + 300 *N +320*B; 
}
int computeManhattanDistance(Vector2 a, Vector2 b) { 
    

    int y = abs(a.y - b.y)/col_height;
    int x = abs(a.x - b.x)/col_width;
    
    return x+y;
}
int kingcomputeManhattanDistance(int x,int y) { 
    
    int centerX = 4;
    int centerY = 4;
    int fileDstFromCentre = fmax(3 - x, x - 4);
    int rankDstFromCentre = fmax (3 - y, y - 4);
    return fileDstFromCentre+rankDstFromCentre;
}

float MopUpEval(Vector2 friendlyKing, Vector2 opponentKing, int myMaterial, int opponentMaterial, float endgameWeight) {
    float mopUpScore = 0;

    if (myMaterial > opponentMaterial + 200&& endgameWeight > 0) {

        

        mopUpScore += kingcomputeManhattanDistance((int)opponentKing.x/col_width,(int)opponentKing.y/col_height) * 10;
        mopUpScore += (14 - computeManhattanDistance(friendlyKing, opponentKing)) * 4;

    

        return (mopUpScore * endgameWeight);
    }

   
    return 0;
}

double Eval()
{
      

    int K = 0, K_ = 0, Q = 0, Q_ = 0, P = 0, P_ = 0, B = 0, B_ = 0, N = 0, N_ = 0, R = 0, R_ = 0;
    int CheckScore = 0;
    int D = 0, D_ = 0, S = 0, S_ = 0, I = 0, I_ = 0;
    int materialCountBlack=0;
    int materialCountWhite=0;
    float cornerBonus=0;
    Vector2 blackKing;
    Vector2 whiteKing;
    for (int i = 0; i < pieceCount; i++)
    {
        Piece * p = getPiece(i);

        if (!p->canDraw)
            continue;
        
        if (p->isWhite)
        {
            K += p->type == 'K';
            Q += p->type == 'Q';
            P += p->type == 'P';
            R += p->type == 'R';
            N += p->type == 'N';
            B += p->type == 'B';
        }
        else
        {
            K_ += p->type == 'K';
            Q_ += p->type == 'Q';
            P_ += p->type == 'P';
            R_ += p->type == 'R';
            N_ += p->type == 'N';
            B_ += p->type == 'B';
        }
        if(p->type=='K'){
           if(p->isWhite){
            whiteKing=p->pos;

           }else{
            blackKing=p->pos;
           }
        }
        // if (p->type == 'P')
        // {
        //     if (p->isWhite)
        //     {
        //         D += HasDoubledPawns(true);
        //         I += IsIsolatedPawn(i);
        //         S += IsBlockedPawn(i);
        //     }
        //     else
        //     {
        //         D_ += HasDoubledPawns(false);
        //         I_ += IsIsolatedPawn(i);
        //         S_ += IsBlockedPawn(i);
        //     }
        // }
    }
    float wEval=0;
    float bEval=0;
    int whiteMaterial = countMaterial (Q,B,R,N);
    int blackMaterial = countMaterial (Q_,B_,R_,N_);
    wEval=whiteMaterial+P*100;
    bEval=blackMaterial+P_*100;
    float whiteEndgameWeight =EndgamePhaseWeight(whiteMaterial);
    float blackEndgameWeight =EndgamePhaseWeight(blackMaterial);
    wEval+= MopUpEval(whiteKing,blackKing,whiteMaterial,blackMaterial,whiteEndgameWeight);
    bEval+= MopUpEval(blackKing,whiteKing,blackMaterial,whiteMaterial,blackEndgameWeight);
    wEval += EvaluatePieceSquareTables (true,whiteEndgameWeight ,whiteKing);
    bEval += EvaluatePieceSquareTables (false, blackEndgameWeight,blackKing);
    if (IsCheckMate(true))
    {
        return INT_MIN;
    }
    else if (IsCheckMate(false))
    {
       return INT_MAX;
    }
    


    double eval =  wEval-bEval;
    return eval;
}