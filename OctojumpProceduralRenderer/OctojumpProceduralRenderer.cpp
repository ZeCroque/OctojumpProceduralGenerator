// OctojumProceduralRenderer.cpp : Définit le point d'entrée de l'application.
//

#include "framework.h"
#include "OctojumpProceduralRenderer.h"
#include <fstream>
#include <iostream>

#define MAX_LOADSTRING 100

using namespace std;

//Window
HWND hWnd;

//Scrolling
int xCurrentScroll;
int yCurrentScroll;
int xMaxScroll;
int yMaxScroll;
bool bScroll;
SCROLLINFO si;

//Data
int** iMap = nullptr;
int iSize;
int fGreyScaleCoeff;

void readMapFromFile(LPWSTR path)
{	
	std::ifstream input(path);

	if (input)
	{
		input >> iSize;

		iMap = new int* [iSize];
		for (int x = 0; x < iSize; ++x)
		{
			iMap[x] = new int[iSize];
		}

		for (int y = 0; y < iSize; ++y)
		{
			for (int x = 0; x < iSize; ++x)
			{
				input >> iMap[x][y];
			}
		}
		
	}
}

void getCoeff()
{
	int iMaxHeight = 0;

	for (int j = 0; j < iSize; ++j)
	{
		for (int i = 0; i < iSize; ++i)
		{
			if (iMap[i][j] > iMaxHeight)
			{
				iMaxHeight = iMap[i][j];
			}
		}
	}

	fGreyScaleCoeff = 255 / ((float)iMaxHeight);
}

void drawRoads()
{
	PAINTSTRUCT ps;
	RECT rect;

	HDC hdc = BeginPaint(hWnd, &ps);
	if (bScroll)
	{
		RECT scrollRect, clientRect;
		int gridX, gridY, xMax, yMax, y, x, i, j;

		scrollRect = ps.rcPaint;
		GetClientRect(hWnd, &clientRect);

		gridX = (scrollRect.left / 5) + (xCurrentScroll / 5);
		xMax = min(gridX + ((scrollRect.right - scrollRect.left) / 5), iSize);
		gridY = (scrollRect.top / 5) + (yCurrentScroll / 5);
		yMax = min(gridY + ((scrollRect.bottom - scrollRect.top) / 5), iSize);

		for (j = gridY, y = scrollRect.top; j < yMax; ++j, y += 5)
		{
			for (i = gridX, x = scrollRect.left; i < xMax; ++i, x += 5)
			{
				int currentValue = iMap[i][j];
				if (currentValue)
				{
					int greyScale = 255 - currentValue * fGreyScaleCoeff;
					HBRUSH brush = CreateSolidBrush(RGB(greyScale, greyScale, greyScale));
					rect = { x, y, x + 5, y + 5 };
					FillRect(hdc, &rect, brush);
					DeleteObject(brush);
				}
			}
		}

		bScroll = false;
	}
	else
	{
		for (int j = 0; j < iSize; ++j)
		{
			for (int i = 0; i < iSize; ++i)
			{
				int currentValue = iMap[i][j];
				if (currentValue)
				{
					int greyScale = 255 - currentValue * fGreyScaleCoeff;
					HBRUSH brush = CreateSolidBrush(RGB(greyScale, greyScale, greyScale));
					rect = { i * 5, j * 5, (i + 1) * 5, (j + 1) * 5 };
					FillRect(hdc, &rect, brush);
					DeleteObject(brush);
				}
			}
		}
	}

	EndPaint(hWnd, &ps);

}

int calculateScroll(int& pos, int max, WPARAM wParam)
{
	int newPos;    // new position
	int delta;

	switch (LOWORD(wParam))
	{
		// User clicked the scroll bar shaft above the scroll box. 
	case SB_PAGEUP:
		newPos = pos - 50;
		break;

		// User clicked the scroll bar shaft below the scroll box. 
	case SB_PAGEDOWN:
		newPos = pos + 50;
		break;

		// User clicked the top arrow. 
	case SB_LINEUP:
		newPos = pos - 5;
		break;

		// User clicked the bottom arrow. 
	case SB_LINEDOWN:
		newPos = pos + 5;
		break;

		// User dragged the scroll box. 
	case SB_THUMBPOSITION:
		newPos = HIWORD(wParam) - HIWORD(wParam) % 5;
		break;

	default:
		newPos = pos;
	}

	// New position must be between 0 and the screen height. 
	newPos = max(0, newPos);
	newPos = min(max, newPos);

	//Calculate delta
	delta = newPos - pos;

	// Reset the current scroll position. 
	pos = newPos;

	return -delta;
}


//Callback for messageHandlerThread
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CREATE:
		{
			xCurrentScroll = 0;
			yCurrentScroll = 0;
			bScroll = true;

			break;
		}
		case WM_SIZE:
		{
			xMaxScroll = max(iSize * 5 - (LOWORD(lParam) - LOWORD(lParam) % 5), 0);
			yMaxScroll = max(iSize * 5 - (HIWORD(lParam) - HIWORD(lParam) % 5), 0);

			si.cbSize = sizeof(si);
			si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
			si.nMin = 0;
			si.nPage = 5;

			si.nPos = xCurrentScroll;
			si.nMax = xMaxScroll;
			SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);

			if (si.nMax)
			{
				bScroll = false;
			}

			si.nPos = yCurrentScroll;
			si.nMax = yMaxScroll;
			SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

			if (si.nMax)
			{
				bScroll = true;
			}

			break;
		}
		case WM_HSCROLL:
		{
			int delta = calculateScroll(xCurrentScroll, xMaxScroll, wParam);

			if (delta != 0)
			{
				bScroll = true;

				// Scroll the window.
				ScrollWindowEx(hWnd, delta, 0, nullptr, nullptr, nullptr, nullptr, SW_INVALIDATE | SW_ERASE);
				UpdateWindow(hWnd);

				// Reset the scroll bar. 
				si.cbSize = sizeof(si);
				si.fMask = SIF_POS;
				si.nPos = xCurrentScroll;
				SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
			}
			break;
		}
		case WM_VSCROLL:
		{
			int delta = calculateScroll(yCurrentScroll, yMaxScroll, wParam);

			if (delta != 0)
			{
				bScroll = true;

				// Scroll the window.
				ScrollWindowEx(hWnd, 0, delta, nullptr, nullptr, nullptr, nullptr, SW_INVALIDATE | SW_ERASE);
				UpdateWindow(hWnd);

				// Reset the scroll bar. 
				si.cbSize = sizeof(si);
				si.fMask = SIF_POS;
				si.nPos = yCurrentScroll;
				SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
			}
			break;
		}
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Analyse les sélections de menu :
			if (wmId == IDM_EXIT)
			{
				DestroyWindow(hWnd);
			}
			else
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
			drawRoads();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,  _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	//Variables
	MSG msg;
	HACCEL hAccelTable;
	WCHAR szTitle[MAX_LOADSTRING];
	WCHAR szWindowClass[MAX_LOADSTRING];
	WNDCLASSEXW wcex;

	//Window params
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_OCTOJUMPPROCEDURALRENDERER, szWindowClass, MAX_LOADSTRING);
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OCTOJUMPPROCEDURALRENDERER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_OCTOJUMPPROCEDURALRENDERER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	readMapFromFile(lpCmdLine);
	getCoeff();

	//Creating window
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return false;
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);


	// Main message loop
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_OCTOJUMPPROCEDURALRENDERER);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
