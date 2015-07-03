#pragma once


#include <cassert>
#include <windows.h>
#include "Resource.h"
#include <vector>


extern HINSTANCE hInst;

//Width of a single block, a Piece is made of four
extern unsigned int BlockWidth;
//Height of a single block, a Piece is made of four
extern unsigned int BlockHeight;

//Base class for all the Pieces
//It make the common initialization, actions and justifies the polimorfism
class Piece
	{
	protected:
		//This have the bitmaps, one for every rotation
		std::vector<HBITMAP> spriteTurns;
		//The current bitmap
		HBITMAP sprite;
		//This have the info of the bitmaps, only the nesesary, not repeated
		std::vector<BITMAPINFOHEADER> infoTurns;
		//The current info of the bitmap
		BITMAPINFOHEADER info;
		//The windows where the piece is located
		HWND hWnd;
		//The blend function to apply
		BLENDFUNCTION fnBlend;

		//The current coordinates of the piece
		//in logic unites
		int x;
		int y;
		//All the regions, respectively with the bitmaps
		std::vector<HRGN> areaTurns;
		//The current area
		HRGN area;
		//The current orientation state
		int PieceState;

		friend class Heap;
	public:
		Piece(HWND hWND,int nova_x=0,int nova_y=0);

		~Piece();

		virtual void Draw(HDC hdc, int nova_x, int nova_y);
						
		virtual void MoveXYBlocks(HDC hdc, Heap* H, int offset_x, int offset_y);
						
		virtual void Turn(HDC hdc);
						
		int GetX();

		int GetY();
	};

