// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

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