// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#ifndef G_CONSOLE_H
#define G_CONSOLE_H
// -----------------------------------------------------------------------
#include "windows.h"
#include <stdio.h>
// -----------------------------------------------------------------------
#define CONSOLETITLE "Ralukat Labs!"
// -----------------------------------------------------------------------

class Console
{
public:
	Console();
	~Console();
	void InitCore();
	void ConsoleOutput(int Color, const char* Format, ...);
	void ConsoleOutputInPula(int Color, BYTE * Format, ...);
	int AddMessageToConsole(char* Buffer);
	HANDLE Handle(BOOL Input);
	void LoadConsoleCommands(char* gImput); 
	int bReturn = 0;
	int ShowTraffic = 0;
	int SaveTraffic = 0;

};
// -----------------------------------------------------------------------
extern Console g_Console;
// -----------------------------------------------------------------------
#endif
// -----------------------------------------------------------------------