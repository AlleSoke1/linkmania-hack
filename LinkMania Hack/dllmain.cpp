// dllmain.cpp : Defines the entry point for the DLL application.
//#define _CRT_SECURE_NO_WARNINGS
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


HINSTANCE hinst;
//Classes init :)
CHitHack HitHack;

/* send */
int (WINAPI *psend)(SOCKET socket, BYTE* buffer, int length, int flags) = NULL;
int WINAPI mysend(SOCKET s, BYTE* buf, int len, int flags);
/* recv */
int (WINAPI *precv)(SOCKET socket, BYTE *buffer, int length, int flags) = NULL;
int WINAPI myrecv(SOCKET s, BYTE* buf, int len, int flags);
/* connect */
int (WINAPI *dconnect)(SOCKET, const struct sockaddr*, int) = NULL;
int WINAPI myconnect(SOCKET s, const struct sockaddr *name, int namelen);





extern  "C"  __declspec(dllexport) void __cdecl Mecanik()
{
	Security.Init();
	g_Console.ConsoleOutput(2, "[OK] Am pornit ! Comenzi Disponibile:");
	g_Console.ConsoleOutput(2, "/exit   (close game)");
	g_Console.ConsoleOutput(2, "/clear  (clear console)");
	g_Console.ConsoleOutput(2, "/dump   (show traffic)");
}

/* THREAD WINDOW */
unsigned int __stdcall threadTEST(void* data)
{

	WinMain(hinst, 0, 0, 1);

	return 1;
}


/* DLL Entry Point */
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,PVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//SPLASH SCREEN ^_^
		SplashScreen();

		//Window Vechi
		hinst = hModule;
		HANDLE hthreadTEST, hEvent;
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		hthreadTEST = (HANDLE)_beginthreadex(0, 0, &threadTEST, 0, 0, 0);



		//everything else.
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);

		setvbuf(stdout, NULL, _IONBF, 0);
		setvbuf(stderr, NULL, _IONBF, 0);

		g_Console.InitCore();
		Mecanik();
		SetByte((PVOID)0x004D1E69, 0xEB);    // mu.exe cacat
		SetByte((PVOID)(0x0095CD6D + 2), 1); //Min level Req to use Helper
		SetByte((PVOID)(0x0095CD93 + 1), 1); //MuHelper Box error


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



	if (buf[0] == (BYTE)0XC1){

		if (buf[2] == (BYTE)0xFB){
			//decrypt packet :)
			/*BYTE * buf1 = new BYTE[buf[1]];
			buf1 = buf;
			for (int i = buf1[1] - 1; i != 2; i += -1)
			{
				buf1[i] ^= buf1[i - 1] ^ cheimagice[i % 32];
			}*/

			BYTE *buf1 = DecodeMagicPacket(buf, buf[1]);

			if (buf1[3] == (BYTE)0x07){

				int index = MAKE_NUMBERW(buf[5], buf[4]); 
				if (index < 8000)
				{
					//printf("\n %.2X %.2X %.2X %.2X INDEX = %d\n", buf[4], buf[5], buf[6], buf[7], MAKE_NUMBERW(buf[5], buf[4]));
					if (autokillOnHover == 1)
					{
						for (int i = 0; i < autokillHitCount; i++)
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
	
	//Get Player Index
	if(buf[0] == 0xC1 && buf[2] == 0xF1 && buf[3] == 0x00)
		parsePlayerIndex((PMSG_JOINRESULT*)buf);
	


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

//to be moved to protocol!
void parsePlayerIndex(PMSG_JOINRESULT* Data) { PlayerIndex = MAKE_NUMBERW(Data->NumberH, Data->NumberL);  printf("Player Index = %d", PlayerIndex); };