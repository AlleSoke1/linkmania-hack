// dllmain.cpp : Defines the entry point for the DLL application.
//#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <tlhelp32.h>
#include "windows.h"
#include <tchar.h>
#include <time.h>
#include "detours.h"
#include <rpc.h>
#include <rpcdce.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <assert.h>
#include <Csignal>
#include <Sys/types.h>
#include <Sys/stat.h>
#include <Aclapi.h>
#include <Sddl.h>
#include <process.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <string>
#include <sstream>
#include "g_Console.h"
#include <stdio.h>
#include "Defines.h"
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "detours.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "wldap32.lib") 
#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "winmm.lib") 
#pragma comment(lib, "user32.lib")
#include "resource.h"
#include "Window.h"
#include <vector>
#include "OTOKILL.h"


using namespace std;

SOCKET gs_socket;
int gsConnect = 0;
int autokill = 0;
int hithackCount = 0;
int maxhitcount = 50;
BYTE teleID[2];
BYTE TELEcoord[2] = { 130, 130 };

//MobList VECTOR
std::vector<int> mobList;

#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define MAKE_NUMBERW(x,y)  ( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* Definitii hook-uri */
/* send */
int (WINAPI *psend)(SOCKET socket, BYTE* buffer, int length, int flags) = NULL;
int WINAPI mysend(SOCKET s, BYTE* buf, int len, int flags);
/* recv */
int (WINAPI *precv)(SOCKET socket, BYTE *buffer, int length, int flags) = NULL;
int WINAPI myrecv(SOCKET s, BYTE* buf, int len, int flags);
/* connect */
int (WINAPI *dconnect)(SOCKET, const struct sockaddr*, int) = NULL;
int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen);
/* Definitii hook-uri */

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

// -------------------------------------------------------------------
//WTF???
extern  "C"  __declspec(dllexport) void __cdecl Mecanik()
{
	
//	MainForm Init;
//	Init.DoModal();
	g_Console.ConsoleOutput(2, "[OK] Am pornit ! Comenzi Disponibile:");
	g_Console.ConsoleOutput(2, "/exit   (close game)");
	g_Console.ConsoleOutput(2, "/clear  (clear console)");
	g_Console.ConsoleOutput(2, "/dump   (show traffic)");
}
/* THREAD WINDOW */
unsigned int __stdcall threadTEST(void* data)
{

	wWinMain(0, 0, 0, 1);

	return 1;
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

	psend(s, buf, len, flags);
}
/* Send Magic Packet :D */
void SendMagicPacket(LPBYTE Data, int Size)
{
	BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };
	int StartPos = 0;
	// ----
	if (Data[0] == 0xC1 || Data[0] == 0xC3)
	{
		StartPos = 3;
	}
	else if (Data[0] == 0xC2 || Data[0] == 0xC4)
	{
		StartPos = 4;
	}
	// ----
	for (int i = StartPos; i < Size; i++)
	{
		Data[i] ^= Data[i - 1] ^ cheimagice[i % 32];
	}
	// ----

	sendpacket(gs_socket, Data, Size, 0);
}
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
BYTE addr = 0; 

//extern "C" { int _afxForceUSRDLL; }

