// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#ifndef __HITHACK_H__
#define __HITHACK_H__
#include <minwindef.h>
#define MacCount 30


class CHitHack
{
public:
	void DoPlayerHit();
    void DoMobHit(SOCKET gs_socket, BYTE* buf, int hithackCount);
};

extern CHitHack HitHack;

#endif