#include "Defines.h"


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

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
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