/* DLL Entry Point */
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);

		setvbuf(stdout, NULL, _IONBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);

		g_Console.InitCore();
		Mecanik();
		SetByte((PVOID)0x004D1E69, 0xEB);    // mu.exe cacat
		SetByte((PVOID)(0x0095CD6D + 2), 1); //Min level Req to use Helper
		SetByte((PVOID)(0x0095CD93 + 1), 1); //MuHelper Box error

    //	addr = GetAddress(0x6C852B78, "zClient.dll");
	//	SetByte((PVOID)(addr), 'H');
	//	SetByte((PVOID)(addr + 1), 'a');
	//  SetByte((PVOID)(addr + 2), 'c');
	//	SetByte((PVOID)(addr + 3), 'k');

		//Window Vechi
		HANDLE hthreadTEST, hEvent;
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		hthreadTEST = (HANDLE)_beginthreadex(0, 0, &threadTEST, 0, 0, 0);
		//Window nou
			
		//threadTEST = (HANDLE)_beginthreadex(NULL, 0, threadTEST, 0, 0, 0);
		/////////////////////////////////////////////////////
		DisableThreadLibraryCalls(hModule);
		DetourRestoreAfterWith();    //Start finding function pointers
		//DetourFindFunction("main.exe", "WindowName");
		//BOOL bRet = HookAPI(   "ntdll.dll", "NtTerminateThread", NtTerminateThreadCallback, (PVOID*) &NtTerminateThreadNext, SAFE_HOOKING);
		precv = (int (WINAPI *)(SOCKET, BYTE*, int, int))DetourFindFunction("Ws2_32.dll", "recv");
		psend = (int (WINAPI *)(SOCKET, BYTE*, int, int))DetourFindFunction("Ws2_32.dll", "send");
		dconnect = (int (WINAPI *)(SOCKET s, const struct sockaddr *name, int namelen))DetourFindFunction("Ws2_32.dll", "connect");
		DetourAndErrorCheck(&(PVOID&)psend, mysend, "send"); //Begin detouring
		DetourAndErrorCheck(&(PVOID&)precv, myrecv, "recv"); //Begin detouring
		DetourAndErrorCheck(&(PVOID&)dconnect, myconnect, "connect");
		// DetourAndErrorCheck((PVOID*) &NtTerminateThreadNext, NtTerminateThreadCallback, "NtTerminateThread");// Test ? :) 
		/////////////////////////////////////////////////////  
		//asteptam();
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		CheckAndDetach(&(PVOID&)psend, mysend, "send");
		CheckAndDetach(&(PVOID&)precv, myrecv, "recv");
		CheckAndDetach(&(PVOID&)precv, myconnect, "connect"); //aici greseala
		WSACleanup();
		break;
	}
	return TRUE;
}

/* fake recv Packet bitch! :) */
void recvpacket(SOCKET s, BYTE* buf, int len, int flags)
{
	cout << "FAKE-RECV ->";

	for (int i = 0; i < len; i++){
		printf(" %.2X ", buf[i]);
	}
	printf("\n");

	precv(s, buf, len, flags);
}
/*
void CheckMouseButtonStatus()
{
	//Check the mouse left button is pressed or not
	if ((GetKeyState(VK_LBUTTON) & 0x80) != 0)
	{
		MessageBox(NULL, "LButton pressed", "LButton pressed", MB_OK);
	}
	//Check the mouse right button is pressed or not
	if ((GetKeyState(VK_RBUTTON) & 0x80) != 0)
	{
	//	MessageBox(NULL, "RButton pressed", "LButton pressed", MB_OK);
	}
} */




// -------------------------------------------------------------------
int WINAPI mysend(SOCKET s, BYTE* buf, int len, int flags) {
	
	int StartPos = 0;
	// ----
	BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };


	if (gsConnect == 1){
		/*
		if (buf[0] == 0xC1 || buf[0] == 0xC3)
		{
			StartPos = 3;
		}
		else if (buf[0] == 0xC2 || buf[0] == 0xC4)
		{
			StartPos = 4;
		}
		// ----
		for (int i = StartPos; i < len; i++)
		{
			buf[i] ^= buf[i - 1] ^ MecanikXOR[i % 32];
		}
		// ----
		*/
	
		//Check the mouse left button is pressed or not
		if (g_Console.bReturn == 1)
		{
			
			if ((GetKeyState(VK_SPACE) & 0x80) != 0)
			{
		////		g_Console.ConsoleOutput(1, "[Kill] ON!");
		///		CreateThread(0, 0, LPTHREAD_START_ROUTINE(AutoKill), 0, 0, 0);
				
			}
		}
	}
	//0xC1 0x07 0x11 0x23 0x93 0x33 0x69 0xC3 0x12
	/*
	FILE *fileAS;
	fopen_s(&fileAS, "LMHack-PacketFilter.txt", "a+");
	fprintf(fileAS, "[SEND][buf:");
	for (int i = 0; i < len; ++i)
	{
		fprintf(fileAS, "%02hX ", (BYTE)buf[i]);
	}
	fprintf(fileAS, "][Len: %d][Socket: %d]", len, s);
	fprintf(fileAS, "\n");
	fclose(fileAS);
	*/
	
	//0xC2 0x00 0x0F 0x13 0x01 0x88 0x70 0x00 0x03
	//   
