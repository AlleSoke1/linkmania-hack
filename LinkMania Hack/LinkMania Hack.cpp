// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#define _CRT_SECURE_NO_DEPRECATE
#include "stdafx.h"
#include "Defines.h"
#include "LinkMania Hack.h"

void CLinkManiaHack::GetServerList(BYTE* buf)
{
	if (buf[0] == (BYTE)0xc1)
	{
		if (buf[1] == (BYTE)0x04)
		{
			if (buf[2] == (BYTE)0x00)
			{
				g_Console.ConsoleOutput(5, "[CORE][ServerList] L-am primit!");

			}

		}
	}
}

void CLinkManiaHack::GetServerConnect(BYTE* buf)
{
	if (buf[0] == (BYTE)0xc2)
	{
		if (buf[1] == (BYTE)0x00)
		{
			if (buf[2] == (BYTE)0x0b)
			{
				g_Console.ConsoleOutput(5, "[CORE][ServerGS] Conectat!");

			}

		}
	}
}

void CLinkManiaHack::GetHPBar(BYTE* buf)
{
	if (buf[0] == (BYTE)0xC1){

		if (buf[1] == (BYTE)0x18){

			if (buf[2] == (BYTE)0xFB){

				if (buf[3] == (BYTE)0x07){

					//	cout << "asta da?:" << buf[4] << endl;dllmain
					g_Console.ConsoleOutput(1, "[CORE][HP BAR] Requested!");
				}

			}
		}

	}


}

void CLinkManiaHack::GetPost(BYTE* buf)
{
	unsigned char XOR[] = { 0x04, 0x08, 0x0f, 0x10, 0x17, 0x2a, 0xff, 0x7b, 0x84, 0xb3 };

	if (buf[2] == 0x00 || buf[2] == 0x02) {
	for (int i = 3; i < buf[1]; i++) {
	buf[i] ^= XOR[(i - 3) % 10];
	}
	}

		BYTE LiNU[10];
		BYTE LiNU2[100];

	if (buf[0x00] == (BYTE)0xc1) {
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

	g_Console.ConsoleOutput(5, "[CORE][World] %s : %s", LiNU, LiNU2);

	}
	}
	}

}

void CLinkManiaHack::AutoKill(BYTE* buf)
{
	// need developing !
	if (buf[0] == (BYTE)0xC1){

		if (buf[2] == (BYTE)0xFB){
			
			if (autokill == 1)
			{//decrypt packet :)
				/*BYTE cheimagice[] = { 0xCF, 0x10, 0x4E, 0x3A, 0xC2, 0xD8, 0x5F, 0xAD, 0xE4, 0x02, 0x20, 0xDF, 0xEB, 0x42, 0x46, 0xED, 0xF0, 0x87, 0x2D, 0x6E, 0x21, 0x53, 0xA7, 0xCE, 0x83, 0xE1, 0xE7, 0xF6, 0xDF, 0x6F, 0x88, 0x1A };
				BYTE * buf1 = new BYTE[buf[1]];
				buf1 = buf;
				

				for (int i = buf1[1] - 1; i != 2; i += -1)
				{
					buf1[i] ^= buf1[i - 1] ^ cheimagice[i % 32];
				}*/
				BYTE *buf1 = DecodeMagicPacket(buf, buf[1]);

			if (buf1[3] == (BYTE)0x07){
			
				int index = MAKE_NUMBERW(buf1[5], buf1[4]);
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

								buf1X[3] = buf1[5];
								buf1X[4] = buf1[4];

								SendMagicPacket(buf1X, sizeof(buf1X));
							}
						}
					}

					if (buf[3] == (BYTE)0x07){
						//DC HACK
						srand(time(NULL));
						BYTE bufDC[] = { 0xC1, 0x07, 0x18, buf1[5], buf1[4], 0x81, rand() % 255 };
						SendMagicPacket(bufDC, bufDC[1]);
						g_Console.ConsoleOutput(1, "[DCHACK] SENT!");
					}

					g_Console.ConsoleOutput(1, "[HP BAR] REQUEST BAR!!");
				}

			}
		}
	}
}

void CLinkManiaHack::DCHack(BYTE* buf)
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

					int targetIndex = MAKE_NUMBERW(buf[5 + _start], buf[6 + _start]);
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

void CLinkManiaHack::WriteRecvPacketLog(BYTE* buf, int len, SOCKET s)
{

	FILE *fileAS1;
	fopen_s(&fileAS1, "LMHack-Packets.txt", "a+");
	fprintf(fileAS1, "[RECV][buf:");
	for (int i = 0; i < len; ++i)
	{
		fprintf(fileAS1, "%02hX ", (BYTE)buf[i]);
	}
	fprintf(fileAS1, "][Len: %d][Socket: %d]", len, s);
	fprintf(fileAS1, "\n");
	fclose(fileAS1);
}

void CLinkManiaHack::WriteSendPacketLog(BYTE* buf, int len, SOCKET s)
{

	FILE *fileAS1;
	fopen_s(&fileAS1, "LMHack-Packets.txt", "a+");
	fprintf(fileAS1, "[SEND][buf:");
	for (int i = 0; i < len; ++i)
	{
		fprintf(fileAS1, "%02hX ", (BYTE)buf[i]);
	}
	fprintf(fileAS1, "][Len: %d][Socket: %d]", len, s);
	fprintf(fileAS1, "\n");
	fclose(fileAS1);
}