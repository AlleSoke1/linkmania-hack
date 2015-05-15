// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //
#include "StdAfx.h"
#include "Security.h"
#include "g_Console.h"
// Security
CSecurity Security;
#include <stdio.h>
#include <Windows.h>
#include <rpc.h>
#include <rpcdce.h>
#pragma comment(lib, "rpcrt4.lib")

bool CSecurity::Init()
{
	unsigned char MACData[6];
	UUID uuid;
	UuidCreateSequential( &uuid ); 

	for (int i=2; i<8; i++) 
		MACData[i - 2] = uuid.Data4[i];

	char MACADDR[18];
	sprintf(MACADDR, "%02X-%02X-%02X-%02X-%02X-%02X", MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
	g_Console.ConsoleOutput(4, "[ ---MAC--- ] This Machine: %02X-%02X-%02X-%02X-%02X-%02X", MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
	if(Checking(MACADDR))	
	{
		g_Console.ConsoleOutput(4, "[ ---MAC--- ] License OK!");
		return true;
	}
	else
	{
		g_Console.ConsoleOutput(1, "[ ---MAC--- ] License Error!");
		MessageBoxA(NULL,"You don't have an active license to use these files!", "Error!",MB_OK);
		::ExitProcess(0);
	}
	return false;
}

bool CSecurity::Checking(LPCSTR MacID)
{
	char ClientsMacs[MacCount][18] =
	{
		
		"D0-50-99-1A-B7-64",  // Alinu   :d
		"00-0C-29-B5-C8-FA",  // Alinu2  :d
		"BC-5F-F4-74-54-4A",  // MECANIK :d
		"B4-82-FE-68-1F-F5",  // MECANIK2 :d
	};

	for(int x=0; x<MacCount; x++)
	{
		if(!strcmp(MacID,ClientsMacs[x]))
		{
			return true;
		}
	}
	return false;
}