if (autokill == 1)
{
	if (buf[2] == 0x11){
		for (int i = 0; i < hithackCount; i++)
		{
			unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0xFF, 0xFF, 0x33, 0x69 }; //  ? 69// hit ar trebuii sa fie ?
		
			buf1[3] = buf[3];
			buf1[4] = buf[4];
			buf1[5] = buf[5];
			buf1[6] = buf[6];
			
			sendpacket(gs_socket, buf1, sizeof(buf1), 0);
		}
	}
}




/**/
BYTE * buf1 = new BYTE[buf[1]];
buf1 = buf;


//memcpy(buf1, buf, sizeof(buf[1]));

if (buf1[0] == (BYTE)0XC1){

		if (buf1[2] == (BYTE)0xFB){
			
			for (int i = buf1[1] - 1; i != 2; i += -1)
			{
				buf1[i] ^= buf1[i - 1] ^ cheimagice[i % 32];
			}

			if (buf1[3] == (BYTE)0x07){

				int index = MAKE_NUMBERW(buf[5], buf[4]); 
				if (index < 8000)
				{
					printf("\n %.2X %.2X %.2X %.2X INDEX = %d\n", buf[4], buf[5], buf[6], buf[7], MAKE_NUMBERW(buf[5], buf[4]));
					if (autokill == 1)
					{
						for (int i = 0; i < hithackCount; i++)
						{
							//unsigned char bufDC[] = { 0xC1, 0x07, 0x18, buf[5], buf[4], 0x84, 0xBD }; //  ? 69// hit ar trebuii sa fie ?
							//printf("SENT DC PAK\n");
							//SendMagicPacket(bufDC, bufDC[1]);
						
							unsigned char buf1X[] = { 0xC1, 0x07, 0x11, 0xFF, 0xFF, 0x78, 0x05 }; //  ? 69// hit ar trebuii sa fie ?
							//  ^      ^
							//( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )
					
							buf1X[3] = buf[5];
							buf1X[4] = buf[4];

							SendMagicPacket(buf1X, sizeof(buf1X));
						}
					}
				}
				g_Console.ConsoleOutput(1, "[HP BAR] REQUEST BAR!!");
			}
		}
}

if (buf[2] == 0xFB)
{
	printf("SEND ->");
	for (int i = 0; i < buf[1]; i++)
	{
		printf("%.2X ", buf[i]);
	}
	printf("\n");
}

	return psend(s, buf, len, flags);
}

