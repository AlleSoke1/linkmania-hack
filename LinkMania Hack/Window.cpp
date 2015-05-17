#include "Defines.h"

#include <windows.h>  //include all the basics
#include <windowsx.h>
#include <tchar.h>    //string and other mapping macros
#include <string.h>
#include <string>
#include <vector>
#include "resource.h"



//MOVE !!
void AddLog(const HWND &hwnd, char * Mesaj, ...);
void AddChatLog(const HWND &hwnd, char * Mesaj, ...);
char *GetChatType(int index);

//globalVals 
int ChatType = 0;


//define an unicode string type alias
typedef std::basic_string<TCHAR> ustring;
//=============================================================================
//message processing function declarations
INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnCommand(const HWND, int, int, const HWND);
INT_PTR OnInitDlg(const HWND, LPARAM);

//non-message function declarations
inline int ErrMsg(const ustring&);
//=============================================================================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	INT_PTR success = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);

	if (success == -1)
	{
		ErrMsg(_T("DialogBox failed."));
	}
	return 0;
}
//=============================================================================
INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
	{
					   OnCommand(hwnd, LOWORD(wParam), HIWORD(wParam),
						   reinterpret_cast<HWND>(lParam));
					   return 0;
	}
	case WM_INITDIALOG:
	{
						  //Add items to chat list 
						  HWND comboBox = GetDlgItem(hwnd, IDC_COMBOAA);;
						  SendMessage(comboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Normal")));
						  SendMessage(comboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Guild")));
						  SendMessage(comboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Alliance")));
						  SendMessage(comboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Gens")));
						  SendMessage(comboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("ALL!!")));
						  SendMessage(comboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("ADMIN!")));

						  //Init dialog :)
						  return OnInitDlg(hwnd, lParam);
	}
	default:
		return FALSE;  //let system deal with msg
	}
}
//=============================================================================
void OnCommand(const HWND hwnd, int id, int notifycode, const HWND hCntrl)
{
	//handles WM_COMMAND message of the modal dialogbox
	//char buf[128];
	//wsprintf(buf, "HIT res %d", id);
	//MessageBoxA(NULL, buf, "OK", MB_OK);


switch (id)
{
	/* TALK !! */
	case IDC_SENDCHAT:{
	HWND HWNDChatText = GetDlgItem(hwnd, IDC_CHATTEXT);
	HWND HWNDChatType = GetDlgItem(hwnd, IDC_COMBOAA);
	HWND HWNDChatSender = GetDlgItem(hwnd, IDC_SENDAS);

	char buf[128];
	char ChatText[89];
	GetWindowText(HWNDChatText, ChatText, 89);
	ChatType = ComboBox_GetCurSel(HWNDChatType);

	char SendAs[11];
	GetWindowText(HWNDChatSender, SendAs, 11);

		wsprintf(buf, "[%s] %s\r\n", GetChatType(ChatType), ChatText);
		if (ChatType == 0)
		{
			PMSG_CHATDATA packet;
			packet.h.c = 0xC1;
			packet.h.headcode = 0x00;
			packet.h.size = sizeof(packet);

			memcpy(packet.chatid, SendAs, strlen(SendAs));
			memcpy(packet.chatmsg, ChatText, sizeof(ChatText));

			SendMagicPacket((LPBYTE)&packet, packet.h.size);
		}
		else if (ChatType == 1)
		{
			PMSG_CHATDATA packet;
			packet.h.c = 0xC1;
			packet.h.headcode = 0x00;
			packet.h.size = sizeof(packet);

			memcpy(packet.chatid, SendAs, strlen(SendAs));
			memcpy(packet.chatmsg, ChatText, sizeof(ChatText));

			SendMagicPacket((LPBYTE)&packet, packet.h.size);
		}
		else if (ChatType == 5)
		{
			PMSG_NOTICE pNotice;
			pNotice.h.c = 0xC1;
			pNotice.h.headcode = 0x0D;
			pNotice.type = 0x00;
			memcpy(pNotice.Notice, ChatText, sizeof(ChatText));

			pNotice.h.size = sizeof(pNotice);

			BYTE * Data = EncodeChatPacket((BYTE*)&pNotice);

			recvpacket(gs_socket, (BYTE*)&Data, pNotice.h.size, 0);
			
			//PHeadSetB((LPBYTE)pNotice, 0x0D, strlen(pNotice->Notice) + sizeof(PMSG_NOTICE)-sizeof(pNotice->Notice) + 1);
		}




		AddChatLog(hwnd, buf);
	}
		break;

	/* HIT HACK */
	case IDC_STARTHITHACK:
	{
		HWND HWNDhitCount = GetDlgItem(hwnd, IDC_HITCOUNT);
		char buf[128];
		char hitcout[3];
		GetWindowText(HWNDhitCount, hitcout, 3);

		if (hithack == 0){
			wsprintf(buf, "HITHACK ON! COUNT = %d\r\n", atoi(hitcout));
			hithack = 1;
			hithackCount = atoi(hitcout);
		}
		else{
			wsprintf(buf, "HITHACK Off!\r\n");
			hithack = 0;
		}

		AddLog(hwnd, buf);
	}
	break;

	/* AUTOKILL */
	case IDC_STARTAUTOKILL:
	{
            if (SendDlgItemMessage(hwnd, IDC_AUTOKILLHOVER, BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				autokillOnHover = 1;

				AddLog(hwnd, "AUTOKILL HOVER ON\r\n");
			}

			HWND HWNDAKhitCount = GetDlgItem(hwnd, IDC_AUTOKILLNOATK);
			char buf[128];
			char hitcout[3];
			GetWindowText(HWNDAKhitCount, hitcout, 3);

			if (autokill == 0){
				wsprintf(buf, "AUTOKILL ON! COUNT = %d\r\n", atoi(hitcout));
				autokill = 1;
				autokillHitCount = atoi(hitcout);
			}
			else{
				wsprintf(buf, "AUTOKILL Off!\r\n");
				autokill = 0;
				autokillOnHover = 0;
			}
			AddLog(hwnd, buf);
	}
	break;

	/* TELEPORT */
	case IDC_TELEPORT:
	{
		//Get X
		HWND HWND_Xcoord = GetDlgItem(hwnd, IDC_TELEPORTX);
		char Xcoord[4];
		GetWindowText(HWND_Xcoord, Xcoord, 4);
		
		//Get Y
		HWND HWND_Ycoord = GetDlgItem(hwnd, IDC_TELEPORTY);
		char Ycoord[4];
		GetWindowText(HWND_Ycoord, Ycoord, 4);

		BYTE X = atoi(Xcoord);
		BYTE Y = atoi(Ycoord);

		unsigned char STC[] = { 0xC1, 0x08, 0xD4, SET_NUMBERH(PlayerIndex), SET_NUMBERL(PlayerIndex), X, Y, 0x10 }; //  ? 69// hit ar trebuii sa fie ?
		unsigned char CTS[] = { 0xC1, 0x06, 0xD4, X, Y, 0x10 }; //HEK
		

		SendMagicPacket(CTS, CTS[1]);
		recvpacket(gs_socket, STC, sizeof(STC), 0);

		Sleep(200);
		
		SendMagicPacket(CTS, CTS[1]);
		recvpacket(gs_socket, STC, sizeof(STC), 0);

		char buf[128];
		wsprintf(buf,"TELEPORT -> X = %d  Y = %d\r\n",X,Y);
		AddLog(hwnd, buf);
	}
	break;

	case IDC_TESTBTN:
	{
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);

		setvbuf(stdout, NULL, _IONBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);


		BYTE WarePacket[] = { 0xC2, 0x00, 0xBC, 0x31, 0x00, 0x0E, 0x00, 0x24, 0x56, 0x4A, 0x11, 0x00, 0xA0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x02, 0x28, 0x00, 0x3E, 0x00, 0x06, 0xB0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x04, 0x58, 0x00, 0x05, 0x00, 0x00, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x05, 0x07, 0x00, 0x38, 0x00, 0x05, 0x80, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x55, 0x00, 0x03, 0x00, 0x00, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0x0A, 0x82, 0x50, 0x01, 0x00, 0x20 , 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x11, 0x01, 0x00, 0x54, 0x00, 0x09, 0xA0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x13, 0x31, 0x18, 0x59, 0x00, 0x00, 0x70, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x17, 0x5A, 0x00, 0x1B, 0x00, 0x00, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x21, 0x24, 0x02, 0x45, 0x04, 0x00, 0x90, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x33, 0x01, 0x00, 0x53, 0x00, 0x05, 0xB0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0x24, 0x00, 0x3E, 0x20, 0x00, 0x70, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x70, 0x57, 0x00, 0x02, 0x00, 0x00, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x71, 0x57, 0x00, 0x01, 0x00, 0x00, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		recvpacket(gs_socket, WarePacket, MAKE_NUMBERW(WarePacket[1],WarePacket[2]), 0);

		/*
		PMSG_DEFAULT pMsg;
		PHeadSetB((LPBYTE)&pMsg, 0x82, sizeof(pMsg));
		SendMagicPacket((LPBYTE)&pMsg, pMsg.h.size);
		*/
		/*	//TEST CRASH
		BYTE CrashPacket[] = { 0xC1, 0x08, 0xFB, 0x07, 0x22, 0x99, 0x93, 0x82 };
		SendMagicPacket(CrashPacket, sizeof(CrashPacket));

		lpCharObj	lpPlayer;
		lpPlayer = &*(ObjectCharacter*)UserObjectStruct;

		lpPlayer->Level = 400;
		lpPlayer->AddPoint = 1337;
		lpPlayer->MoneyInventory = 2000000000;


	
		GAMESHOP_ANS_POINT pPoint;
		PHeadSubSetB((LPBYTE)&pPoint, 0xD2, 0x1, sizeof(GAMESHOP_ANS_POINT));
		// ----
		pPoint.WCoinC[0] = 1337;
		pPoint.WCoinC[1] = 1337;
		pPoint.WCoinP[0] = 1337;
		pPoint.WCoinP[1] = 1337;
		pPoint.GoblinPoint = 13371337;
		recvpacket(gs_socket,(BYTE*)&pPoint, pPoint.h.size, 0);
	

		//


		NEWS_REQ_NEWS pRequest2;
		pRequest2.h.set((LPBYTE)&pRequest2, 0xFE, sizeof(pRequest2));
		pRequest2.ID = 0xA;
		SendMagicPacket((LPBYTE)&pRequest2, pRequest2.h.size);
		
		PMSG_TRADE_OKBUTTON pMsg;
		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x3C;
		pMsg.h.size = sizeof(pMsg);//, 0x3C, sizeof(pMsg));
		pMsg.Flag = 1;
		recvpacket(gs_socket, (BYTE*)&pMsg, pMsg.h.size, 0);
		
		PMSG_TRADE_OKBUTTON pMsg2;
		pMsg2.h.c = 0xC1;
		pMsg2.h.headcode = 0x3C;
		pMsg2.h.size = sizeof(pMsg);//, 0x3C, sizeof(pMsg));
		pMsg2.Flag = 1;
		SendMagicPacket((BYTE*)&pMsg2, pMsg2.h.size);

		CONNECTEX_LOGIN pRequest;
		pRequest.h.set((LPBYTE)&pRequest, 0xFB, 12, sizeof(CONNECTEX_LOGIN));
		memcpy(pRequest.AccountID, "MeRonin", 10);
		memcpy(pRequest.Password, "annegeddes", 10);
		memcpy(pRequest.Name, "%d%s", 10);
		pRequest.TickCount = GetTickCount();
		SendMagicPacket((LPBYTE)&pRequest, pRequest.h.size);
		*/
		AddLog(hwnd, "OK TEST BTN\r\n");
	}
	break;

	
}
}
//=============================================================================
INT_PTR OnInitDlg(const HWND hwnd, LPARAM lParam)
{
	//set the small icon for the dialog. IDI_APPLICATION icon is set by default 
	//for winxp
	SendMessage(hwnd, WM_SETICON, ICON_SMALL,
		reinterpret_cast<LPARAM>(LoadImage(0, IDI_APPLICATION, IMAGE_ICON,
		0, 0, LR_SHARED)));
	//ensure focus rectangle is properly draw around control with focus
	PostMessage(hwnd, WM_KEYDOWN, VK_TAB, 0);
	return TRUE;
}
//=============================================================================
inline int ErrMsg(const ustring& s)
{
	return MessageBox(0, s.c_str(), _T("ERROR"), MB_OK | MB_ICONEXCLAMATION);
}
//=============================================================================

//MOVE !!!
void AddLog(const HWND &hwnd, char * Mesaj, ...)
{
	HWND hwndOutput = GetDlgItem(hwnd, IDC_HACKLOGS);

	int outLength = GetWindowTextLength(hwndOutput) + lstrlen(Mesaj) + 1;

	std::vector<TCHAR> buf(outLength);
	TCHAR *pbuf = &buf[0];

	GetWindowText(hwndOutput, pbuf, outLength);

	_tcscat_s(pbuf, outLength, Mesaj);

	SetWindowText(hwndOutput, pbuf);
}

void AddChatLog(const HWND &hwnd, char * Mesaj, ...)
{
	// get edit control from dialog
	HWND hwndOutput = GetDlgItem(hwnd, IDC_LOGS);

	// get new length to determine buffer size
	int outLength = GetWindowTextLength(hwndOutput) + lstrlen(Mesaj) + 1;

	// create buffer to hold current and new text
	std::vector<TCHAR> buf(outLength);
	TCHAR *pbuf = &buf[0];

	// get existing text from edit control and put into buffer
	GetWindowText(hwndOutput, pbuf, outLength);

	// append the newText to the buffer
	_tcscat_s(pbuf, outLength, Mesaj);

	// Set the text in the edit control
	SetWindowText(hwndOutput, pbuf);

}

char *GetChatType(int index)
{
	switch (index)
	{
	case 0:
		return "Normal";
		break;
	case 1:
		return "Guild";
		break;
	case 2:
		return "Alliance";
		break;
	case 3:
		return "GENS";
		break;

	default:
		return "UNKNOWN";
		break;
	}
}
/*

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
*/
