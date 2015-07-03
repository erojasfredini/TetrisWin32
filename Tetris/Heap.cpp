#include "stdafx.h"
#include "Heap.h"

Heap::Heap(HWND hWND,int nova_x,int nova_y)
	{
	hWnd = hWND;

	x = nova_x;
	y = nova_y;

	HDC hdc = GetDC(hWnd);
	HeapWidth = ::GetDeviceCaps(hdc,HORZRES);
	HeapHeight = ::GetDeviceCaps(hdc,VERTRES);
	bmHeap = CreateBitmap(HeapWidth,HeapHeight,1,32,NULL);
	ReleaseDC(hWnd,hdc);

	//Just the bottom of the windows, the first base of the heap
	area = ::CreateRectRgn(x,y,HeapWidth,y+50);//base region
	top = ::CreateRectRgn(0,-50,HeapWidth,-1);//top of the heap

	//Initialize the blendfunction
	fnBlend.AlphaFormat = AC_SRC_ALPHA;
	fnBlend.BlendFlags = 0;
	fnBlend.BlendOp = AC_SRC_OVER;
	fnBlend.SourceConstantAlpha = 255;
	}

Heap::~Heap()
	{
	DeleteObject(bmHeap);
	DeleteObject(area);
	}

bool Heap::WillTouch(Piece* P, Movement M) 
	{
		//Show me the area for DEBUGING
		//HDC hdc = ::GetDC(hWnd);
		//FillRgn(hdc,area,(HBRUSH)GetStockObject(BLACK_BRUSH));
		//::ReleaseDC(hWnd,hdc);

	HRGN movedRGN = CreateRectRgn(0,0,1,1);//dummy region;
	CombineRgn(movedRGN,P->area,NULL,RGN_COPY);
	HRGN Intersection = CreateRectRgn(0,0,1,1);//dummy region;
	switch( M )
		{
		case MOVE_DOWN:
			{
			assert( OffsetRgn(movedRGN,0,BlockHeight) != ERROR);
			if( NULLREGION == CombineRgn(Intersection,area,movedRGN,RGN_AND) )
				{
				::DeleteObject(movedRGN);
				::DeleteObject(Intersection);
				return false;
				}else
				{
				::DeleteObject(movedRGN);
				::DeleteObject(Intersection);
				return true;
				}
			}
			break;
		case MOVE_RIGHT:
			{
			assert( OffsetRgn(movedRGN,BlockWidth,0) != ERROR);
			if( NULLREGION == CombineRgn(Intersection,area,movedRGN,RGN_AND) )
				{
				::DeleteObject(movedRGN);
				::DeleteObject(Intersection);
				return false;
				}else
				{
				::DeleteObject(movedRGN);
				::DeleteObject(Intersection);
				return true;
				}
			}
			break;
		case MOVE_LEFT:
			{
			assert( OffsetRgn(movedRGN,-BlockWidth,0) != ERROR);
			if( NULLREGION == CombineRgn(Intersection,area,movedRGN,RGN_AND) )
				{
				::DeleteObject(movedRGN);
				::DeleteObject(Intersection);
				return false;
				}else
				{
				::DeleteObject(movedRGN);
				::DeleteObject(Intersection);
				return true;
				}
			}
			break;
		}
	}

void Heap::AddPiece(HDC hdc, Piece* P)
	{
	//----- The RGN part
	HRGN tmpArea = area;
	CombineRgn(area,tmpArea,P->area,RGN_OR);

	HRGN TopIntersection = ::CreateRectRgn(0,0,1,1);//dummy region
	if( CombineRgn(TopIntersection,this->top,this->area,RGN_AND) != NULLREGION )
		SendMessage(hWnd,TM_GAMEOVER,NULL,NULL);

	DeleteObject(TopIntersection);
		
	//----- The Bitmap part

	//This will have the heap bitmap
	HDC dcHeap = ::CreateCompatibleDC(hdc);
	HBITMAP prevbmHeap = (HBITMAP)SelectObject(dcHeap,(HBITMAP)bmHeap);
	
	//Will be temporary to append the new piece to dcHeap
	HDC dcMemory = ::CreateCompatibleDC(hdc);
	HBITMAP prevbmMemory = (HBITMAP)SelectObject(dcMemory,(HBITMAP)P->sprite);

	//Appending the new piece
	assert( AlphaBlend(dcHeap,P->x,P->y,P->info.biWidth,P->info.biHeight,dcMemory,0,0,P->info.biWidth,P->info.biHeight,fnBlend) == true);

	SelectObject(dcMemory,(HBITMAP)prevbmMemory);
	//I already know that it will return P->sprite
	DeleteDC(dcMemory);

	//Paint the New Heap, I don´t call HeapPrint because I have the dc and every thing else, this will be faster
	assert( AlphaBlend(hdc,0,0,HeapWidth,HeapHeight,dcHeap,0,0,HeapWidth,HeapHeight,fnBlend) == true);
	Sleep(1000);//without this the displaying of the heap will flick, I think because the 
				//connection with the device is in another thread
	
	//Saving the Heap bitmap
	bmHeap = (HBITMAP)SelectObject(dcHeap,(HBITMAP)prevbmHeap);
	DeleteDC(dcHeap);

	
	//FillRgn(hdc,area,(HBRUSH)GetStockObject(BLACK_BRUSH));
	SearchFillRow(hdc);
	return;
	}