// -------------------------------------------------------------------
int WINAPI myrecv(SOCKET s, BYTE *buf, int len, int flags)
{

	//unsigned char XOR[] = { 0x04, 0x08, 0x0f, 0x10, 0x17, 0x2a, 0xff, 0x7b, 0x84, 0xb3 };

	/*if (buf[2] == 0x00 || buf[2] == 0x02) {
		for (int i = 3; i < buf[1]; i++) {
			buf[i] ^= XOR[(i - 3) % 10];
		}
	}*/

//	BYTE LiNU[10];
//	BYTE LiNU2[100];

	/*if (buf[0x00] == (BYTE)0xc1) {
		if (buf[0x02] == (BYTE)0x02) {
			if (buf[0x01] == (BYTE)0x68) {
				for (int i = 0x03; i < 0x0d; i++) {
					if (buf[i] != 0x00) {
						LiNU[i - 3] = buf[i];
					}
					else {
						break;
					}
				}
				
				for (int i = 0x15; i < 0x9d; i++) {
					if (buf[i] != 0x00) {
						LiNU2[i - 0x15] = buf[i];
					}
					else {
						break;
					}

				}

				g_Console.ConsoleOutput(5, "[World] %s : %s", LiNU, LiNU2);

			}
		}
	}*/
	// Get server list
	if (buf[0] == (BYTE)0xc1)
	{
		if (buf[1] == (BYTE)0x04)
		{
			if (buf[2] == (BYTE)0x00)
			{
				g_Console.ConsoleOutput(5, "[ServerList] L-am primit!");

			}

		}
	}

	//connect to gs -_- ?
	if (buf[0] == (BYTE)0xc2) 
	{
		if (buf[1] == (BYTE)0x00)
		{
			if (buf[2] == (BYTE)0x0b)
			{
				g_Console.ConsoleOutput(5, "[ServerGS] Conectat!");

			}

		}
	}
	//test mob hit?
	if (buf[0] == (BYTE)0XC1){

		if (buf[1] == (BYTE)0x18){

			if (buf[2] == (BYTE)0xFB){

				if (buf[3] == (BYTE)0x07){
				
				//	cout << "asta da?:" << buf[4] << endl;dllmain
					g_Console.ConsoleOutput(1, "[HP BAR SENT] hit!!");
				}

			}
		}

	}

	// C1 07 D4 skill hit
	// C1 08 D4 monster dead ?
	/*if (buf[0] == (BYTE)0xC1)
	{
		if (buf[1] == 0x20)
		{
			if (buf[2] == 0x11)
			{
				//buf[3] buf[4]
				for (int i = 0; i < 5; i++)
				{
					if (autokill == 1)
					{
						unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0x23, 0x93, 0x33, 0x69 }; //  ? 69// hit ar trebuii sa fie ?
						int targetIndex = ((WORD)(buf[3] & 0x7F) << 8) | (BYTE)buf[3 + 1];//MAKE_NUMBERW(buf[3], buf[4]);
						buf1[3] = (BYTE)((targetIndex >> 8) & 0x00FF);
						buf1[4] = (BYTE)(targetIndex & 0x00FF);
					
						sendpacket(gs_socket, buf1, sizeof(buf1), 0);
					}
				}
			}
		}
	} */
	
	if (autokill == 2) // enable soon
	{
		if (buf[0] == (BYTE)0xC2)
		{

			if (buf[3] == 0x13)
			{
				//buf[5] buf[6] == 1
				if (buf[4] == 1)
				{

					int targetIndex = MAKE_NUMBERW(buf[5], buf[6]);
					cout << "TARGET INDEX" << targetIndex << endl;
					for (int i = 0; i < hithackCount; i++)
					{

						unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0xFF, 0xFF, 0x78, 0x05 }; //  ? 69// hit ar trebuii sa fie ?
																//  ^      ^
						//( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )
						int targetIndex = MAKE_NUMBERW(buf[5], buf[6]);
						buf1[3] = (BYTE)((targetIndex >> 8) & 0x00FF);
						buf1[4] = (BYTE)(targetIndex & 0x00FF);

						SendMagicPacket(buf1, sizeof(buf1));
					}
				}
				if (buf[4] > 1)
				{
					BYTE _start = 0;
					for (int i = 0; i < buf[4]; i++)
					{
			
						int targetIndex = MAKE_NUMBERW(buf[5+_start], buf[6+_start]);
						cout << "TARGET INDEX" << targetIndex << endl;
						for (int i = 0; i < hithackCount; i++)
						{
							unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0xFF, 0xFF, 0x78, 0x05 }; //  ? 69// hit ar trebuii sa fie ?
																	//  ^      ^
							
							//( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )
							int targetIndex = MAKE_NUMBERW(buf[5 + _start], buf[6 + _start]);
							buf1[3] = (BYTE)((targetIndex >> 8) & 0x00FF);
							buf1[4] = (BYTE)(targetIndex & 0x00FF);

							Sleep(10); 
							SendMagicPacket(buf1, sizeof(buf1));
							
						}
						_start += 10;
					}
				}
			}
		}
	}

	//cout << "[recv]: 0x%02hX / 0x%02hX / 0x%02hX / 0x%02hX / 0x%02hX / 0x%02hX " << (unsigned char)buf[0] << (unsigned char)buf[1] << (unsigned char)buf[2] << (unsigned char)buf[3] << (unsigned char)buf[4] << (unsigned char)buf[5] << endl;

	/*	FILE *fileAS1;
		fopen_s(&fileAS1, "LMHack-PacketFilter.txt", "a+");
		fprintf(fileAS1, "[RECV][buf:");
		for (int i = 0; i < len; ++i)
		{
			fprintf(fileAS1, "%02hX ", (BYTE)buf[i]);
		}
		fprintf(fileAS1, "][Len: %d][Socket: %d]", len, s);
		fprintf(fileAS1, "\n");
		fclose(fileAS1);
	*/
	if (buf[0] == 0xC1)
	{
		if (buf[2] == 0xD4)
		{
			teleID[0] = buf[3];
			teleID[1] = buf[4];
		}
	}


	if (buf[0] == 0xC2 && buf[3] == 0x13) //mob sent
	{
		printf("RECV ->");
		for (int i = 0; i < buf[1] * 256 + buf[2]; i++)
		{
			printf("%.2X ",buf[i]);
		}
		printf("\n");
	}
	
			///\g_Console.ConsoleOutput(4, "[RECV]: 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX", (BYTE*)buf[0], (BYTE*)buf[1], (BYTE*)buf[2], (BYTE*)buf[3], (BYTE*)buf[4], (BYTE*)buf[5], (BYTE*)buf[6], (BYTE*)buf[7], (BYTE*)buf[8], (BYTE*)buf[9], (BYTE*)buf[10], (BYTE*)buf[11]);
	
	return precv(s, buf, len, flags);
}

