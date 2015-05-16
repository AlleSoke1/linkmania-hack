// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#pragma once
// -------------------------------------------------------------------

//#include "targetver.h"
// -------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
// -------------------------------------------------------------------
#include <windows.h>
#include "targetver.h"
#include <stdio.h>
#include <gl\GL.h>
#include <Math.h>

#include <winsock2.h>
//GLOBALS
void SendMagicPacket(LPBYTE Data, int Size);
void sendpacket(SOCKET s, BYTE* buf, int len, int flags);
void recvpacket(SOCKET s, BYTE* buf, int len, int flags);