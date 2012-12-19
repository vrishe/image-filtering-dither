// ImageFilterTest.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "ImageFilterTest.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGEFILTERTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	til::TIL_Init();

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMAGEFILTERTEST));
	
	// Цикл основного сообщения:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	til::TIL_ShutDown();

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
//  КОММЕНТАРИИ:
//
//    Эта функция и ее использование необходимы только в случае, если нужно, чтобы данный код
//    был совместим с системами Win32, не имеющими функции RegisterClassEx'
//    которая была добавлена в Windows 95. Вызов этой функции важен для того,
//    чтобы приложение получило "качественные" мелкие значки и установило связь
//    с ними.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGEFILTERTEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_IMAGEFILTERTEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной

   hWnd = CreateWindow(szWindowClass, szTitle, WS_BORDER | WS_SYSMENU,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_COMMAND	- обработка меню приложения
//  WM_PAINT	-Закрасить главное окно
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
//

til::Image *ref_image = NULL;
til::Image *test_image = NULL;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;


	switch (message)
	{
	case WM_CREATE:
		{
			test_image = til::TIL_Load("test_image.bmp", TIL_FILE_ADDWORKINGDIR | TIL_DEPTH_R8G8B8);
			ref_image = til::TIL_Load("test_image.bmp", TIL_FILE_ADDWORKINGDIR | TIL_DEPTH_R8G8B8);
			if (test_image == NULL || ref_image == NULL)
			{
				MessageBox(hWnd, _T("Failed to load image specified!"), _T("Fuck"), MB_OK);
				return -1;
			}

			RECT clientRect, windowRect;
			GetClientRect(hWnd, &clientRect); 
			GetWindowRect(hWnd, &windowRect);
			SetWindowPos(hWnd, NULL, 0, 0, (windowRect.right - windowRect.left) - (clientRect.right - clientRect.left) + test_image->GetWidth(),
				(windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top) + test_image->GetHeight(), SWP_NOMOVE | SWP_NOZORDER);

		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
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
			ipo::IImageFilter *filter = new ipo::RGBLinearQuantizationFilter(RGB_QUANTS(8,8,4));
			filter->Apply(*test_image);
			delete filter;

			/*ipo::IImageFilter *filter = new ipo::RGBPriorityQuantizationFilter(256);
			filter->Apply(*test_image);
			delete filter;*/

			//float distrib_pattern[8] = {0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125};
			float distrib_pattern[8] = {0, 0, 0, 7.0 / 16.0, 1.0 / 16.0, 5.0 / 16.0, 3.0 / 16.0, 0};
			ipo::IImageFilter *dither = new ipo::UniformDistributionDither(*ref_image, distrib_pattern, DITH_MOVE_HORIZONTAL | DITH_STARTPOINT_REMAIN);
			dither->Apply(*test_image);
			delete dither;

			ipo::ImageTransformationAnalysis::FindStandardDeviation(*ref_image, *test_image);

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			HDC hMemDc		= CreateCompatibleDC(NULL);
			HBITMAP hBmp	= CreateCompatibleBitmap(hdc, test_image->GetWidth(), test_image->GetHeight());

			SetBitmapBits(hBmp, test_image->GetWidth() * test_image->GetHeight() * 4, test_image->GetPixels());

			HBITMAP hBmpOld = static_cast<HBITMAP>(SelectObject(hMemDc, hBmp));
			BitBlt(hdc, 0, 0, test_image->GetWidth(), test_image->GetHeight(), hMemDc, 0, 0, SRCCOPY);
			SelectObject(hMemDc, hBmpOld);

			DeleteObject(hBmp);
			DeleteDC(hMemDc);

			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		til::TIL_Release(test_image);
		til::TIL_Release(ref_image);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Обработчик сообщений для окна "О программе".
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
