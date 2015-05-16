// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#include <WinSock2.h>
#include "detours.h"
#include "windows.h"
#include <string.h>
#include <stdlib.h>
#include "g_Console.h"
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
#include <vector>
#include "splash.h"
#include "Security.h"
#include "HitHack.h"
#include "LinkMania Hack.h"

using namespace std;

#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define MAKE_NUMBERW(x,y)  ( (WORD)(((BYTE)((y)&0xFF)) |   ((BYTE)((x)&0xFF)<<8 ))  )

//window
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);
//detours
void DetourAndErrorCheck(PVOID* pFunctionToDetour, PVOID pReplacement, const char* functionName);
void CheckAndDetach(PVOID* pDetouredFunction, PVOID pReplacement, const char* functionName);

void SplashScreen();

//packets
void SendMagicPacket(LPBYTE Data, int Size);
BYTE* DecodeMagicPacket(LPBYTE Data, int Size);
void recvpacket(SOCKET s, BYTE* buf, int len, int flags);
void sendpacket(SOCKET s, BYTE* buf, int len, int flags);
BYTE* EncodeChatPacket(LPBYTE Data);

//memory
DWORD WriteMemory(const LPVOID lpAddress, const LPVOID lpBuf, const UINT uSize);
DWORD ReadMemory(const LPVOID lpAddress, LPVOID lpBuf, const UINT uSize);
DWORD SetByte(const LPVOID dwOffset, const BYTE btValue);
DWORD GetByte(const LPVOID dwOffset, BYTE & btValue);

//GLOBALS
extern SOCKET gs_socket;
extern int gsConnect;
extern int hithack;
extern int autokill;
extern int autokillOnHover;
extern int autokillHitCount;
extern int maxhitcount;
extern BYTE teleID[2];
extern BYTE TELEcoord[2];
extern int hithackCount;
extern BYTE addr;
extern int PlayerIndex;

/* send */
extern int (WINAPI *psend)(SOCKET socket, BYTE* buffer, int length, int flags) ;
extern int WINAPI mysend(SOCKET s, BYTE* buf, int len, int flags);
/* recv */
extern int (WINAPI *precv)(SOCKET socket, BYTE *buffer, int length, int flags) ;
extern int WINAPI myrecv(SOCKET s, BYTE* buf, int len, int flags);
/* connect */
extern int (WINAPI *dconnect)(SOCKET, const struct sockaddr*, int) ;
extern int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen);


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


//packet structs :)

#pragma pack(push, 1)
struct PBMSG_HEAD
{
public:
	void set(LPBYTE lpBuf, BYTE head, BYTE size)
	{
		lpBuf[0] = 0xC1;
		lpBuf[1] = size;
		lpBuf[2] = head;
	};
	void setE(LPBYTE lpBuf, BYTE head, BYTE size)
	{
		lpBuf[0] = 0xC3;
		lpBuf[1] = size;
		lpBuf[2] = head;
	};
	BYTE c;
	BYTE size;
	BYTE headcode;
};
#pragma pack(pop)

struct PMSG_JOINRESULT
{
	PBMSG_HEAD h;	// C1:F1
	BYTE scode;	// 3
	BYTE result;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE CliVersion[5];	// 7
};

struct PMSG_CHATDATA
{
	PBMSG_HEAD h;	//	
	char chatid[10];	//	3
	char chatmsg[89];	//	D
};

struct PMSG_NOTICE
{
	PBMSG_HEAD h;
	BYTE type;	// 3
	BYTE btCount;	// 4
	WORD wDelay;	// 6	
	int dwColor;	// 8
	BYTE btSpeed;	// C
	char Notice[256];	// D

};

void parsePlayerIndex(PMSG_JOINRESULT* Data);
