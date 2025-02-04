#include <main.h>
int CalulatePossiblePosition(int depth, bool isWhiteTurn)
{
    if (depth == 0)
    {
        return 1;
    }

    int totalMoves = 0;
    int Moves[64];
    int n = GenerateAllLegalMoves(isWhiteTurn, Moves);
    for (int i = 0; i < n; i++)
    {
        int move = Moves[i];
        char to = ((move >> 6) & (63));
        char pieceIndex = (move >> 16) & (63);
        Vector2 newPos = (Vector2){.x = (to % 8) * col_width, .y = (7 - (to / 8)) * col_height};
        MoveState moveState = ApplyMove(pieceIndex, newPos);
        totalMoves += CalulatePossiblePosition(depth - 1, !isWhiteTurn);
        UndoMove(moveState);
    }

    return totalMoves;
}
double AlphaBeta(int depth, double alpha, double beta, bool isMaximizingPlayer)
{
    if (depth == 0 || IsCheckMate(isMaximizingPlayer) || IsStaleMate(isMaximizingPlayer))
    {
        return Eval() +1/depth;
    }
    int Moves[64] = {0};
    int n = GenerateAllLegalMoves(isMaximizingPlayer, Moves);
    if (isMaximizingPlayer)
    {
        double maxEval = INT_MIN;
        for (int i = 0; i < n; i++)
        {
            int move = Moves[i];
            int pieceIndex = getMovePieceIndex(move);
            Piece *p = getPiece(pieceIndex);
            Vector2 newPos = moveToVector2(move);
            // if (IsValidMove(pieceIndex, newPos))
            // {
                MoveState state = ApplyMove(pieceIndex, newPos);

                if (p->type == 'P' && IsPromotionSquare(newPos, p->isWhite))
                {
                    char promotions[] = "QRNB";

                    state.promoteTo = 'P';
                    for (int j = 0; j < 4; j++)
                    {
                        p->type = promotions[j];
                        double eval = AlphaBeta(depth - 1, alpha, beta, false);
                        maxEval = (eval > maxEval) ? eval : maxEval;
                    }
                    p->type = 'P';
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
            // }
        }

        return maxEval;
    }
    else
    {
        double minEval = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            int move = Moves[i];
            int pieceIndex = getMovePieceIndex(move);
            Piece *p = getPiece(pieceIndex);
            Vector2 newPos = moveToVector2(move);
            // if (IsValidMove(pieceIndex, newPos))
            // {
                MoveState state = ApplyMove(pieceIndex, newPos);
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
                    p->type = 'P';
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
            // }
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