void Heap::SearchFillRow(HDC hdc)
	{
	for(int i=0;i < (6*3-1);++i)
		{
		HRGN rgnRow = CreateRectRgn(0,
			BlockHeight*i,
			4*3*BlockWidth,BlockHeight*(i+1));

		HRGN Intersection = CreateRectRgn(0,0,1,1);//dummy region
		if( NULLREGION != CombineRgn(Intersection,rgnRow,this->area,RGN_AND) )
			{
			if( NULLREGION == CombineRgn(Intersection,rgnRow,Intersection,RGN_XOR))
				{
				this->EraseRow(hdc,rgnRow,BlockHeight*i);
				}
			}
		}
	return;
	}

void Heap::EraseRow(HDC hdc, HRGN rgnRow,int pos)
	{
	//FillRgn(hdc,rgnRow,(HBRUSH)GetStockObject(BLACK_BRUSH));
	//::Sleep(3000);
	HDC dcMemory = CreateCompatibleDC(hdc);
	HBITMAP prevbm = (HBITMAP)SelectObject(dcMemory,(HBITMAP)this->bmHeap);

	HDC old = CreateCompatibleDC(hdc);
	HBITMAP n = CreateCompatibleBitmap(hdc,HeapWidth,HeapHeight);
	HBITMAP prevbm2 = (HBITMAP)SelectObject(old,(HBITMAP)n);
	
	assert( AlphaBlend(old,0,BlockHeight,HeapWidth,pos,dcMemory,0,0,HeapWidth,pos,fnBlend) == true);
	assert( AlphaBlend(old,0,pos+BlockHeight,HeapWidth,HeapHeight-(pos+BlockHeight),dcMemory,0,pos+BlockHeight,HeapWidth,HeapHeight-(pos+BlockHeight),fnBlend) == true);

	//PaintDesktop(hdc);
	//assert( AlphaBlend(hdc,0,BlockHeight,HeapWidth,HeapHeight,old,0,0,HeapWidth,HeapHeight,fnBlend) == true);
	//::Sleep(3000);

	this->bmHeap = (HBITMAP)SelectObject(old,prevbm2);
	DeleteDC(old);

	(HBITMAP)SelectObject(dcMemory,prevbm);
	DeleteDC(dcMemory);

	HRGN upper = CreateRectRgn(0,0,HeapWidth,pos);
	HRGN upper_intersection = CreateRectRgn(0,0,1,1);//dummy region
	CombineRgn(upper_intersection ,upper,this->area,RGN_AND);
	OffsetRgn(upper_intersection,0,BlockHeight);
	HRGN lower = CreateRectRgn(0,pos+BlockHeight,HeapWidth,HeapHeight);
	HRGN lower_intersection = CreateRectRgn(0,0,1,1);//dummy region
	CombineRgn(lower_intersection,lower,this->area,RGN_AND);

	CombineRgn(this->area,lower_intersection,upper_intersection,RGN_OR);
	
	::DeleteObject(upper);
	::DeleteObject(upper_intersection);
	::DeleteObject(lower);
	::DeleteObject(lower_intersection);

	//FillRgn(hdc,area,(HBRUSH)GetStockObject(BLACK_BRUSH));
	//::Sleep(3000);

	this->HeapPrint(hdc);

	//Plays the LineComplete Sound
	assert( PlaySound(_T("LineComplete"),hInst,SND_RESOURCE | SND_ASYNC) == TRUE);
		
	return;
	}

void Heap::HeapPrint(HDC hdc)
	{
	HDC dcHeap = ::CreateCompatibleDC(hdc);
	HBITMAP prevbm = (HBITMAP)SelectObject(dcHeap,(HBITMAP)bmHeap);

	assert( AlphaBlend(hdc,0,0,HeapWidth,HeapHeight,dcHeap,0,0,HeapWidth,HeapHeight,fnBlend) == true);

	SelectObject(dcHeap,(HBITMAP)prevbm);
	//I already know that it will return bmHeap
	DeleteDC(dcHeap);
	return;
	}

void Heap::UpdateRect(HDC hdc,int Left,int Top,int Right,int Bottom)
	{
	//If we try to map bmHeap to a negative position we´ll have an horrible error
	if( Top < 0 || Left < 0 )
		return;
	HDC dcHeap = ::CreateCompatibleDC(hdc);
	HBITMAP prevbm = (HBITMAP)SelectObject(dcHeap,(HBITMAP)bmHeap);

	assert( AlphaBlend(hdc,Left,Top,Right-Left,Bottom-Top,dcHeap,Left,Top,Right-Left,Bottom-Top,fnBlend) == true);

	SelectObject(dcHeap,(HBITMAP)prevbm);
	//I already know that it will return bmHeap
	DeleteDC(dcHeap);
	return;
	}