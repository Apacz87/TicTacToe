// TicTacToe.cpp : Defines the entry point for the application.

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
HINSTANCE hInst;								// Current instance
HWND MainWindowHandle;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HICON hCrossIcon, hCircleIcon;
std::shared_ptr<TicTacGame::Game> ticTacGame;


// The Game settings
static TicTacGame::GameSettings CurrentGameSettings;

// Forward declarations of functions included in this code module:
ATOM				RegisterMainWindow(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
HWND                InitializationOfMainWindow(HINSTANCE);
LRESULT CALLBACK	MainWindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL    CALLBACK    NewGameDlgProc(HWND, UINT, WPARAM, LPARAM);
bool                GameEnd(HWND&, std::shared_ptr<TicTacGame::Game>&);
bool                GameReinitialization(HWND&, std::shared_ptr<TicTacGame::Game>&);

// Game Over message.
bool EndGame(HWND& hWnd, std::shared_ptr<TicTacGame::Game>& game)
{
	if (ticTacGame->checkIfSpecifiedPlayerWon(TicTacGame::Player::CROSS))
	{
		MessageBox(NULL, L"Player X won!", L"Win!", MB_ICONINFORMATION);
	}
	else if (ticTacGame->checkIfSpecifiedPlayerWon(TicTacGame::Player::CIRCLE))
	{
		MessageBox(NULL, L"Player O won!", L"Win!", MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(NULL, L"Nobody won!", L"Draw!", MB_ICONINFORMATION);
	}

	int ret = DialogBox(hInst, MAKEINTRESOURCE(IDD_PLAY_AGAIN), hWnd, NewGameDlgProc);
	if (ret == IDOK)
	{
		return !GameReinitialization(hWnd, game);
	}
	
	return true;
}

// Initialization of a new game.
bool GameReinitialization(HWND& hWnd, std::shared_ptr<TicTacGame::Game>& game)
{
	int ret = DialogBox(hInst, MAKEINTRESOURCE(IDD_NEWGAME_DIALOG), hWnd, NewGameDlgProc);
	if (ret == IDOK)
	{
		game = std::make_shared<TicTacGame::Game>(CurrentGameSettings);
		//Resetting game board in GUI.
		for (size_t fieldNumber = 0; fieldNumber <= 9; fieldNumber++)
		{
			SendMessage(GetDlgItem(hWnd, fieldNumber + 301), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)NULL);
		}

		auto cp = game->currentPlayer();
		SetWindowText(GetDlgItem(hWnd, IDC_PLABEL), (LPCWSTR)&cp);
		return true;

	}
	
	return false;
}

// Function refreshing Node label.
void NodeLabelRefresh(HWND hwnd)
{
	while (true)
	{
		SetWindowText(GetDlgItem(hwnd, IDC_NLABEL), std::to_wstring(ticTacGame->numberOfExistingNodes()).c_str());
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

// The WinApi main function.
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
	hInst = hInstance; // Store instance handle in our global variable

	// Perform application initialization:
	int ret = DialogBox(hInst, MAKEINTRESOURCE(IDD_NEWGAME_DIALOG), NULL, NewGameDlgProc);
	if (ret == IDOK)
	{
		MainWindowHandle = InitializationOfMainWindow(hInst);
		if (!MainWindowHandle)
		{
			return FALSE;
		}
	}
	else if (ret == IDCANCEL)
		return FALSE;

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
//   FUNCTION: InitializationOfMainWindow(HINSTANCE)
//
//   PURPOSE: Creates main window
//
HWND InitializationOfMainWindow(HINSTANCE hInstance)
{
	ticTacGame = std::make_shared<TicTacGame::Game>(CurrentGameSettings);
	HWND hWnd, ButtonOne, ButtonTwo, ButtonThree, ButtonFour, ButtonFive, ButtonSix, ButtonSeven, ButtonEight, ButtonNine, PlayerLabel, CurentPlayerLabel, NodesLabel, NumberOfNodesLabel;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGH, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return NULL;
	}

	ButtonOne = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, FIRST_COLUMN, FIRST_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_ONE, hInstance, NULL);
	ButtonTwo = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, SECOND_COLUMN, FIRST_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_TWO, hInstance, NULL);
	ButtonThree = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, THIRD_COLUMN, FIRST_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_THREE, hInstance, NULL);

	ButtonFour = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, FIRST_COLUMN, SECOND_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_FOUR, hInstance, NULL);
	ButtonFive = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, SECOND_COLUMN, SECOND_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_FIVE, hInstance, NULL);
	ButtonSix = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, THIRD_COLUMN, SECOND_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_SIX, hInstance, NULL);

	ButtonSeven = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, FIRST_COLUMN, THIRD_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_SEVEN, hInstance, NULL);
	ButtonEight = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, SECOND_COLUMN, THIRD_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_EIGHT, hInstance, NULL);
	ButtonNine = CreateWindow(L"BUTTON", NULL, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_ICON, THIRD_COLUMN, THIRD_ROW, BUTTON_WIDTH, BUTTON_HEIGH, hWnd, (HMENU)IDB_NINE, hInstance, NULL);

	PlayerLabel = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_LEFT, 5, MAIN_WINDOW_HEIGH - 85, 46, 18, hWnd, NULL, hInstance, NULL);
	CurentPlayerLabel = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_LEFT, 54, MAIN_WINDOW_HEIGH - 85, 10, 18, hWnd, (HMENU)IDC_PLABEL, hInstance, NULL);

	NodesLabel = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_LEFT, 103, MAIN_WINDOW_HEIGH - 85, 47, 18, hWnd, NULL, hInstance, NULL);
	NumberOfNodesLabel = CreateWindow(L"STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_LEFT, 151, MAIN_WINDOW_HEIGH - 85, 47, 18, hWnd, (HMENU)IDC_NLABEL, hInstance, NULL);

	SetWindowText(PlayerLabel, L"Player:");
	auto playerSymbol = ticTacGame->currentPlayer();
	SetWindowText(CurentPlayerLabel, (LPCWSTR)&playerSymbol);
	SetWindowText(NodesLabel, L"Nodes:");
	SetWindowText(NumberOfNodesLabel, std::to_wstring(ticTacGame->numberOfExistingNodes()).c_str());
	hCrossIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CROSSICO));
	hCircleIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CIRCLEICO));

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	//Creation of the thread for refreshing nodes label
	std::thread(NodeLabelRefresh, hWnd).detach();

	return hWnd;
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
			if (!ticTacGame->isMoveAllowed(((wmId % 10) - 1)))
			{
				MessageBox(NULL, L"You're cheating!", L"Error!", MB_ICONEXCLAMATION);
				break;
			}

			SendMessage(GetDlgItem(hWnd, wmId), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(ticTacGame->currentPlayer() == TicTacGame::Player::CROSS ? hCrossIcon : hCircleIcon));
			ticTacGame->makeMove(((wmId % 10) - 1));
			if (ticTacGame->isGameOver())
			{
				if (EndGame(hWnd, ticTacGame))
				{
					DestroyWindow(hWnd);
				}

				break;
			}

			if (ticTacGame->isAiPlaying())
			{
				auto x = ticTacGame->bestAvailableMove();
				SendMessage(GetDlgItem(hWnd, x + 301), BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)(ticTacGame->currentPlayer() == TicTacGame::Player::CROSS ? hCrossIcon : hCircleIcon));
				ticTacGame->makeMove(x);
				if (ticTacGame->isGameOver())
				{
					if (EndGame(hWnd, ticTacGame))
					{
						DestroyWindow(hWnd);
					}

					break;
				}
			}

			auto cp = ticTacGame->currentPlayer();
			SetWindowText(GetDlgItem(hWnd, IDC_PLABEL), (LPCWSTR)&cp);
			SetWindowText(GetDlgItem(hWnd, IDC_NLABEL), std::to_wstring(ticTacGame->numberOfExistingNodes()).c_str());
			break;
		}

		// Parse the menu selections:
		switch (wmId)
		{
			case IDM_NEWGAME:
				GameReinitialization(hWnd, ticTacGame);
				break;
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

// Message handler for new game box.
BOOL CALLBACK NewGameDlgProc(HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
		case WM_INITDIALOG:
			{
				HWND hDropdownlist = GetDlgItem(hwnd, IDC_ALGORITHM_BOX);
				SendMessage(hDropdownlist, CB_ADDSTRING, 0, (LPARAM)L"MinMax - Game Tree");
				SendMessage(hDropdownlist, CB_ADDSTRING, 0, (LPARAM)L"MinMax - Recursively");
				SendMessage(hDropdownlist, CB_SETCURSEL, 0, NULL);
			}
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					{
						HWND hPlayWithAi = GetDlgItem(hwnd, IDC_PLAY_WITH_AI);
						HWND hDropdownlist = GetDlgItem(hwnd, IDC_ALGORITHM_BOX);
						CurrentGameSettings.AI = (IsDlgButtonChecked(hwnd, IDC_PLAY_WITH_AI) == BST_CHECKED);
						CurrentGameSettings.Implementation = (TicTacGame::Algorithm)SendMessage(hDropdownlist, CB_GETCURSEL, 0, 0);
					}
					EndDialog(hwnd, IDOK);
					break;
				case IDCANCEL:
					EndDialog(hwnd, IDCANCEL);
					break;
			}
			break;
		default:
			return FALSE;
	}
	return TRUE;
}
