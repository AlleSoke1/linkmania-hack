#include <WinSock2.h>
#include "detours.h"
#include <string.h>
#include <stdlib.h>
#include "g_Console.h"

using namespace std;

#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define MAKE_NUMBERW(x,y)  ( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

SOCKET gs_socket;
int gsConnect = 0;
int autokill = 0;
typedef unsigned char       BYTE;
int maxhitcount = 50;
BYTE teleID[2];
BYTE TELEcoord[2] = { 130, 130 };
int hithackCount = 0;
BYTE addr = 0;

//MobList VECTOR
//std::vector<int> mobList;

/* Cateva offseturi */
#define Version	0xE61F68 //-> 22749
#define Serial	0xE61F70 //-> k1Pk2jcET48mxL3b
#define MapNumber	*(int*)0xE61E18
#define MainState	*(int*)0xE609E8
#define UserObjectStruct	0x8128AC8
#define ObjectPreviewStruct	0x7BC4F04
#define MasterLevel	*(short*)0x87935D8
#define MasterPoints	*(short*)0x87935F2
#define CursorX *(int*)0x879340C
#define CursorY *(int*)0x8793410
#define MaxZenWidth1	*(BYTE*)0x7AF026
#define MaxZenWidth2	*(BYTE*)0x7AF2C6
#define MaxZenWidth3	*(BYTE*)0x7AF646
#define MaxZenWidth4	*(BYTE*)0x7B01C6
#define MaxZenWidth5	*(BYTE*)0x1 //-> If 0x0 or 0x1 = not in use
#define WinWidth	*(GLsizei*)0xE61E58
#define WinHeight	*(GLsizei*)0xE61E5C

#define ID_AUTOKILL 1
#define ID_AUTOKILLOFF 2
#define ID_HITCOUNT 3
#define ID_TELETEST 4
#define ID_TELETESTX 5
#define ID_TELETESTY 6
#define ID_DEC 7
#define ID_BUFF 8

/* Get Address */
DWORD GetAddress(DWORD addie, LPCSTR module)

{

	if (GetModuleHandle(module) != NULL)

	{

		DWORD addie1 = (DWORD)GetModuleHandle(module);

		DWORD addie2 = addie;

		DWORD address = addie1 + addie2;

		return address;

	}

	else

		return NULL;

}

/* Write MEMORY */
DWORD WriteMemory(const LPVOID lpAddress, const LPVOID lpBuf, const UINT uSize)
{
	DWORD dwErrorCode = 0;
	DWORD dwOldProtect = 0;
	// ----
	int iRes = VirtualProtect(lpAddress, uSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	memcpy(lpAddress, lpBuf, uSize);
	// ----
	DWORD dwBytes = 0;
	// ----
	iRes = VirtualProtect(lpAddress, uSize, dwOldProtect, &dwBytes);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	return 0x00;
}
/* Read MEMORY */
DWORD ReadMemory(const LPVOID lpAddress, LPVOID lpBuf, const UINT uSize)
{
	DWORD dwErrorCode = 0;
	DWORD dwOldProtect = 0;
	// ----
	int iRes = VirtualProtect(lpAddress, uSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	memcpy(lpBuf, lpAddress, uSize);
	// ----
	DWORD dwBytes = 0;
	// ----
	iRes = VirtualProtect(lpAddress, uSize, dwOldProtect, &dwBytes);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	return 0x00;
}
/* Set BYTE */
DWORD SetByte(const LPVOID dwOffset, const BYTE btValue)
{
	return WriteMemory(dwOffset, (LPVOID)& btValue, sizeof(BYTE));
}
/* Get BYTE */
DWORD GetByte(const LPVOID dwOffset, BYTE & btValue)
{
	return ReadMemory(dwOffset, (LPVOID)btValue, sizeof(BYTE));
}

// -------------------------------------------------------------------
// Functie pentru detour...
void DetourAndErrorCheck(PVOID* pFunctionToDetour, PVOID pReplacement, const char* functionName)
{
	char* debugString = new char[256];
	ZeroMemory(debugString, 256);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(pFunctionToDetour, pReplacement);
	int errorCode = DetourTransactionCommit();
	if (errorCode == NO_ERROR)
	{
#ifdef DEBUG
		FILE* FileHwnd1;
		FileHwnd1 = fopen("LinkManiaHackERROR.log", "a+");
		fprintf(FileHwnd1, "------------ LOG --------------- \n");
		fprintf(FileHwnd1, "Address of %s: 0x%x\n", functionName);
		fprintf(FileHwnd1, "Address of replacement function: 0x%x\n", pFunctionToDetour);
		fprintf(FileHwnd1, "%s() was detoured successfully \n", pReplacement, functionName);
		fclose(FileHwnd1);
		OutputDebugString(debugString);
#endif
	}
	else
	{
		if (errorCode == ERROR_INVALID_DATA)
			MessageBoxA(NULL, "ERROR_INVALID_DATA returned", "ERROR", MB_OK);
		else if (errorCode == ERROR_INVALID_OPERATION)
			MessageBoxA(NULL, "ERROR_INVALID_OPERATION returned", "ERROR", MB_OK);
		else
#ifdef DEBUG
			MessageBoxA(NULL, "Other error code returned", "ERROR", MB_OK);
		sprintf_s(debugString, 255, "%s() was not able to be detoured\n"
			"Address of %s: 0x%x\n"
			"Address of replacement function: 0x%x", functionName,
			functionName, pFunctionToDetour, pReplacement);
		FILE* FileHwnd1;
		FileHwnd1 = fopen("LinkManiaHackERROR.log", "a+");
		fprintf(FileHwnd1, "------------ LOG --------------- \n");
		fprintf(FileHwnd1, "%s() was not able to be detoured \n", functionName);
		fprintf(FileHwnd1, "Address of %s: 0x%x \n", functionName);
		fprintf(FileHwnd1, "Address of replacement function: 0x%x \n", pFunctionToDetour, pReplacement);
		fclose(FileHwnd1);

#endif
		OutputDebugString(debugString);
	}
	delete[] debugString;
}

// -------------------------------------------------------------------
// Functie pentru detour... gasire functie
void CheckAndDetach(PVOID* pDetouredFunction, PVOID pReplacement, const char* functionName)
{
	if (pDetouredFunction != NULL)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)pDetouredFunction, pReplacement);
		DetourTransactionCommit();
	}
	else
	{
		int errorSize = 64 + strlen(functionName);
		char* errorString = new char[errorSize];
		ZeroMemory(errorString, 32);
		MessageBoxA(NULL, " could not be detached\n Was it detoured in the first place?", "ERROR", MB_OK);
		OutputDebugString(errorString);
		delete[] errorString;
	}
}

