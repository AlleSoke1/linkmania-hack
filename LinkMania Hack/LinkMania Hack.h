// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#ifndef __LINKMANIAHACK_H__
#define __LINKMANIAHACK_H__
#include <minwindef.h>

class CLinkManiaHack
{
public:
	void GetServerList(BYTE* buf);
	void GetServerConnect(BYTE* buf);
	void GetHPBar(BYTE* buf);
	void GetPost(BYTE* buf);
	void DCHack(BYTE* buf);
	void AutoKill(BYTE* buf);
	void WriteRecvPacketLog(BYTE* buf, int len, SOCKET s);
	void WriteSendPacketLog(BYTE* buf, int len, SOCKET s);
};

extern CLinkManiaHack LinkManiaHack;

#endif