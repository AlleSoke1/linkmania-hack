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
void PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size);
void PHeadSetB(LPBYTE lpBuf, BYTE head, int size);
void PHeadSetW(LPBYTE lpBuf, BYTE head, int size);

//window
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DialogProc(HWND hWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

void AddChatLog(const HWND &hwnd, char * Mesaj, ...);



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
extern BYTE* XorKeys;
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

struct PBMSG_HEAD2	// Packet - Byte Type
{

public:

	void set(LPBYTE lpBuf, BYTE head, BYTE sub, BYTE size)
	{
		lpBuf[0] = 0xC1;
		lpBuf[1] = size;
		lpBuf[2] = head;
		lpBuf[3] = sub;
	};

	BYTE c;
	BYTE size;
	BYTE headcode;
	BYTE subcode;
};
#pragma pack(push, 1)
struct NEWS_REQ_NEWS
{
	PBMSG_HEAD	h;
	BYTE		ID;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CONNECTEX_LOGIN
{
	PBMSG_HEAD2	h;
	char		AccountID[10];
	char		Password[10];
	char		Name[10];
	DWORD		TickCount;
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


struct PMSG_XORKEYS
{
	PBMSG_HEAD h;	//	
	BYTE subhead;
	BYTE XORKEYES[10];	//	4
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

struct PMSG_CHATDATA_NUMBER
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char chatmsg[89];	// 5
};

#pragma pack(push, 1)
struct GAMESHOP_ANS_POINT
{
	PBMSG_HEAD2 h;
	// ----
	BYTE	Result;
	double	WCoinC[2];
	double	WCoinP[2];
	double	GoblinPoint;
};
#pragma pack(pop)

struct PMSG_TRADE_OKBUTTON
{
	PBMSG_HEAD h;	// C1:3C
	BYTE Flag;	// 3
};

struct PMSG_DEFAULT
{
	PBMSG_HEAD h;
};


void parsePlayerIndex(PMSG_JOINRESULT* Data);


//OBJECT STRUCT
#pragma pack(push, 1)
struct UnknownStruct0
{
	/*+0*/	WORD	Unknown0;//?
	/*+2*/	WORD	Unknown2;//?
	/*+4*/	WORD	Unknown4;//?
	/*+6*/	WORD	Unknown6;
	/*+8*/	WORD	Unknown8;
	/*+10*/	WORD	Unknown10;
	/*+12*/	WORD	Unknown12;//?
	/*+14*/	WORD	Unknown14;
	/*+16*/	WORD	Unknown16;
};
#pragma pack(pop)
#pragma pack(push, 1)
typedef struct	//-> Complete (size: 107)
{
	/*+0*/		short	ItemID;
	/*+2*/		int		Level;
	/*+6*/		char	Unknown6;
	/*+7*/		char	Unknown7;
	/*+8*/		char	Unknown8;
	/*+9*/		short	DamageMin;
	/*+11*/		short	DamageMax;
	/*+13*/		char	Unknown13;
	/*+14*/		short	Unknown14;
	/*+16*/		short	Unknown16;
	/*+18*/		char	Unknown18;
	/*+19*/		char	Unknown19;
	/*+20*/		short	Unknown20;
	/*+22*/		char	Durability;
	/*+23*/		char	ExcellentOption;
	/*+24*/		char	AncientOption;
	/*+25*/		short	ReqStrenght;
	/*+27*/		short	ReqDextirity;
	/*+29*/		short	ReqEnergy;
	/*+31*/		short	ReqVitality;
	/*+33*/		short	ReqCommand;
	/*+35*/		short	ReqLevel;
	/*+37*/		char    SpecialCount;
	/*+38*/		WORD	SpecialType[8];
	/*+54*/		BYTE	SpecialValue[8];
	/*+62*/		int		UniqueID;
	/*+66*/		char	Unknown66;
	/*+67*/		char 	PosX;
	/*+68*/		char 	PosY;
	/*+69*/		WORD 	HarmonyType;
	/*+71*/		short	HarmonyValue;
	/*+73*/		char 	Is380Item;
	/*+74*/		char	SocketOption[5];
	/*+79*/		char	Unknown79;
	/*+80*/		char	SocketSeedIndex[5];
	/*+85*/		char	SocketSphereLevel[5];
	/*+90*/		char	SocketSet;
	BYTE gap01[5];
	/*+96*/		char	DurabilityState;
	/*+97*/		char 	PeriodItem;
	/*+98*/		char 	ExpiredItem;
	/*+99*/		int		ExpireDateConvert;
	/*+103*/	int		Unknown103;
} ObjectItem, *lpItemObj;
#pragma pack(pop)
#pragma pack(push, 1)
typedef struct	//-> InDev (size: 6012)
{
	/*+0*/		char	Name[11];
	/*+11*/		char	Class;
	/*+12*/		BYTE	Unknown12;
	/*+13*/		BYTE	Unknown13;
	/*+14*/		short	Level;
	/*+16*/		DWORD	Experience;
	/*+20*/		DWORD	NextExperience;
	/*+24*/		WORD	Strength;
	/*+26*/		WORD	Dexterity;
	/*+28*/		WORD	Vitality;
	/*+30*/		WORD	Energy;
	/*+32*/		WORD	Leadership;
	/*+34*/		WORD	Life;
	/*+36*/		WORD	Mana;
	/*+38*/		WORD	MaxLife;
	/*+40*/		WORD	MaxMana;
	/*+42*/		WORD	Shield;
	/*+44*/		WORD	MaxShield;
	/*+46*/		WORD	AttackRate;
	/*+48*/		WORD	DefenseRate;
	/*+50*/		short	AddStrength;
	/*+52*/		short	AddDexterity;
	/*+54*/		short	AddVitality;
	/*+56*/		short	AddEnergy;
	BYTE gap01[4];
	/*+62*/		WORD	AddLeadership;	//GS use unsigned value...
	/*+64*/		WORD	Stamina;
	/*+66*/		WORD	MaxStamina;
	/*+68*/		BYTE	ItemSpecialUseFlag;		//Bit decomposit (1, 2, 8)
	BYTE UnknownGap;
	/*+70*/		WORD	ItemSpecialUseTime[3];	//[ ]
	/*+76*/		WORD	AddPoint;	//FruitStat start
	/*+78*/		WORD	MaxAddPoint;
	/*+80*/		WORD	MinusPoint;
	/*+82*/		WORD	MaxMinusPoint;	//FruitStat end
	/*+84*/		WORD	AttackSpeed;
	/*+86*/		WORD	DamageRate;
	/*+88*/		WORD	DamageMin;
	/*+90*/		WORD	DamageMax;
	/*+92*/		WORD	Unknown92;
	/*+94*/		WORD	Unknown94;
	/*+96*/		WORD	MagicSpeed;
	BYTE gap02[10];
	/*+108*/	WORD	Unknown108;
	/*+110*/	WORD	Unknown110;
	BYTE gap03[4];
	/*+116*/	WORD	LevelPoint;
	/*+118*/	BYTE	MagicCount;
	/*+119*/	BYTE	Unknown119;
	/*+120*/	WORD	pMagicList[650];	//maybe 150?
	/*1420*/
	BYTE gap04[3252];	//maybe preview struct some here
	/*+4672*/	ObjectItem pEquipment[12];
	/*+5956*/	DWORD	MoneyInventory;	//(C4)
	/*+5960*/	DWORD	MoneyWarehouse;	//(C4)
	BYTE gap05[8];
	/*+5972*/	UnknownStruct0 Unknown5972;
	BYTE gap06[2];
	/*+5992*/	WORD	Unknown5992;
	/*+5994*/	WORD	Unknown5994;
	BYTE gap07[2];
	/*+5998*/	WORD	Unknown5998;
	/*+6000*/	WORD	Unknown6000;
	/*+6002*/	WORD	Unknown6002;
	/*+6004*/	WORD	Unknown6004;
	/*+6006*/	WORD	Unknown6006;
	/*+6008*/	BYTE	Unknown6008;
	/*+6009*/	BYTE	Unknown6009;
	/*+6010*/	BYTE	Unknown6010;
	/*+6011*/	BYTE	Unknown6011;
} ObjectCharacter, *lpCharObj;
#pragma pack(pop)

void ParseXorKeys(PMSG_XORKEYS * recv);
void ParseChat(PMSG_CHATDATA * recv);