#include "Defines.h"

//SEND / RECIEVE PACKET FUNCTIONS :)
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

BYTE* DecodeMagicPacket(LPBYTE Data, int Size)
{
	BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };
	
	for (int i = Data[1] - 1; i != 2; i += -1)
	{
		Data[i] ^= Data[i - 1] ^ cheimagice[i % 32];
	}

	return Data;
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

void sendpacket(SOCKET s, BYTE* buf, int len, int flags)
{


	/*	cout << "SEND ->";

	for (int i = 0; i < len; i++){
	printf(" %.2X ", Packet[i]);
	}
	printf("\n");
	*///g_Console.ConsoleOutput(1, "Packet sent: 0x%02X", (BYTE)Packet);

	psend(s, buf, len, flags); // was psend2
}


//Splash Screen
void SplashScreen()
{
	CSplash splash1(IDB_BITMAP1, RGB(255, 255, 255));
	splash1.ShowSplash();
	Sleep(5000);
	splash1.CloseSplash();
}