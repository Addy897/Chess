#include <main.h>
int CalulatePossiblePosition(int depth, bool isWhiteTurn) {
    if (depth == 0) {
        return 1;  
    }

    int totalMoves = 0;

    
    for (int i = 0; i < pieceCount; i++) {
         Piece * p = getPiece(i);
        if (p->isWhite == isWhiteTurn && p->canDraw) {

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
                    // char from[3];
                    // char to[3];
                    // squareToNotation(index,from);
                    // squareToNotation(possibleMoves.to[j],to);
                    // printf("%c %c %s %s\n",p->isWhite?'W':'B',p->type,from,to)  ;

                    
                    if (IsValidMove(i, newPos)) {
                        
                        if (p->type == 'P' && IsPromotionSquare(newPos, p->isWhite)) {
                            char promotions[] = "QRNB";

                            
                            MoveState state = ApplyMove(i, newPos);
                            state.promoteTo = 'P';  

                            
                            for (int k = 0; k < 4; k++) {
                                p->type = promotions[k];  
                                
                                
                                totalMoves += CalulatePossiblePosition(depth - 1, !isWhiteTurn);
                            }

                            
                            p->type = 'P';
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
double AlphaBeta(int depth, double alpha, double beta, bool isMaximizingPlayer)
{
    if (depth == 0 || IsCheckMate(isMaximizingPlayer) || IsStaleMate(isMaximizingPlayer))
    {
        return Eval();
    }
         
   

   

    if (isMaximizingPlayer)
    {
        double maxEval = INT_MIN;
        for (int i = 0; i < pieceCount; i++)
        {
             Piece * p = getPiece(i);
            if (p->canDraw && p->isWhite)
            {
                for (int x = 0; x < COLS; x++)
                {
                    for (int y = 0; y < ROWS; y++)
                    {
                        Vector2 newPos = {x * col_width, y * col_height};

                        if (IsValidMove(i, newPos))
                        {
                            MoveState state = ApplyMove(i, newPos);

                            if (p->type == 'P' && IsPromotionSquare(newPos, p->isWhite))
                            {
                                char promotions[] = "QRNB";

                                state.promoteTo ='P';
                                for (int j = 0; j < 4; j++)
                                {
                                    p->type = promotions[j];
                                    double eval = AlphaBeta(depth - 1, alpha, beta, false);
                                    maxEval = (eval > maxEval) ? eval : maxEval;
                                    
                                }
                                p->type= 'P';

                                
                            }
                            else
                            {
                                double eval = AlphaBeta(depth - 1, alpha, beta, false);
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
        double minEval = INT_MAX;
        for (int i = 0; i < pieceCount; i++)
        {
             Piece * p = getPiece(i);
            if (p->canDraw && !p->isWhite)
            {
                for (int x = 0; x < COLS; x++)
                {
                    for (int y = 0; y < ROWS; y++)
                    {
                        Vector2 newPos = {x * col_width, y * col_height};
                        if (IsValidMove(i, newPos))
                        {
                            MoveState state = ApplyMove(i, newPos);

                            if (p->type == 'P' && IsPromotionSquare(newPos, p->isWhite))
                            {
                                char promotions[] = "QRNB";

                                state.promoteTo = 'P';
                                for (int j = 0; j < 4; j++)
                                {
                                    p->type = promotions[j];
                                    double eval = AlphaBeta(depth - 1, alpha, beta, true);
                                    minEval = (eval < minEval) ? eval : minEval;
                                    
                                }
                                p->type= 'P';

                                

                                
                            }
                            else
                            {

                                double eval = AlphaBeta(depth - 1, alpha, beta, true);
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


double IterativeDeepening(int maxDepth, int alpha, int beta, bool isMaximizingPlayer, double timeLimitInSeconds)
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