// -------------------------------------------------------------------
int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen) {

	SOCKADDR_IN* name_in = (SOCKADDR_IN*)name;
	unsigned short uPort = ntohs(name_in->sin_port);
	char *sender = inet_ntoa(name_in->sin_addr);

	g_Console.ConsoleOutput(1, "[Connect]: %s : %d", sender, uPort);

	if ( uPort == 55901) // verificam daca e portul de la GS
	{

		gs_socket = s; // salvam socketul
		gsConnect = 1;
	}
	else{
		gsConnect = 0; //fix for server switch
	}

	return dconnect(s, name, namelen);
}

//need to be moved

#define ID_AUTOKILL 1
#define ID_AUTOKILLOFF 2
#define ID_HITCOUNT 3
#define ID_TELETEST 4
#define ID_TELETESTX 5
#define ID_TELETESTY 6
#define ID_DEC 7
#define ID_BUFF 8

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

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_FORMVIEW),
		hDialog, reinterpret_cast<DLGPROC>(DialogProc));

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
	static HWND hwndEdit,TeleX,TeleY;
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
			unsigned char CTS[] = { 0xC1, 0x06, 0xD4, TELEcoord[0] , TELEcoord[1], 0x10 }; //HEK


			SendMagicPacket(CTS, CTS[1]);
			recvpacket(gs_socket, STC, sizeof(STC), 0);

			Sleep(200); 
			SendMagicPacket(CTS, CTS[1]);
		//	sendpacket(gs_socket, CTS, sizeof(CTS), 0);
			recvpacket(gs_socket, STC, sizeof(STC), 0);

			g_Console.ConsoleOutput(2, "[TELEPORT] TO x= %d y= %d  ", TELEcoord[0], TELEcoord[1]);
		}
		if (LOWORD(wParam) == ID_BUFF) {
			BYTE BuffPacket[] = { 0xC1, 0x04, 0xF6 , 0xFD };
			
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
				BYTE *a = (BYTE*)(oXORFilterStart+4*i)+3;
				printf(" %.2X ", *(a));
			}

			BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };
			unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0x23, 0x93, 0x33, 0x69 };
			for (int i = buf1[1]-1; i != 2; i += -1)
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