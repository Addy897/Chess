#include "types.h"
#ifndef PRECOMPUTE_MOVES_H
#define PRECOMPUTE_MOVES_H

extern Squares queenSquares[64]; 
extern Squares kingSquares[64]; 
extern Squares knightSquares[64];
extern Squares bishopSquares[64];
extern Squares rookSquares[64];
extern Squares whitePawnSquares[64]; 
extern Squares blackPawnSquares[64]; 

extern void precomputePawnMoves();
extern void precomputeQueenMoves();
extern void precomputeKingMoves();
 
extern void precomputeKnightMoves();
 
extern void precomputeBishopMoves();
 
extern void precomputeRookMoves();
extern char notationToSquare(char notation[3]);
extern void squareToNotation(char square,char notation[3]);
extern void preComputeAll();
extern void printMoves(Squares squares, char fromSquare, char * piece);
#endif