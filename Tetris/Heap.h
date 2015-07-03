#pragma once

#include <windows.h>
#include "Pieces.h"

extern HINSTANCE hInst;

//Width of a single block, a Piece is made of four
extern unsigned int BlockWidth;
//Height of a single block, a Piece is made of four
extern unsigned int BlockHeight;

extern UINT TM_GAMEOVER;

class Heap
	{
	private:
		HBITMAP bmHeap;
		HRGN area;
		HRGN top;
		HWND hWnd;
		BLENDFUNCTION fnBlend;
		
		unsigned int HeapWidth;
		unsigned int HeapHeight;
		int x;
		int y;

	public:
		Heap(HWND hWND,int x=0,int y=0);

		~Heap();

		//Represent the kinds of movement of a piece
		enum Movement{MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT};
		
		//Respond if the piece will crash with the heap after
		//a movement of the type specified with M
		bool WillTouch(Piece* P, Movement M); 
		//Put a new piece in the heap
		void AddPiece(HDC hdc, Piece* P);

		void SearchFillRow(HDC hdc);

		void EraseRow(HDC hdc, HRGN rgnRow,int pos);
		//re-draw all the heap
		void HeapPrint(HDC hdc);
		//Re-draw only a part of the heap
		void UpdateRect(HDC hdc,int Left,int Top,int Right,int Bottom);

	};