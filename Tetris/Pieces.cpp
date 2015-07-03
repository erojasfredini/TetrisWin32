#include "stdafx.h"
#include "Pieces.h"

//This is included here to pass over a cyclic dependence
//					Pieces.h | -------------> | Heap.h
//						     | <------------- | 
//
//			So I include Heap in Pieces.cpp so the recursivity is eliminated 
#include "Heap.h"

//--------------------------------------	Class Piece		----------------------------------------------------------

Piece::Piece(HWND hWND,int nova_x,int nova_y)
	{
	hWnd = hWND;
	x = nova_x;
	y = nova_y;

	PieceState = 0;

	//Initialize the blendfunction
	fnBlend.AlphaFormat = AC_SRC_ALPHA;
	fnBlend.BlendFlags = 0;
	fnBlend.BlendOp = AC_SRC_OVER;
	fnBlend.SourceConstantAlpha = 255;
	}

Piece::~Piece()
	{
	//We dont erase sprite, because we could be erasing it in the child classes, 
	//and if we try to delete it twice we could have a problem
	}

void Piece::Draw(HDC hdc, int nova_x, int nova_y)
	{
	}
						
void Piece::MoveXYBlocks(HDC hdc, Heap* H, int offset_x, int offset_y)
	{
	}

void Piece::Turn(HDC hdc)
	{
	}

int Piece::GetX()
	{
	return x;
	}
int Piece::GetY()
	{
	return y;
	}


//--------------------------------------	Class Piece_A	---------------------------------------------------------

Piece_A::~Piece_A()
	{
	for(int i=0;i < 4; ++i)
		{
		assert( DeleteObject(spriteTurns[i]) != 0 );
		}
		
	for(int i=0;i < 4; ++i)
		assert( DeleteObject(areaTurns[i]) != 0);

	}

void Piece_A::Draw(HDC hdc, int nova_x, int nova_y)
	{
	//Re-position the piece
	for( int i=0;i < 4; ++i )
		OffsetRgn(areaTurns[i],nova_x-x,nova_y-y);
	
	x = nova_x;
	y = nova_y;

	::HDC dcMemory = CreateCompatibleDC(hdc);
	HBITMAP prevbm = (HBITMAP)SelectObject(dcMemory,sprite);

	assert( AlphaBlend(hdc,x,y,info.biWidth,info.biHeight,dcMemory,0,0,info.biWidth,info.biHeight,fnBlend) == true);
		//::FillRgn(hdc,area,(HBRUSH)GetStockObject(BLACK_BRUSH));
	
	//I already know that it return sprite
	SelectObject(dcMemory,prevbm);
	::DeleteDC(dcMemory);
	return;
	}

void Piece_A::MoveXYBlocks(HDC hdc,Heap* H, int offset_x, int offset_y)
	{
	//Let´s clean the prev show area
	WNDCLASS wndClass;
	GetClassInfo(hInst,_T("Tetris"),&wndClass);
	HRGN prevShow;
	prevShow = CreateRectRgn(x,y,x+info.biWidth,y+info.biHeight);
	FillRgn(hdc,prevShow,wndClass.hbrBackground);//The Brush already effsets the fill so it cleans the prev area
	assert( DeleteObject(prevShow) );
		//Re-draw the part of the heap
	H->UpdateRect(hdc,x,y,x+info.biWidth,y+info.biHeight);
	
	
	//Re-position the piece
	for( int i=0;i < 4; ++i )
		OffsetRgn(areaTurns[i],offset_x,offset_y);
	
	x += offset_x;
	y += offset_y;

	::HDC dcMemory = CreateCompatibleDC(hdc);
	HBITMAP prevbm =  (HBITMAP)SelectObject(dcMemory,sprite);
	
	assert( AlphaBlend(hdc,x,y,info.biWidth,info.biHeight,dcMemory,0,0,info.biWidth,info.biHeight,fnBlend) == true);
		//::FillRgn(hdc,area,(HBRUSH)GetStockObject(BLACK_BRUSH));
	
	//I already know that it return sprite
	SelectObject(dcMemory,prevbm);
	DeleteDC(dcMemory);
	return;
	}

void Piece_A::Turn(HDC hdc)
	{
	HDC dcMemory = CreateCompatibleDC(hdc);

	switch( PieceState )
		{
		case 0:
			{
			sprite = spriteTurns[1];
			info = infoTurns[1];
			area = areaTurns[1];
			OffsetRgn(area,0,-BlockHeight);
			y -= BlockHeight;
			PieceState = 90;
			}
			break;
		case 90:
			{
			sprite = spriteTurns[2];
			info = infoTurns[0];
			area = areaTurns[2];
			OffsetRgn(areaTurns[1],0,BlockHeight);
			y += BlockHeight;
			PieceState = 180;
			}
			break;
		case 180:
			{
			sprite = spriteTurns[3];
			info = infoTurns[1];
			area = areaTurns[3];
			OffsetRgn(area,0,-BlockHeight);
			y -= BlockHeight;
			PieceState = 240;
			}
			break;
		case 240:
			{
			sprite = spriteTurns[0];
			info = infoTurns[0];
			area = areaTurns[0];
			OffsetRgn(areaTurns[3],0,BlockHeight);
			y += BlockHeight;
			PieceState = 0;
			}
			break;
		}
	HBITMAP prevbm =  (HBITMAP)SelectObject(dcMemory,sprite);
	assert( AlphaBlend(hdc,x,y,info.biWidth,info.biHeight,dcMemory,0,0,info.biWidth,info.biHeight,fnBlend) == true);
	//I already know that it return sprite
	SelectObject(dcMemory,prevbm);
	::DeleteDC(dcMemory);
	return;
	}