
#ifndef MAIN_H
    #define MAIN_H
        #include <raylib.h>
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <time.h>
        #include <math.h>
        #include "preComputeMoves.h"
        #include "types.h"
        #define COLS 8
        #define ROWS 8
        #define SIDE_WIDTH 400
        #define FLOAT_MAX 99999.0
        #define FLOAT_MIN -FLOAT_MAX
        extern Piece pieces[64];
        extern const int col_width;
        extern const int col_height;
        extern const int screenHeight;
        extern const int screenWidth;
        extern Sound captureSound;
        extern Sound moveSound;
        extern Sound checkSound;

        extern PromotionChoice pendingPromotion;
        extern Move lastMove;
        extern GameState gameState;
        extern int pieceCount;
        extern int lastSelectedPiece;
        extern int promotingPieceIndex;
        extern int currentMovesIndex;
        extern int BoardMoves[64];

        static inline Piece * getPiece(int index) {
            return &pieces[index];
        }

        static inline void setPiece(int index, Piece p) {
            pieces[index] = p;
        }
        extern void AddMove(int pieceIndex,Vector2 endPos);
        extern void GenerateAllLegalMovesForPiece(int pieceIndex,int Moves[64],int*);
        extern int GenerateAllLegalMoves(bool forWhite,int Moves[64]);
        extern void DrawBoard();
        extern void DrawPieces(Texture2D piecesTexture);
        extern void SetupPiecesFromFEN(const char *fen);
        extern void HighlightLegalMoves(int pieceIndex);
        extern void UpdateLastMove(int pieceIndex, Vector2 newPos);
        extern void HandleEnPassant(int pieceIndex, Vector2 newPos);
        extern void DrawPromotionPopup(Texture2D piecesTexture);
        extern void CheckForInput();
        extern void PlayMove(bool withWhite);
        extern void PlayMoveOnBoard(Vector2 new_pos);
        extern void HighlightPreviousMove();
        extern void SortMovesByPriority(Piece *piece);
        extern bool IsValidMoveWithoutCheck(int pieceIndex, Vector2 newPos);
        extern bool IsKingInCheck(bool isWhite);
        extern bool IsSquareUnderAttack(Vector2 square, bool isWhite);
        extern bool IsCheckMate(bool isWhite);
        extern bool IsStaleMate(bool isWhite);
        extern bool IsPromotionSquare(Vector2 pos, bool isWhite);
        extern bool IsVector2Equal(Vector2 a, Vector2 b);
        extern bool IsValidMove(int pieceIndex, Vector2 newPos);
        extern bool IsSquareUnderAttack(Vector2 square, bool isWhite);
        extern bool IsLegalMove(int pieceIndex, Vector2 newPos);
        extern bool IsSquareOccupied(Vector2 pos);
        extern bool IsEdgeSquare(Vector2 pos);


        extern char getMovePieceIndex(unsigned int move);
        extern char getMoveFrom(int move);
        extern char getMoveTo(int move);
        extern int GetPieceValue(char type);
        extern int IsCapture(Vector2 square, bool isWhite);
        extern int CalulatePossiblePosition(int depth,bool isWhiteTurn);
        extern double AlphaBeta(int depth, double alpha, double beta, bool isMaximizingPlayer);
        extern double Eval();   
        extern double IterativeDeepening(int maxDepth, int alpha, int beta, bool isMaximizingPlayer, double timeLimitInSeconds);
        extern int GetMovePriority(Move *move);
        extern Vector2 FindKingPosition(bool isWhite);
        extern Vector2 moveToVector2(int move);
        extern Vector2 squareToVector2(char to);
        extern void UndoMove(MoveState state);
        extern void ApplyPromotion(MoveState *state);
        extern MoveState ApplyMove(int pieceIndex, Vector2 newPos);
        extern float EndgamePhaseWeight (int materialCountWithoutPawns);
        extern float MopUpEval(Vector2 friendlyKing, Vector2 opponentKing, int myMaterial, int opponentMaterial, float endgameWeight);
        extern float EvaluatePieceSquareTables (bool isWhite, float endgamePhaseWeight,Vector2 king);
#endif