// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

//  ===========================================================================
//  File    Splash.cpp
//  Desc    The implementation file for the CSplash class.
//  ===========================================================================
#include "stdafx.h"
#include "splash.h"
#include "windowsx.h"

//  ===========================================================================
//  The following is used for layering support which is used in the 
//  splash screen for transparency. In VC 6 these are not defined in the headers
//  for user32.dll and hence we use mechanisms so that it can work in VC 6.
//  We define the flags here and write code so that we can load the function
//  from User32.dll explicitely and use it. This code requires Win2k and above
//  to work.
//  ===========================================================================
typedef BOOL(WINAPI *lpfnSetLayeredWindowAttributes)
(HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);

lpfnSetLayeredWindowAttributes g_pSetLayeredWindowAttributes;

#define WS_EX_LAYERED 0x00080000 
//Only VC++ 6 need this
//#define LWA_COLORKEY 1 // Use color as the transparency color.
//#define LWA_ALPHA    2 // Use bAlpha to determine the opacity of the layer

//  ===========================================================================
//  Func    ExtWndProc
//  Desc    The windows procedure that is used to forward messages to the 
//          CSplash class. CSplash sends the "this" pointer through the
//          CreateWindowEx call and the pointer reaches here in the 
//          WM_CREATE message. We store it here and use it for message 
//          forwarding.
//  ===========================================================================
static LRESULT CALLBACK ExtWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static CSplash * spl = NULL;
	if (uMsg == WM_CREATE)
	{
		spl = (CSplash*)((LPCREATESTRUCT)lParam)->lpCreateParams;
	}
	if (spl)
		return spl->WindowProc(hwnd, uMsg, wParam, lParam);
	else
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CSplash::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//  We need to handle on the WM_PAINT message
	switch (uMsg)
	{
		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CSplash::OnPaint(HWND hwnd)
{
	if (!m_hBitmap)
		return;

	//  =======================================================================
	//  Paint the background by BitBlting the bitmap
	//  =======================================================================
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hwnd, &ps);

	RECT   rect;
	::GetClientRect(m_hwnd, &rect);

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, m_hBitmap);

	BitBlt(hDC, 0, 0, m_dwWidth, m_dwHeight, hMemDC, 0, 0, SRCCOPY);

	::SelectObject(hMemDC, hOldBmp);

	::DeleteDC(hMemDC);

	EndPaint(hwnd, &ps);
}

void CSplash::Init()
{
	//  =======================================================================
	//  Initialize the variables
	//  =======================================================================
	m_hwnd = NULL;
	m_lpszClassName = TEXT("SPLASH");
	m_colTrans = 0;

	//  =======================================================================
	//  Keep the function pointer for the SetLayeredWindowAttributes function
	//  in User32.dll ready
	//  =======================================================================
	HMODULE hUser32 = GetModuleHandle(TEXT("USER32.DLL"));

	g_pSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)
		GetProcAddress(hUser32, "SetLayeredWindowAttributes");
}

CSplash::CSplash()
{
	Init();
}

CSplash::CSplash(int resourceID, COLORREF colTrans) //addon alin1337
{
	Init();

	SetBitmap(LoadBitmap(GetModuleHandle("LM2Hack.dll"),MAKEINTRESOURCE(resourceID)));
	SetTransparentColor(colTrans);
}

CSplash::~CSplash()
{
	FreeResources();
}

HWND CSplash::RegAndCreateWindow()
{
	//  =======================================================================
	//  Register the window with ExtWndProc as the window procedure
	//  =======================================================================
	WNDCLASSEX wndclass;
	wndclass.cbSize = sizeof (wndclass);
	wndclass.style = CS_BYTEALIGNCLIENT | CS_BYTEALIGNWINDOW;
	wndclass.lpfnWndProc = ExtWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;
	wndclass.hInstance = ::GetModuleHandle(NULL);
	wndclass.hIcon = NULL;
	wndclass.hCursor = ::LoadCursor(NULL, IDC_WAIT);
	wndclass.hbrBackground = (HBRUSH)::GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = m_lpszClassName;
	wndclass.hIconSm = NULL;

	if (!RegisterClassEx(&wndclass))
		return NULL;

	//  =======================================================================
	//  Create the window of the application, passing the this pointer so that
	//  ExtWndProc can use that for message forwarding
	//  =======================================================================
	DWORD nScrWidth = ::GetSystemMetrics(SM_CXFULLSCREEN);
	DWORD nScrHeight = ::GetSystemMetrics(SM_CYFULLSCREEN);

	int x = (nScrWidth - m_dwWidth) / 2;
	int y = (nScrHeight - m_dwHeight) / 2;
	m_hwnd = ::CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, m_lpszClassName,
		TEXT("Banner"), WS_POPUP, x, y,
		m_dwWidth, m_dwHeight, NULL, NULL, NULL, this);

	//  =======================================================================
	//  Display the window
	//  =======================================================================
	if (m_hwnd)
	{
		MakeTransparent();
		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);
	}
	return m_hwnd;
}

