// Tetris.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Tetris.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HBITMAP bmBackground;
HBRUSH brBackground;
//Tetris Message GameOver
//					wParam: NULL
//					lParam: NULL
UINT TM_GAMEOVER;

enum GameState{Play,GameOver};
GameState MainGameState;

char* hMainThemeRaw;

//Medium Quality	
unsigned int BlockWidth = 126/3;
unsigned int BlockHeight = 126/3;

unsigned int cpWndWidth;//negrada
unsigned int cpWndHeight;
HWND hWnd;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndTetrisProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
 	MSG msg;
	HACCEL hAccelTable;

	MainGameState = GameState::Play;

	HRSRC MusicInfo = FindResource(hInstance,MAKEINTRESOURCE(IDW_MAINTHEME),RT_RCDATA);
	hMainThemeRaw = (char*)LoadResource(hInstance,MusicInfo);

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TETRIS, szWindowClass, MAX_LOADSTRING);

	assert( (TM_GAMEOVER = RegisterWindowMessage(_T("GAMEOVER"))) != 0 );

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TETRIS));

	// Main message loop:
	while(true)
		{
		if( PeekMessage(&msg,NULL,0,0,PM_REMOVE) )
			{
			if( msg.message == WM_QUIT )
				break;

			if(MainGameState == GameState::GameOver)//negrada
				{
				HDC hdc = GetDC(hWnd);
				RECT rec;
				rec.top=cpWndHeight/6;rec.bottom=2*cpWndHeight/6;rec.left=cpWndWidth/4;rec.right=3*cpWndWidth/4;
				HFONT fontGameOver = CreateFont(40,40,NULL,NULL,900,false,false,NULL,NULL,NULL,NULL,ANTIALIASED_QUALITY,NULL,NULL);
				HFONT prevfont = (HFONT)SelectObject(hdc,fontGameOver);
				SetBkMode(hdc,TRANSPARENT);//The text is transparent
				DrawText(hdc,_T("Game\nOver"),9,&rec,DT_VCENTER | DT_CENTER);
				DeleteObject(SelectObject(hdc,prevfont));
				ReleaseDC(hWnd,hdc);
				
				continue;//if the game is over just stand still until the window is close
				}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}else
			{
			//Here we update the non-timed animations

				}
		}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	bmBackground = ::LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_BACKGROUND));
	brBackground= ::CreatePatternBrush(bmBackground);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndTetrisProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TETRIS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= brBackground;
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_TETRIS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_DLGFRAME | WS_SYSMENU,
      CW_USEDEFAULT, 0, 4*3*BlockWidth+6/*los 18 compensan los bordes de la ventana*/,
	  6*3*BlockHeight-16
	  /*los -4 compensan los bordes de la ventana*/, NULL, NULL, hInstance, NULL);
   
   ::hWnd = hWnd;//negrada
   
   //RECT rec;
   //GetClientRect(hWnd,&rec);

   if (!hWnd)
   {
      return FALSE;
   }

   SetTimer(hWnd,IDT_USR_PIECE,1000,NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndTetrisProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	//The Width and Height in pixels of the screen
	static unsigned int cpWndWidth;
	static unsigned int cpWndHeight;

	//The list of Pices, this have the next Pieces that will come
	static std::list<Piece*> lsPieces;
	//The current Piece, this one it is not in lsPieces
	//It is the one that reacts to user input
	static Piece* CurrentPiece;

	static Heap* H= new Heap(hWnd,0,6*3*BlockHeight-4);

	//Music Resource
	static HWAVEOUT hWaveOut;
	static WAVEFORMATEX waveformat; 
	static PWAVEHDR pWaveHdr1;
	static PBYTE pBuffer1;

	switch (message)
	{
	case WM_CREATE:
		{
		//So InsertRandomPieces will generate sequences pseudo-aleatory
		::srand(time(NULL));

		::SetupNextPiece(hWnd,CurrentPiece,lsPieces);

		pWaveHdr1= new WAVEHDR;
		ZeroMemory(pWaveHdr1,sizeof(WAVEHDR));
		waveformat = *((WAVEFORMATEX*)(hMainThemeRaw+=20));
		waveformat.cbSize=0;
		hMainThemeRaw+=sizeof(WAVEFORMATEX)-2+4;
		pWaveHdr1->dwBufferLength= *((unsigned int*)hMainThemeRaw);
		hMainThemeRaw += 4;
		pWaveHdr1->lpData= hMainThemeRaw;

		pWaveHdr1->dwLoops=1000;//Repeat it many times, if it ends all this times, the buffer re-starts

				//Tring to load the file from disk
				//pWaveHdr1= new WAVEHDR;
				//OFSTRUCT ofInfo;
				//HFILE musicFile = OpenFile("Content\\Menu_Loop.wav",&ofInfo,OF_READ);
				//(musicFile == NULL)? exit(0):0;

				//DWORD cBytes=0;
				//ReadFile((HANDLE)musicFile,&waveformat,20,&cBytes,NULL);
				//if( (ReadFile((HANDLE)musicFile,&waveformat ,sizeof(WAVEFORMATEX)-2,(LPDWORD)&cBytes,NULL) == 0) || (cBytes == 0) )
				//	exit(0);
				//ReadFile((HANDLE)musicFile,pWaveHdr1,4,&cBytes,NULL);
				//ZeroMemory(pWaveHdr1,sizeof(WAVEHDR));
				//ReadFile((HANDLE)musicFile,&(pWaveHdr1->dwBufferLength),4,&cBytes,NULL);
				//char* buf= new char[pWaveHdr1->dwBufferLength];
				//cBytes=0;
				//if( (ReadFile((HANDLE)musicFile,buf,pWaveHdr1->dwBufferLength,(LPDWORD)&cBytes,NULL) == 0) || (cBytes == 0) )
				//	exit(0);
				//pWaveHdr1->lpData = buf;
				//CloseHandle((HANDLE)musicFile);
				//End for the load
				
		assert( waveOutOpen(&hWaveOut,WAVE_MAPPER,&waveformat,(DWORD_PTR)hWnd,0,CALLBACK_WINDOW) == MMSYSERR_NOERROR );
	
		assert( waveOutPrepareHeader(hWaveOut,pWaveHdr1,sizeof(WAVEHDR)) == MMSYSERR_NOERROR );
			
		}
		break;
	case MM_WOM_OPEN:
		{
		assert( waveOutWrite(hWaveOut,pWaveHdr1,sizeof(WAVEHDR)) == MMSYSERR_NOERROR );
		return true;
		}
		break;
	case MM_WOM_DONE:
		{
		assert( waveOutWrite(hWaveOut,pWaveHdr1,sizeof(WAVEHDR)) == MMSYSERR_NOERROR );
		return true;
		}
		break;
	case WM_SIZE:
		{
		cpWndWidth = LOWORD(lParam);
		cpWndHeight = HIWORD(lParam);

		::cpWndWidth = LOWORD(lParam);//negrada
		::cpWndHeight = HIWORD(lParam);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
		hdc = BeginPaint(hWnd, &ps);

		CurrentPiece->Draw(hdc,CurrentPiece->GetX(),CurrentPiece->GetY());
		H->HeapPrint(hdc);
		
		EndPaint(hWnd, &ps);
		}
		break;
	case WM_LBUTTONDOWN:
		{
		//::HRGN client;
		//::RECT rect;
		//::GetClientRect(hWnd,&rect);
		//client = ::CreateRectRgnIndirect(&rect);
		////hdc = ::GetDC(hWnd);
		////HRGN area = CreateRectRgn(0,0,1,1);
		////HRGN lower = CreateRectRgn(0,0+BlockHeight*2,0+BlockWidth*3,0+BlockHeight*3);
		////HRGN upper = CreateRectRgn(0+BlockWidth*2,BlockHeight,0+BlockWidth*3,0+BlockHeight*2);
		////CombineRgn(area,lower,upper,RGN_OR);
		////::FillRgn(hdc,area,(HBRUSH)GetStockObject(BLACK_BRUSH));
		////ReleaseDC(hWnd,hdc);

		/*if( PtInRegion(A.area,LOWORD(lParam),HIWORD(lParam)) )
			{
			hdc = ::GetDC(hWnd);
			TextOut(hdc,400,400,_T("Golpeaste adentro!"),18);
			ReleaseDC(hWnd,hdc);
			}else
			{
			hdc = ::GetDC(hWnd);
			TextOut(hdc,400,400,_T("No golpeaste"),12);
			ReleaseDC(hWnd,hdc);
				}*/
		}
		break;
	case WM_KEYDOWN:
		{
		switch( wParam )
			{
			case VK_RIGHT:
				{
				int nextpos = ((CurrentPiece->GetX()+BlockWidth)+BlockWidth*3);
				if( nextpos > cpWndWidth )
					break;

				hdc = ::GetDC(hWnd);
					
				if( !H->WillTouch(CurrentPiece,Heap::MOVE_RIGHT) ) 
					{
					//The Move also cleans the pass region
					CurrentPiece->MoveXYBlocks(hdc,H,BlockWidth,0);
					}

				::ReleaseDC(hWnd,hdc);
				}
				break;
			case VK_LEFT:
				{
				int nextpos = (CurrentPiece->GetX()-BlockWidth);
				if( nextpos < 0 )
					break;
				hdc = ::GetDC(hWnd);

				if( !H->WillTouch(CurrentPiece,Heap::MOVE_LEFT) ) 
					{
					//The Move also cleans the pass region
					CurrentPiece->MoveXYBlocks(hdc,H,-BlockWidth,0);
					}

				::ReleaseDC(hWnd,hdc);
				}
				break;
			case VK_DOWN:
				{
				hdc = ::GetDC(hWnd);

				if( H->WillTouch(CurrentPiece,Heap::MOVE_DOWN) ) 
				{
				assert( PlaySound(_T("BlockDown"),hInst,SND_RESOURCE | SND_ASYNC) == TRUE);
				H->AddPiece(hdc, CurrentPiece);
				if( dynamic_cast<Piece_A*>(CurrentPiece) )
					delete static_cast<Piece_A*>(CurrentPiece);
				SetupNextPiece(hWnd,CurrentPiece,lsPieces);
				}else
					{
					//The Move also cleans the pass region
					CurrentPiece->MoveXYBlocks(hdc,H,0,BlockHeight);
					}

				::ReleaseDC(hWnd,hdc);
				}
				break;
			case VK_UP:
				{
				hdc = ::GetDC(hWnd);

				ClearScreen(hWnd,hdc);

				CurrentPiece->Turn(hdc);

				::ReleaseDC(hWnd,hdc);
				}
				break;
			}
		//Show the position of the Current Piece, so we can DEBUG
		/*
		hdc = GetDC(hWnd);
		RECT text;text.top=0;text.bottom=50;
		TCHAR* buf = new TCHAR[200];
		::_itot(CurrentPiece->GetX(),buf,10);
		text.left=175;text.right=200;
		::DrawText(hdc,_T("X:"),2,&text,NULL);
		text.left=200;text.right=400;
		::DrawText(hdc,buf,wcslen(buf),&text,NULL);
		::_itot(CurrentPiece->GetY(),buf,10);text.top=50;text.bottom=100;
		text.left=175;text.right=200;
		::DrawText(hdc,_T("Y:"),2,&text,NULL);
		text.left=200;text.right=400;
		::DrawText(hdc,buf,wcslen(buf),&text,NULL);
		delete buf;
		::ReleaseDC(hWnd,hdc);
		//End the DEBUGING aid code
		*/
		}
		break;
	case WM_TIMER:
		{
		if( wParam = IDT_USR_PIECE )
			{
			hdc = ::GetDC(hWnd);
			H->HeapPrint(hdc);

				//WCHAR* buf=new WCHAR[100];
				//_itow(lsPieces.size(),buf,10);
				//TextOut(hdc,200,200,buf,::wcslen(buf));
				//delete[] buf;

			/*
			if( (CurrentPiece->GetY()+BlockHeight)+BlockHeight*3 > cpWndHeight )
				SetupNextPiece(hWnd,CurrentPiece,lsPieces);
				*/

			if( H->WillTouch(CurrentPiece,Heap::MOVE_DOWN) ) 
			{
			assert( PlaySound(_T("BlockDown"),hInst,SND_RESOURCE | SND_ASYNC) == TRUE);
			H->AddPiece(hdc, CurrentPiece);
			if( dynamic_cast<Piece_A*>(CurrentPiece) )
				delete static_cast<Piece_A*>(CurrentPiece);
			
			SetupNextPiece(hWnd,CurrentPiece,lsPieces);
			}else
				{
				//The Move also cleans the pass region			
				CurrentPiece->MoveXYBlocks(hdc,H,0,BlockHeight);
				}
			
			::ReleaseDC(hWnd,hdc);
			}
		}
		break;
	case WM_DESTROY:
		{
		::DeleteObject(bmBackground);
		::DeleteObject(brBackground);

		delete H;

		if( dynamic_cast<Piece_A*>(CurrentPiece) )
			delete static_cast<Piece_A*>(CurrentPiece);

		//If i do this i´ll have problem when it trys to erase the list
		//std::list<Piece*>::iterator it = lsPieces.begin();
		//while( it != lsPieces.end() )
		//	{
		//	if( dynamic_cast<Piece_A*>(*it) )
		//		delete static_cast<Piece_A*>(*it);
		//	}
		lsPieces.clear();
		PostQuitMessage(0);
		}
		break;
	default:
		{
		if( message == TM_GAMEOVER )
			{
			assert( PlaySound(_T("GameOver"),hInst,SND_RESOURCE | SND_ASYNC) == TRUE );
			MainGameState = GameState::GameOver;
			RECT rec;
			GetClientRect(hWnd,&rec);
			InvalidateRect(hWnd,&rec,TRUE);
			return true;
			}
		return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;

}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
