// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#include "stdafx.h"
#include "Defines.h"

void CHitHack::DoPlayerHit() //class fix
{

	//soon 
}

void CHitHack::DoMobHit(SOCKET gs_socket, BYTE* buf, int hithackCount) //class fix
{
	if (buf[2] == 0x11){
		for (int i = 0; i < hithackCount; i++)
		{
			unsigned char buf1[] = { 0xC1, 0x07, 0x11, 0xFF, 0xFF, 0x33, 0x69 }; // 

			buf1[3] = buf[3];
			buf1[4] = buf[4];
			buf1[5] = buf[5];
			buf1[6] = buf[6];
			//g_Console.ConsoleOutput(5, "[HitHack]: PlayerHit called"); //because called too many times.
			// LOL i knowed that but i was too sleepy ! ^_^

			sendpacket(gs_socket, buf1, sizeof(buf1), 0);
		}
	}

}