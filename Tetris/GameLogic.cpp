#include "stdafx.h"

#include "GameLogic.h"

void ClearScreen(HWND hWnd, HDC hdc)
	{
	RECT rect;
	GetClientRect(hWnd,&rect);
	WNDCLASS WndClass;
	GetClassInfo(hInst,_T("TETRIS"),&WndClass);
	::FillRect(hdc,&rect,WndClass.hbrBackground);

	return;
	}

void InsertRandomPieces(HWND hWnd, std::list<Piece*>& lsPieces)
	{
	int random = rand()%4;

	Piece* novaPiece;
	for( int i=0;i<2;++i)
		{
		switch( random )
			{	
			case 0:
				{
				novaPiece = static_cast<Piece*>(new Piece_A(hWnd,0,0));
				lsPieces.push_back(novaPiece);
				}
			break;
			case 1:
				{
				novaPiece = static_cast<Piece*>(new Piece_A(hWnd,0,0));
				lsPieces.push_back(novaPiece);
				}
			break;
			case 2:
				{
				novaPiece = static_cast<Piece*>(new Piece_A(hWnd,0,0));
				lsPieces.push_back(novaPiece);
				}
			break;
			case 3:
				{
				novaPiece = static_cast<Piece*>(new Piece_A(hWnd,0,0));
				lsPieces.push_back(novaPiece);
				}
			break;
			}
		}
	return;
	}

void SetupNextPiece(HWND hWnd, Piece*& CurrentPiece, std::list<Piece*>& lsPieces)
	{
	if( lsPieces.size() == 0 )
		InsertRandomPieces(hWnd,lsPieces);

	CurrentPiece= lsPieces.front();
	lsPieces.pop_front();

	int novaposX = rand()%10;
	novaposX *= BlockWidth;

	HDC hdc = GetDC(hWnd);
	ClearScreen(hWnd,hdc);
	
	CurrentPiece->Draw(hdc,novaposX,-BlockHeight*2);
		
	::ReleaseDC(hWnd,hdc);

	return;
	}