//This piece:
//					---
//				    | |
//				-------
//				| | | |
//				-------
//								By default
class Piece_A: public Piece
	{
	private:
	public:
		Piece_A(HWND hWND,int nova_x=0,int nova_y=0):Piece(hWND,nova_x,nova_y)
			{
			//Loads the specific bitmap
			spriteTurns.push_back( LoadBitmap(hInst,MAKEINTRESOURCE(IDB_PIECE_A_MED_B)) );
			spriteTurns.push_back( LoadBitmap(hInst,MAKEINTRESOURCE(IDB_PIECE_A_MED_B_90)) );
			spriteTurns.push_back( LoadBitmap(hInst,MAKEINTRESOURCE(IDB_PIECE_A_MED_B_180)) );
			spriteTurns.push_back( LoadBitmap(hInst,MAKEINTRESOURCE(IDB_PIECE_A_MED_B_240)) );
			sprite = spriteTurns[0];

			BITMAP bmSprite;
			GetObject(sprite,sizeof(BITMAP),&bmSprite);
			BITMAPINFO bminfoSprite;

			
			#pragma region Initialize the specific regions
			areaTurns.push_back( ::CreateRectRgn(0,0,1,1) );//dummy region
			HRGN lower;
			HRGN upper;

			//We can have redond errors
			if( abs(bmSprite.bmHeight - BlockHeight*3) < 10 )//We can load IDB_PIECE_A_MED or IDB_PIECE_A_MED_B
				{											 //the diference is the Height, this code threats it
				lower = CreateRectRgn(x,y+BlockHeight*2,x+BlockWidth*3,y+BlockHeight*3);
				upper = CreateRectRgn(x+BlockWidth*2,y+BlockHeight,x+BlockWidth*3,y+BlockHeight*2);
				}else
				{
				lower = CreateRectRgn(x,y+BlockHeight,x+BlockWidth*3,y+BlockHeight*2);
				upper = CreateRectRgn(x+BlockWidth*2,y,x+BlockWidth*3,y+BlockHeight);
					}
			 
			CombineRgn(areaTurns[0],lower,upper,RGN_OR);
			DeleteObject(lower);
			DeleteObject(upper);

			areaTurns.push_back( ::CreateRectRgn(0,0,1,1) );//dummy region

			//We can have redond errors
			if( abs(bmSprite.bmHeight - BlockHeight*3) < 10 )//We can load IDB_PIECE_A_MED or IDB_PIECE_A_MED_B
				{											 //the diference is the Height, this code threats it
				lower = CreateRectRgn(x,y+BlockHeight*2,x+BlockWidth*3,y+BlockHeight*3);
				upper = CreateRectRgn(x+BlockWidth*2,y+BlockHeight,x+BlockWidth*3,y+BlockHeight*2);
				}else
				{
				lower = CreateRectRgn(x,y,x+BlockWidth,y+BlockHeight*3);
				upper = CreateRectRgn(x+BlockWidth,y+BlockHeight*2,x+BlockWidth*2,y+BlockHeight*3);
					}

			CombineRgn(areaTurns[1],lower,upper,RGN_OR);
			DeleteObject(lower);
			DeleteObject(upper);

			areaTurns.push_back( ::CreateRectRgn(0,0,1,1) );//dummy region

			//We can have redond errors
			if( abs(bmSprite.bmHeight - BlockHeight*3) < 10 )//We can load IDB_PIECE_A_MED or IDB_PIECE_A_MED_B
				{											 //the diference is the Height, this code threats it
				lower = CreateRectRgn(x,y+BlockHeight*2,x+BlockWidth*3,y+BlockHeight*3);
				upper = CreateRectRgn(x+BlockWidth*2,y+BlockHeight,x+BlockWidth*3,y+BlockHeight*2);
				}else
				{
				lower = CreateRectRgn(x,y,x+BlockWidth*3,y+BlockHeight);
				upper = CreateRectRgn(x,y+BlockHeight,x+BlockWidth,y+BlockHeight*2);
					}

			CombineRgn(areaTurns[2],lower,upper,RGN_OR);
			DeleteObject(lower);
			DeleteObject(upper);

			areaTurns.push_back( ::CreateRectRgn(0,0,1,1) );//dummy region

			//We can have redond errors
			if( abs(bmSprite.bmHeight - BlockHeight*3) < 10 )//We can load IDB_PIECE_A_MED or IDB_PIECE_A_MED_B
				{											 //the diference is the Height, this code threats it
				lower = CreateRectRgn(x,y+BlockHeight*2,x+BlockWidth*3,y+BlockHeight*3);
				upper = CreateRectRgn(x+BlockWidth*2,y+BlockHeight,x+BlockWidth*3,y+BlockHeight*2);
				}else
				{
				lower = CreateRectRgn(x,y,x+BlockWidth*2,y+BlockHeight);
				upper = CreateRectRgn(x+BlockWidth,y+BlockHeight,x+BlockWidth*2,y+BlockHeight*3);
					}

			CombineRgn(areaTurns[3],lower,upper,RGN_OR);
			DeleteObject(lower);
			DeleteObject(upper);
			#pragma endregion

			area = areaTurns[0];

			HDC hdc = GetDC(hWnd);

			for( int g=0;g < 4;g++)
				{
				HBITMAP BM = spriteTurns[g];

				BITMAP bmSprite;
				GetObject(BM,sizeof(BITMAP),&bmSprite);
				BITMAPINFO bminfoSprite;

				ZeroMemory(&bminfoSprite, sizeof(BITMAPINFO));

				bminfoSprite.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				bminfoSprite.bmiHeader.biWidth = bmSprite.bmWidth;
				bminfoSprite.bmiHeader.biHeight = bmSprite.bmHeight;
				bminfoSprite.bmiHeader.biPlanes = bmSprite.bmPlanes;
				bminfoSprite.bmiHeader.biBitCount = bmSprite.bmBitsPixel;
				bminfoSprite.bmiHeader.biCompression = BI_RGB;

				int iImageSize = bmSprite.bmHeight*bmSprite.bmWidth*bmSprite.bmBitsPixel;

				bminfoSprite.bmiHeader.biSizeImage = iImageSize;

				LPVOID bits = new char[iImageSize];

				//Begin the alpha pre-multiplication and alpha assignament
				assert(GetDIBits(hdc,BM,0,bmSprite.bmHeight,bits,&bminfoSprite,DIB_RGB_COLORS));

				info = bminfoSprite.bmiHeader;
				if( g == 0 ) 
					infoTurns.push_back(info);
				if( g == 1 ) 
					infoTurns.push_back(info);

				for( int x=0;x < bmSprite.bmHeight; ++x )
					for( int y=0;y < bmSprite.bmWidth; ++y )
						{
						int curPixel =x*bmSprite.bmWidth*(bmSprite.bmBitsPixel/8)+y*(bmSprite.bmBitsPixel/8);
						//The withe pixels should be transparent
						if( ((unsigned char*)bits)[curPixel] == 255 
							&& ((unsigned char*)bits)[curPixel+1] == 255
							&& ((unsigned char*)bits)[curPixel+2] == 255 )
							((unsigned char*)bits)[curPixel+3] = 0;
						else
							((unsigned char*)bits)[curPixel+3] = 255;

						//This make the black zones have a cool efect like an hole in the blocks
						if( ((unsigned char*)bits)[curPixel] <= 30 
							&& ((unsigned char*)bits)[curPixel+1] <= 30
							&& ((unsigned char*)bits)[curPixel+2] <= 30 )
							((unsigned char*)bits)[curPixel+3] = 0;

						//Premultiply RGB channels by alpha channel
						((unsigned char*)bits)[curPixel]=
							((unsigned char*)bits)[curPixel] * ((unsigned char*)bits)[curPixel+3] /255;

						((unsigned char*)bits)[curPixel+1]=
							((unsigned char*)bits)[curPixel+1] * ((unsigned char*)bits)[curPixel+3] /255;

						((unsigned char*)bits)[curPixel+2]=
							((unsigned char*)bits)[curPixel+2] * ((unsigned char*)bits)[curPixel+3] /255;
						}
					
				assert(SetDIBits(hdc,BM,0,bmSprite.bmHeight,bits,&bminfoSprite,DIB_RGB_COLORS));
				delete[] bits;
				//End of alpha pre-multiplication and alpha assignament
				}
			//The first is horizontal
			info = infoTurns[0];

			ReleaseDC(hWnd,hdc);
			}
	
		~Piece_A();

		//Draws the piece in that coordinates
		void Draw(HDC hdc, int nova_x, int nova_y);

		//Moves the piece this values
		//And cleans the previous region passed
		void MoveXYBlocks(HDC hdc, Heap* H, int offset_x, int offset_y);

		//Turns the piece clock-wishe
		//And cleans the previous region turned
		void Turn(HDC hdc);
	};