// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#include "stdafx.h"
#include "Defines.h"


SOCKET gs_socket;
int gsConnect = 0;
int hithack = 0;

int autokill = 0;
int autokillOnHover = 0; //folosinduse de Get HP Bar packet :)
int autokillHitCount = 0;

int maxhitcount = MaxHITCount;
BYTE teleID[2];
BYTE TELEcoord[2] = { 130, 130 };
int hithackCount = 0;
BYTE addr = 0;
int PlayerIndex = 0;

//Classes init :)
HINSTANCE hinst;
CHitHack HitHack;
CLinkManiaHack LinkManiaHack;

/* send */
int (WINAPI *psend)(SOCKET socket, BYTE* buffer, int length, int flags) = NULL;
int WINAPI mysend(SOCKET s, BYTE* buf, int len, int flags);
/* recv */
int (WINAPI *precv)(SOCKET socket, BYTE *buffer, int length, int flags) = NULL;
int WINAPI myrecv(SOCKET s, BYTE* buf, int len, int flags);
/* connect */
int (WINAPI *dconnect)(SOCKET, const struct sockaddr*, int) = NULL;
int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen);

/* THREAD WINDOW */
unsigned int __stdcall threadTEST(void* data)
{

	WinMain(hinst, 0, 0, 1);

	return 1;
}

extern  "C"  __declspec(dllexport) void __cdecl Mecanik(HMODULE hModule)
{
	//SPLASH SCREEN ^_^
	SplashScreen();
	Security.Init();
	g_Console.ConsoleOutput(2, "[CORE]: OK Am pornit ! Comenzi Disponibile:");
	g_Console.ConsoleOutput(2, "/exit        (close game)");
	g_Console.ConsoleOutput(2, "/clear       (clear console)");
	g_Console.ConsoleOutput(2, "/dump        (show traffic)");
	g_Console.ConsoleOutput(2, "/writedump   (save traffic to .txt)");

	//Window Vechi
	hinst = hModule;
	HANDLE hthreadTEST, hEvent;
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hthreadTEST = (HANDLE)_beginthreadex(0, 0, &threadTEST, 0, 0, 0);


}


/* DLL Entry Point */
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,PVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);

		setvbuf(stdout, NULL, _IONBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);

		g_Console.InitCore();
		Mecanik(hModule);                    // asa nul poate chema prin export huh! ^_^
		SetByte((PVOID)0x004D1E69, 0xEB);    // mu.exe cacat
		SetByte((PVOID)(0x0095CD6D + 2), 1); // Min level Req to use Helper
		SetByte((PVOID)(0x0095CD93 + 1), 1); // MuHelper Box error

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



int WINAPI mysend(SOCKET s, BYTE* buf, int len, int flags) {
	
	int StartPos = 0;
	// ----
	BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };

	if (hithack == 1)
	{
		HitHack.DoMobHit(gs_socket, buf, hithackCount);
	}

	if (autokill == 1)
		LinkManiaHack.AutoKill(buf);

	if (buf[0] == 0xC2 && buf[3] == 0x31) //trade
	{
		printf("SEND ->");
		for (int i = 0; i < buf[1] * 256 + buf[2]; i++) //c2 = buf[1] * 256 + buf[2]
		{
			printf("%.2X ", buf[i]);
		}
		printf("\n");
	}



	// if console
	if (g_Console.ShowTraffic == 1){
		g_Console.ConsoleOutput(4, "[CORE][SEND]: 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX", (BYTE*)buf[0], (BYTE*)buf[1], (BYTE*)buf[2], (BYTE*)buf[3], (BYTE*)buf[4], (BYTE*)buf[5], (BYTE*)buf[6], (BYTE*)buf[7], (BYTE*)buf[8], (BYTE*)buf[9], (BYTE*)buf[10], (BYTE*)buf[11]);
	}

	// if save packets
	if (g_Console.SaveTraffic == 1){
		LinkManiaHack.WriteSendPacketLog(buf, len, s);
	}

	return psend(s, buf, len, flags);
}

int WINAPI myrecv(SOCKET s, BYTE *buf, int len, int flags)
{
	
	//Get Player Index
	if (buf[0] == 0xC1 && buf[2] == 0xF1 && buf[3] == 0x00){
		parsePlayerIndex((PMSG_JOINRESULT*)buf);
	}

	// Get server list
	LinkManiaHack.GetServerList(buf);

	//connect to gs -_- ?
	LinkManiaHack.GetServerList(buf);

	//get hp bar
	//LinkManiaHack.GetHPBar(buf);
	



	if (buf[0] == 0xC2 && buf[3] == 0x31) //warehouse
	{
		printf("RECV ->");
		for (int i = 0; i < buf[1] * 256 + buf[2]; i++) //c2 = buf[1] * 256 + buf[2]
		{
			printf("%.2X ",buf[i]);
		}
		printf("\n");
	}

	// if console
	if (g_Console.ShowTraffic == 1){
		g_Console.ConsoleOutput(4, "[CORE][RECV]: 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX 0x%02hX", (BYTE*)buf[0], (BYTE*)buf[1], (BYTE*)buf[2], (BYTE*)buf[3], (BYTE*)buf[4], (BYTE*)buf[5], (BYTE*)buf[6], (BYTE*)buf[7], (BYTE*)buf[8], (BYTE*)buf[9], (BYTE*)buf[10], (BYTE*)buf[11]);
	}

	// if save packets
	if (g_Console.SaveTraffic == 1){
		LinkManiaHack.WriteRecvPacketLog(buf, len, s);
	}

	return precv(s, buf, len, flags);
}

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

void parsePlayerIndex(PMSG_JOINRESULT* Data)
{
	PlayerIndex = MAKE_NUMBERW(Data->NumberH, Data->NumberL); 
	//printf("Player Index = %d", PlayerIndex);
};