int CSplash::DoLoop()
{
	//  =======================================================================
	//  Show the window
	//  =======================================================================
	if (!m_hwnd)
		ShowSplash();

	//  =======================================================================
	//  Get into the modal loop
	//  =======================================================================
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}

void CSplash::ShowSplash()
{
	CloseSplash();
	RegAndCreateWindow();
}


DWORD CSplash::SetBitmap(HBITMAP hBitmap)
{
	int nRetValue;
	BITMAP  csBitmapSize;

	//  =======================================================================
	//  Free loaded resource
	//  =======================================================================
	FreeResources();

	if (hBitmap)
	{
		m_hBitmap = hBitmap;

		//  ===================================================================
		//  Get bitmap size
		//  ===================================================================
		nRetValue = ::GetObject(hBitmap, sizeof(csBitmapSize), &csBitmapSize);
		if (nRetValue == 0)
		{
			FreeResources();
			return 0;
		}
		m_dwWidth = (DWORD)csBitmapSize.bmWidth;
		m_dwHeight = (DWORD)csBitmapSize.bmHeight;
	}

	return 1;
}

void CSplash::FreeResources()
{
	if (m_hBitmap)
		::DeleteObject(m_hBitmap);
	m_hBitmap = NULL;
}

int CSplash::CloseSplash()
{

	if (m_hwnd)
	{
		DestroyWindow(m_hwnd);
		m_hwnd = 0;
		UnregisterClass(m_lpszClassName, ::GetModuleHandle(NULL));
		return 1;
	}
	return 0;
}

bool CSplash::SetTransparentColor(COLORREF col)
{
	m_colTrans = col;

	return MakeTransparent();
}

bool CSplash::MakeTransparent()
{
	//  =======================================================================
	//  Set the layered window style and make the required color transparent
	//  =======================================================================
	if (m_hwnd && g_pSetLayeredWindowAttributes && m_colTrans)
	{
		//  set layered style for the window
		SetWindowLong(m_hwnd, GWL_EXSTYLE, GetWindowLong(m_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		//  call it with 0 alpha for the given color
		g_pSetLayeredWindowAttributes(m_hwnd, m_colTrans, 0, LWA_COLORKEY);
	}
	return TRUE;
}

/*

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SPLASH::SPLASH()
{

}

SPLASH::~SPLASH()
{
	DestroyWindow(hSplashWnd);
}

void SPLASH::Init(HWND hWnd, HINSTANCE hInst, int resid)
{
	hParentWindow = hWnd;
	hSplashWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, "STATIC", "",
		WS_POPUP | SS_BITMAP, 300, 300, 300, 300, NULL, NULL, NULL, NULL);
	
	SendMessage(hSplashWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoadBitmap(hInst, MAKEINTRESOURCE(resid)));
	this->SHOWING = FALSE;
}

void SPLASH::Show()
{
	//get size of hSplashWnd (width and height)
	int x, y;
	int pwidth, pheight;
	int tx, ty;
	HDWP windefer;
	RECT rect, prect;
	GetClientRect(hSplashWnd, &rect);
	x = rect.right; y = rect.bottom;
	//get parent screen coordinates
	GetWindowRect(this->hParentWindow, &prect);

	//center splash window on parent window
	pwidth = prect.right - prect.left;
	pheight = prect.bottom - prect.top;



	tx = (pwidth / 2) - (x / 2);
	ty = (pheight / 2) - (y / 2);

	tx += prect.left;
	ty += prect.top;


	windefer = BeginDeferWindowPos(1);
	DeferWindowPos(windefer, hSplashWnd, HWND_NOTOPMOST, tx, ty, 50, 50, SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOZORDER);
	EndDeferWindowPos(windefer);

	ShowWindow(hSplashWnd, SW_SHOWNORMAL);
	UpdateWindow(hSplashWnd);
	this->SHOWING = TRUE;
}

void SPLASH::Hide()
{
	ShowWindow(hSplashWnd, SW_HIDE);
	this->SHOWING = FALSE;
}

*/