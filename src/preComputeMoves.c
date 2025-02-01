#include <preComputeMoves.h>
#include <stdio.h>
Squares queenSquares[64] = {0}; 
Squares kingSquares[64] = {0}; 
Squares knightSquares[64] = {0};
Squares bishopSquares[64] = {0};
Squares rookSquares[64] = {0};
Squares whitePawnSquares[64] = {0}; 
Squares blackPawnSquares[64] = {0}; 

void precomputePawnMoves() {
    for (int square = 0; square < 64; square++) {
        int x = square % 8;  
        int y = square / 8;  

        int whiteIndex = 0;
        int blackIndex = 0;

        
        if (y < 7) {  
            whitePawnSquares[square].to[whiteIndex++] = (y + 1) * 8 + x;  
        }
        if (y == 1) {  
            whitePawnSquares[square].to[whiteIndex++] = (y + 2) * 8 + x;  
        }
        
        if (x > 0 && y < 7) {  
            whitePawnSquares[square].to[whiteIndex++] = (y + 1) * 8 + (x - 1);
        }
        if (x < 7 && y < 7) {  
            whitePawnSquares[square].to[whiteIndex++] = (y + 1) * 8 + (x + 1);
        }
        whitePawnSquares[square].n=whiteIndex;
        
        if (y > 0) {  
            blackPawnSquares[square].to[blackIndex++] = (y - 1) * 8 + x;  
        }
        if (y == 6) {  
            blackPawnSquares[square].to[blackIndex++] = (y - 2) * 8 + x;  
        }
        
        if (x > 0 && y > 0) {  
            blackPawnSquares[square].to[blackIndex++] = (y - 1) * 8 + (x - 1);
        }
        if (x < 7 && y > 0) {  
            blackPawnSquares[square].to[blackIndex++] = (y - 1) * 8 + (x + 1);
        }
        blackPawnSquares[square].n=blackIndex;
    }
}
void precomputeQueenMoves() {
    for (int square = 0; square < 64; square++) {
        int x = square % 8; 
        int y = square / 8; 
        int index = 0;

        
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue; 
                int nx = x + dx;
                int ny = y + dy;
                while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                    queenSquares[square].to[index++] = ny * 8 + nx;
                    nx += dx;
                    ny += dy;
                }
                
            }
        }
        queenSquares[square].n=index;
    }
}


void precomputeKingMoves() {
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int square = 0; square < 64; square++) {
        int x = square % 8;
        int y = square / 8;
        int index = 0;

        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                kingSquares[square].to[index++] = ny * 8 + nx;
            }
        }
        kingSquares[square].n=index;
    }
}


void precomputeKnightMoves() {
    int dx[] = {-2, -1, 1, 2, 2, 1, -1, -2};
    int dy[] = {1, 2, 2, 1, -1, -2, -2, -1};

    for (int square = 0; square < 64; square++) {
        int x = square % 8;
        int y = square / 8;
        int index = 0;

        for (int i = 0; i < 8; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                knightSquares[square].to[index++] = ny * 8 + nx;
            }
        }
        knightSquares[square].n=index;
    }
}


void precomputeBishopMoves() {
    for (int square = 0; square < 64; square++) {
        int x = square % 8; 
        int y = square / 8; 
        int index = 0;

        
        for (int dx = -1; dx <= 1; dx += 2) {
            for (int dy = -1; dy <= 1; dy += 2) {
                int nx = x + dx;
                int ny = y + dy;
                while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                    bishopSquares[square].to[index++] = ny * 8 + nx;
                    nx += dx;
                    ny += dy;
                }
            }
        }
        bishopSquares[square].n=index;
    }
}


void precomputeRookMoves() {
    for (int square = 0; square < 64; square++) {
        int x = square % 8; 
        int y = square / 8; 
        int index = 0;

        
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx != 0 && dy != 0) continue; 
                if (dx == 0 && dy == 0) continue; 
                int nx = x + dx;
                int ny = y + dy;
                while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                    rookSquares[square].to[index++] = ny * 8 + nx;
                    nx += dx;
                    ny += dy;
                }
            }
        }
        rookSquares[square].n=index;
    }
}

char notationToSquare(char notation[3]){
    char rank=notation[1]-'1';
    char file=notation[0]-'a';
    return rank*8+file;
}
void squareToNotation(char square,char notation[3]){
    notation[0]=('a' + (square % 8));
    notation[1]=(square / 8 + '1');
    notation[2]='\0';
}
void preComputeAll(){
    precomputeBishopMoves();
    precomputeQueenMoves();
    precomputeKingMoves();
    precomputeRookMoves();
    precomputePawnMoves();
    precomputeKnightMoves();
}

void printMoves(Squares squares, char fromSquare, char * piece) {
    char from[3];
    squareToNotation(fromSquare,from);
    printf("Possible %s moves from square %s:  ",piece,from);
    for (int i = 0; i < squares.n; i++) {
        
        
        squareToNotation(squares.to[i],from);
        printf("%s,",from);
        
        
    }
    printf("\n");
}