int (WINAPI *psend2)(SOCKET socket, BYTE* buffer, int length, int flags) = NULL;
/* Send Packet bitch! :) */
void sendpacket(SOCKET s, BYTE* buf, int len, int flags)
{
	/*BYTE Packet[50];
	for (int i = 0; i < len; i++){
	if (buf[i] != 0x00) {
	Packet[i] = buf[i];
	}
	}*/

	/*	cout << "SEND ->";

	for (int i = 0; i < len; i++){
	printf(" %.2X ", Packet[i]);
	}
	printf("\n");
	*///g_Console.ConsoleOutput(1, "Packet sent: 0x%02X", (BYTE)Packet);

	psend2(s, buf, len, flags);
}

//need to be moved



int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {

	MSG  msg;
	WNDCLASSW wc = { 0 };

	HWND hDialog = 0;



	wc.lpszClassName = L"otokill";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClassW(&wc);

	CreateWindowW(wc.lpszClassName, L"[ LinkMania 哈克 ]",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 400, 400, 300, 0, 0, hInstance, 0);

	//	DialogBox(hInstance, MAKEINTRESOURCE(IDD_FORMVIEW),
	//		hDialog, reinterpret_cast<DLGPROC>(DialogProc));

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
BOOL CALLBACK DialogProc(HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	//static Controller* control = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		/*try
		{
		control = new Controller(hwnd);
		}
		catch (WinException e)
		{
		MessageBox(0, e.GetMessage(), "Exception",
		MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(1);
		}
		catch (...)
		{
		MessageBox(0, "Unknown", "Exception",
		MB_ICONEXCLAMATION | MB_OK);
		PostQuitMessage(2);
		}*/
		return TRUE;
	case WM_COMMAND:
		//control->Command(hwnd, LOWORD(wParam), HIWORD(wParam));
		return TRUE;
	case WM_HSCROLL:
		//control->Scroll(hwnd, LOWORD(wParam), HIWORD(wParam));
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	case WM_CLOSE:
		//delete control;
		DestroyWindow(hwnd);
		return TRUE;
	}
	return FALSE;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {
	static HWND hwndEdit, TeleX, TeleY;
	switch (msg) {

	case WM_CREATE:
		hwndEdit = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			50, 20, 150, 20, hwnd, (HMENU)ID_HITCOUNT,
			NULL, NULL);

		TeleX = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			75, 85, 30, 20, hwnd, (HMENU)ID_TELETESTX,
			NULL, NULL);

		TeleY = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			110, 85, 30, 20, hwnd, (HMENU)ID_TELETESTY,
			NULL, NULL);

		CreateWindowW(L"button", L"HITHACK ON",
			WS_VISIBLE | WS_CHILD,
			20, 50, 80, 25,
			hwnd, (HMENU)ID_AUTOKILL, NULL, NULL);

		CreateWindowW(L"button", L"HITHACK OFF",
			WS_VISIBLE | WS_CHILD,
			120, 50, 80, 25,
			hwnd, (HMENU)ID_AUTOKILLOFF, NULL, NULL);

		CreateWindowW(L"button", L"TELEPORT",
			WS_VISIBLE | WS_CHILD,
			60, 110, 80, 25,
			hwnd, (HMENU)ID_TELETEST, NULL, NULL);

		CreateWindowW(L"button", L"BUFF PLS",
			WS_VISIBLE | WS_CHILD,
			60, 150, 80, 25,
			hwnd, (HMENU)ID_BUFF, NULL, NULL);

		CreateWindowW(L"button", L"DECRYPT",
			WS_VISIBLE | WS_CHILD,
			60, 180, 80, 25,
			hwnd, (HMENU)ID_DEC, NULL, NULL);
		break;

	case WM_COMMAND:

		if (LOWORD(wParam) == ID_AUTOKILL) {
			//MessageBeep(MB_OK);'
			g_Console.ConsoleOutput(3, "[Hithack] ON! :)");

			int len = GetWindowTextLengthW(hwndEdit) + 1;

			wchar_t text[5];
			GetWindowTextW(hwndEdit, text, len);
			//
			char buffer[32];
			wcstombs(buffer, text, sizeof(buffer));

			hithackCount = atoi(buffer);

			if (hithackCount > maxhitcount){

				g_Console.ConsoleOutput(1, "Esti prost? Max e 50 ba!");
				autokill = 0;
				return false;
			}
			else{
				g_Console.ConsoleOutput(4, "[Hithack] HITCOUNT = %d", hithackCount);
				autokill = 1;
			}
		}

		if (LOWORD(wParam) == ID_AUTOKILLOFF) {
			//PostQuitMessage(0);
			g_Console.ConsoleOutput(4, "[Hithack] Off! :(");
			autokill = 0;
		}

		if (LOWORD(wParam) == ID_TELETEST) {
			//PostQuitMessage(0);
			//g_Console.ConsoleOutput(1, "[TELETEST] ! :(");

			//getX
			int lenX = GetWindowTextLengthW(TeleX) + 1;

			wchar_t textX[5];
			GetWindowTextW(TeleX, textX, lenX);
			char bufferX[32];
			wcstombs(bufferX, textX, sizeof(bufferX));
			TELEcoord[0] = atoi(bufferX);


			//GetY
			int lenY = GetWindowTextLengthW(TeleY) + 1;

			wchar_t textY[5];
			GetWindowTextW(TeleY, textY, lenY);
			char bufferY[32];
			wcstombs(bufferY, textY, sizeof(bufferY));
			TELEcoord[1] = atoi(bufferY);

			BYTE MecanikXOR[] = { 0x3A, 0x4E, 0x10, 0xCF, 0xAD, 0x5F, 0xD8, 0xC2, 0xDF, 0x20, 0x02, 0xE4, 0xED, 0x46, 0x42, 0xEB, 0x6E, 0x2D, 0x87, 0xF0, 0xCE, 0xA7, 0x53, 0x21, 0xF6, 0xE7, 0xE1, 0x83, 0x1A, 0x88, 0x6F, 0xDF };

			unsigned char STC[] = { 0xC1, 0x08, 0xD4, teleID[0], teleID[1], TELEcoord[0], TELEcoord[1], 0x10 }; //  ? 69// hit ar trebuii sa fie ?
			unsigned char CTS[] = { 0xC1, 0x06, 0xD4, TELEcoord[0], TELEcoord[1], 0x10 }; //HEK


			SendMagicPacket(CTS, CTS[1]);
			recvpacket(gs_socket, STC, sizeof(STC), 0);

			Sleep(200);
			SendMagicPacket(CTS, CTS[1]);
			//	sendpacket(gs_socket, CTS, sizeof(CTS), 0);
			recvpacket(gs_socket, STC, sizeof(STC), 0);

			g_Console.ConsoleOutput(2, "[TELEPORT] TO x= %d y= %d  ", TELEcoord[0], TELEcoord[1]);
		}
		if (LOWORD(wParam) == ID_BUFF) {
			BYTE BuffPacket[] = { 0xC1, 0x04, 0xF6, 0xFD };

			sendpacket(gs_socket, BuffPacket, sizeof(BuffPacket), 0);
			g_Console.ConsoleOutput(2, "[BUFFED]");
		}


		if (LOWORD(wParam) == ID_DEC) {
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			/* Special */
#define oXORFilterStart			0x00404C99
			printf("[Hacker-XOR]");
			for (int i = 0; i < 32; i++)
			{
				BYTE *a = (BYTE*)(oXORFilterStart + 4 * i) + 3;
				printf(" %.2X ", *(a));
			}

			BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };
			unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0x23, 0x93, 0x33, 0x69 };
			for (int i = buf1[1] - 1; i != 2; i += -1)
			{
				buf1[i] ^= buf1[i - 1] ^ cheimagice[i % 32];
			}

			printf("\n\nDECRYPT PACKET -> ");
			for (int i = 0; i < buf1[1]; i++)
			{
				printf(" %.2X ", buf1[i]);
			}

		}

		break;

	case WM_DESTROY:

		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}