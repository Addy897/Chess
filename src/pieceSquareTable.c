#include "main.h"


int pawns[] = {
    100,  100,  100,  100,  100,  100,  100,  100,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};
int kingEnd[] = {
    -50,-40,-30,-20,-20,-30,-40,-50,
    -30,-20,-10,  0,  0,-10,-20,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 30, 40, 40, 30,-10,-30,
    -30,-10, 20, 30, 30, 20,-10,-30,
    -30,-30,  0,  0,  0,  0,-30,-30,
    -50,-30,-30,-30,-30,-30,-30,-50
};
int knights[] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50,
};
int bishops[] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20,
    };
int rooks[] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};
int queens[] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};
int kingMiddle[] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};
float EvaluatePieceSquareTable (char type,bool isWhite) {
			float value = 0;
            int * table;
            
            switch (type)
            {
            case 'P':
                table=&pawns[0];
                break;
            case 'Q':
                table=&queens[0];
                break;
            case 'B':
                table=&bishops[0];
                break;
            case 'N':
                table=&knights[0];
                break;
                
            case 'R':
                table=&rooks[0];
                break;

            
            default:
                break;
            }
			for (int i = 0; i < pieceCount; i++) {
                Piece* p=getPiece(i);
                if(p->canDraw&& p->type==type && p->isWhite==isWhite){
                    int rank=(p->pos.y/col_height);
                    int file=(p->pos.x/col_width);
                if(!isWhite){
                    rank=7-rank;
                }
				value += table[rank*8+file];
                }
			}
			return value;
}
float EvaluatePieceSquareTables (bool isWhite, float endgamePhaseWeight,Vector2 king) {
    float value = 0;
    value += EvaluatePieceSquareTable ('P', isWhite);
    value += EvaluatePieceSquareTable ('R', isWhite);
    value += EvaluatePieceSquareTable ('N', isWhite);
    value += EvaluatePieceSquareTable ('B', isWhite);
    value += EvaluatePieceSquareTable ('Q', isWhite);
    int rank=(king.y/col_height);
    int file=(king.x/col_width);
    if(!isWhite){
        rank=7-rank;
    }

    int kingEarlyPhase = kingMiddle[rank*8+file];
    value +=  (kingEarlyPhase * (1 - endgamePhaseWeight));

    return value;
}		