// TicTacToe.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TicTacToe.h"

#define MAX_LOADSTRING 100
#define MAIN_WINDOW_WIDTH 220
#define MAIN_WINDOW_HEIGH 280
#define BUTTON_WIDTH 60
#define BUTTON_HEIGH 60
#define FIRST_COLUMN 10
#define SECOND_COLUMN FIRST_COLUMN + BUTTON_WIDTH
#define THIRD_COLUMN SECOND_COLUMN + BUTTON_WIDTH
#define FIRST_ROW 10
#define SECOND_ROW FIRST_ROW + BUTTON_HEIGH
#define THIRD_ROW SECOND_ROW + BUTTON_HEIGH

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HICON hCrossIcon, hCircleIcon;
std::shared_ptr<TicTacGame::Game> ticTacGame;

// Forward declarations of functions included in this code module:
ATOM				RegisterMainWindow(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	MainWindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void EndGame()
{
	if (ticTacGame->CheckIfSpecifiedPlayerWon(TicTacGame::Player::CROSS))
	{
		MessageBox(NULL, L"Player X won!", L"Win!", MB_ICONINFORMATION);
	}
	else if (ticTacGame->CheckIfSpecifiedPlayerWon(TicTacGame::Player::CIRCLE))
	{
		MessageBox(NULL, L"Player O won!", L"Win!", MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(NULL, L"Nobody won!", L"Draw!", MB_ICONINFORMATION);
	}
}


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TICTACTOE, szWindowClass, MAX_LOADSTRING);
	RegisterMainWindow(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICTACTOE));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: RegisterMainWindow()
//
//  PURPOSE: Registers the window class.
//
ATOM RegisterMainWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = MainWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICTACTOE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TICTACTOE);
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
	ticTacGame = std::make_shared<TicTacGame::Game>(true);
	HWND hWnd, ButtonOne, ButtonTwo, ButtonThree, ButtonFour, ButtonFive, ButtonSix, ButtonSeven, ButtonEight, ButtonNine;

	hInst = hInstance; // Store instance handle in our global variable
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGH, NULL, NULL, hInstance, NULL);

	ButtonOne = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, FIRST_COLUMN, FIRST_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_ONE, hInstance, NULL);
	ButtonTwo = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, SECOND_COLUMN, FIRST_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_TWO, hInstance, NULL);
	ButtonThree = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, THIRD_COLUMN, FIRST_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_THREE, hInstance, NULL);

	ButtonFour = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, FIRST_COLUMN, SECOND_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_FOUR, hInstance, NULL);
	ButtonFive = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, SECOND_COLUMN, SECOND_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_FIVE, hInstance, NULL);
	ButtonSix = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, THIRD_COLUMN, SECOND_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_SIX, hInstance, NULL);

	ButtonSeven = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, FIRST_COLUMN, THIRD_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_SEVEN, hInstance, NULL);
	ButtonEight = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, SECOND_COLUMN, THIRD_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_EIGHT, hInstance, NULL);
	ButtonNine = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, THIRD_COLUMN, THIRD_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_NINE, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	hCrossIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CROSSICO));
	hCircleIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CIRCLEICO));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: MainWindowProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if (wmId == IDB_ONE || wmId == IDB_TWO || wmId == IDB_THREE || wmId == IDB_FOUR || wmId == IDB_FIVE || wmId == IDB_SIX || wmId == IDB_SEVEN || wmId == IDB_EIGHT || wmId == IDB_NINE)
		{
			if (!ticTacGame->MakeMove((wmId % 10) - 1))
			{
				MessageBox(NULL, L"You're cheating!", L"Error!", MB_ICONEXCLAMATION);
				break;
			}

			SendMessage(GetDlgItem(hWnd, wmId), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(ticTacGame->CurrentPlayer() == TicTacGame::Player::CROSS ? hCrossIcon : hCircleIcon));
			if (ticTacGame->IsGameOver())
			{
				EndGame();
				DestroyWindow(hWnd);
				break;
			}

			if (ticTacGame->AiIsPlaying())
			{
				ticTacGame->UpdateRootNode((wmId % 10) - 1);
				auto x = ticTacGame->BestAvailableMove();
				ticTacGame->MakeMove(x);
				SendMessage(GetDlgItem(hWnd, x + 301), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(ticTacGame->CurrentPlayer() == TicTacGame::Player::CROSS ? hCrossIcon : hCircleIcon));
				if (ticTacGame->IsGameOver())
				{
					EndGame();
					DestroyWindow(hWnd);
					break;
				}

				ticTacGame->UpdateRootNode(x);
				ticTacGame->CleanUpTree();
			}

			if (!ticTacGame->AiIsPlaying())
			{
				ticTacGame->SwitchPlayer();
			}

			break;
		}

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
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
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
