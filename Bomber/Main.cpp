#pragma once
#include "Headers.h"

wchar_t AppTitle[] = L"Bomber";

// Default field size
int FieldWidth = 10;
int FieldHeight = 10;
// Min/Max field size
int fieldMaxWidth = 64, fieldMinWidth = 5;
int fieldMaxHeight = 64, fieldMinHeight = 5;
// Global game object
Game game;
// View object for interact with opengl
View view;
// Main procedure
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
// Child procedures for statistic window and game setup window
BOOL CALLBACK ChildDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK statDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


int WINAPI WinMain(HINSTANCE hInstance,		// App handle
	HINSTANCE hPrevInstance,				// Parend app handle
	LPSTR lpCmdLine,						// CMD params
	int nCmdShow)							// Window status param
{
	MSG msg;							// Windows messages struct
	BOOL done = false;					// End of the while
										// Setup view object
	game.SetView(&view);
	// Create window
	if (!view.CreateGLWindow(WndProc, AppTitle, 32)) return 0;
	
	// Load obj model from file
	ObjReader reader;
	GLuint vertexList;
	if (reader.load("Model/bomb.obj", vertexList, 400) == false) {
		MessageBox(0, L"Unable to load obj model", L"Error", MB_ICONERROR);
	}
	view.setObj(vertexList);

	// Proceed window messages
	while (!done) {
		// Get next message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// Proceed exit message
			if (msg.message == WM_QUIT) done = true;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		// No messages yet
		else {
			// If window is active
			if (view.active) {
				// Check if we press escape key
				if (view.keys[VK_ESCAPE]) {
					// Time to close
					done = true;
				}
				// Otherwise update screen
				else
				{
					SetWindowText(view.hWnd, AppTitle);
					view.DrawGLScene();
					SwapBuffers(view.hDC);
				}
			}
		}
	}
	// Save game stat before exit
	game.GetStat().SaveStat();

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,	UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:// When window is open trying to load game stats from file
		game.GetStat().LoadStat();
		return TRUE;
		break;
	case WM_ACTIVATE: {
		view.active = !HIWORD(wParam);
		return 0;
	}

	case WM_SYSCOMMAND: {
		switch (wParam)
		{
			// Prevent screensaver execution
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}
	
	case WM_CLOSE: { // Close window
		PostQuitMessage(0);
		return 0;
	}

	case WM_KEYDOWN: { // Keyboard interaction "key down"
		// Setup key marker
		view.keys[wParam] = true;
		if (wParam == VK_UP) view.menuUp();
		if (wParam == VK_DOWN) view.menuDown();
		if (wParam == VK_RETURN) {
			switch (view.getSelectedMenuItem()) {
			case 1:
				// Execute dialog from recource
				DialogBox(NULL, MAKEINTRESOURCE(IDD_CHILDLG), view.hWnd, ChildDlgProc);
				//DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_CHILDLG), view.hWnd, (DLGPROC)ChildDlgProc, NULL);
				// Default field params
				FieldWidth = FIELD_TYPE0;
				FieldHeight = FIELD_TYPE0;
				break;
			case 2:
				DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOGBAR), view.hWnd, statDlgProc);
				//DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_DIALOGBAR), view.hWnd, (DLGPROC)statDlgProc, NULL);
				break;
			case 3: 
				view.keys[VK_ESCAPE] = true;
				return 0;
			default:
				break;
			}
		}
		return 0;
	}

	case WM_KEYUP: { // Keyboard interaction "key up"
		// Remove key marker
		view.keys[wParam] = false;
		return 0;
	}

	case WM_SIZE: { // Resize window
		view.ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}

	case WM_TIMER: { // Game timer
		game.UpdateTimer();
		return 0;
	}

	case WM_LBUTTONDOWN: {
		// If game model is not setup -> do nothing
		if (game.IsGameOver() && !game.IsStart()) return 0;
		// Otherwise proceed mouse click
		// Click coords
		POINT p;
		// Get coords
		p.x = MAKEPOINTS(lParam).x;
		p.y = MAKEPOINTS(lParam).y;
		// Recalculate click position to determine field cell
		POINT pgl = view.GetOGLPos(p.x, p.y);
		// Click inside game field ?
		if (pgl.x < view.SceneWidth - view.BorderSize &&
			pgl.x > view.BorderSize &&
			pgl.y < view.SceneHeight - view.BorderSize &&
			pgl.y > view.BorderSize)
		{
			// Get cell indices
			POINT id = view.GetCellIndex(pgl);
			if (id.x == -1 || id.y == -1) return 0;
			// Game is not start yet
			if (!game.IsStart()) {
				// Start it
				game.GameStart();
			}
			// Open cell
			game.GetModel()->Open(id.x, id.y);
			// Check game status
			if (game.GameProcess(id.x, id.y) == GameState::CompWin) {
				MessageBox(hWnd, L"You loose", L"Game Over", MB_OK | MB_ICONERROR);
				game.EndGame();
				return 0;
			}
			else if (game.GameProcess(id.x, id.y) == GameState::PlayerWin) {
				wchar_t buff[32];
				_itow(game.GetModel()->timer, buff, 10);
				MessageBox(hWnd, L"You Win", L"LEFT BUTTON", MB_OK | MB_ICONEXCLAMATION);
				MessageBox(hWnd, buff, L"Total elapsed time", MB_OK | MB_ICONINFORMATION);
				game.EndGame();
				return 0;
			}
		}
		return 0;
	}

	case WM_RBUTTONDOWN: {
		// Game is not created yet -> do nothing
		if (!game.GetModel()) return 0;
		// Otherwise get click coords
		POINT p;
		p.x = MAKEPOINTS(lParam).x;
		p.y = MAKEPOINTS(lParam).y;
		// Recalculate position
		POINT pgl = view.GetOGLPos(p.x, p.y);
		// Mouse click inside game field ?
		if (pgl.x < view.SceneWidth - view.BorderSize &&
			pgl.x > view.BorderSize &&
			pgl.y < view.SceneHeight - view.BorderSize &&
			pgl.y > view.BorderSize)
		{
			// Get selected cell
			POINT id = view.GetCellIndex(pgl);

			if (!game.IsStart()) { game.GameStart(); }
			// Mark cells 
			game.GetModel()->Mark(id.x, id.y);
			// Check game status
			if (game.GameProcess(id.x, id.y) == GameState::PlayerWin) {
				wchar_t buff[32];
				_itow(game.GetModel()->timer, buff, 10);
				MessageBox(NULL, L"You win !", L"All bombs found", MB_OK | MB_ICONEXCLAMATION);
				MessageBox(NULL, buff, L"Total Elapsed Time", MB_OK | MB_ICONINFORMATION);
				game.EndGame();
				return 0;
			}
		}
		return 0;
	}

	case WM_COMMAND: {
		switch (wParam)
		{
		case IDC_MENU_1:
			// Execute dialog from recource
			DialogBox(NULL, MAKEINTRESOURCE(IDD_CHILDLG), view.hWnd, ChildDlgProc);
			// Default field params
			FieldWidth = FIELD_TYPE0;
			FieldHeight = FIELD_TYPE0;
			break;
		case IDC_MENU_2:
			// Statistic dialog from recource
			DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOGBAR), view.hWnd, statDlgProc);
			break;
		case IDC_MENU_3: // Exit
			view.keys[VK_ESCAPE] = true;
			break;
		default:
			break;
		}
		return 0;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK ChildDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			if (game.IsStart()) // Stop game
				game.EndGame();
			// Initialize game with default field size
			game.InitGame(FieldWidth, FieldHeight);
			EndDialog(hWnd, 0);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	case WM_INITDIALOG: {
		// Default params for game field options
		SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_STATIC_1), 0);
		SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETRANGE, 0, MAKELPARAM(fieldMaxWidth, fieldMinWidth));
		SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_SETPOS, 0, FieldWidth);
		
		SendMessage(GetDlgItem(hWnd, IDC_SPIN2), UDM_SETBUDDY, (WPARAM)GetDlgItem(hWnd, IDC_STATIC_2), 0);
		SendMessage(GetDlgItem(hWnd, IDC_SPIN2), UDM_SETRANGE, 0, MAKELPARAM(fieldMaxWidth, fieldMinWidth));
		SendMessage(GetDlgItem(hWnd, IDC_SPIN2), UDM_SETPOS, 0, FieldHeight);
	}
	break;
	case WM_VSCROLL: {
		// Get current game field param
		FieldWidth = LOWORD(SendMessage(GetDlgItem(hWnd, IDC_SPIN1), UDM_GETPOS, 0, 0));
		FieldHeight = LOWORD(SendMessage(GetDlgItem(hWnd, IDC_SPIN2), UDM_GETPOS, 0, 0));
		// Update game field params
		SetDlgItemInt(hWnd, IDC_STATIC_1, FieldWidth, 0);
		SetDlgItemInt(hWnd, IDC_STATIC_2, FieldHeight, 0);
	}
	break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK statDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hWnd, 0);
			break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}
		break;
	case WM_INITDIALOG:
	{
		HWND edit = GetDlgItem(hWnd, IDC_EDIT1);
		string str = "";
		// Gather all game stats to one string
		for (size_t i = 0; i < game.GetStat().GetRecords().size(); i++)
		{
			str += game.GetStat().GetRecords().at(i)->ToString().c_str();
			str += "\r\n";
		}
		// Append game stats
		str += game.GetStat().ToString();
		// Add text
		SetWindowTextA(edit, (LPCSTR)str.c_str());
	}
	break;
	case WM_ACTIVATE: {
		HWND edit = GetDlgItem(hWnd, IDC_EDIT1);
		// Buffer string
		string str = "";
		// Gather all game stats to one string
		for (size_t i = 0; i < game.GetStat().GetRecords().size(); i++) {
			str += game.GetStat().GetRecords().at(i)->ToString().c_str();
			str += "\r\n";
		}
		// Append game stats
		str += game.GetStat().ToString();
		// Add text
		SetWindowTextA(edit, (LPCSTR)str.c_str());
	}
	break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}