// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#include "stdafx.h"
#include "HitHack.h"
#include "Defines.h"
#include <WinSock2.h>
#include "g_Console.h"

void DoPlayerHit()
{

	//soon 
}

void DoMobHit(SOCKET gs_socket, BYTE* buf, int hithackCount)
{
	if (buf[2] == 0x11){
		for (int i = 0; i < hithackCount; i++)
		{
			unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0xFF, 0xFF, 0x33, 0x69 }; //  ? 69// hit ar trebuii sa fie ?

			buf1[3] = buf[3];
			buf1[4] = buf[4];
			buf1[5] = buf[5];
			buf1[6] = buf[6];
			g_Console.ConsoleOutput(5, "[HitHack]: PlayerHit called");

			sendpacket(gs_socket, buf1, sizeof(buf1), 0);
		}
	}

}