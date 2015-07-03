#include <list>
#include "Pieces.h"

void ClearScreen(HWND hWnd, HDC hdc);

void InsertRandomPieces(HWND hWnd, std::list<Piece*>& lsPieces);

void SetupNextPiece(HWND hWnd, Piece*& CurrentPiece, std::list<Piece*>& lsPieces);