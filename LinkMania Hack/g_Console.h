#ifndef G_CONSOLE_H
#define G_CONSOLE_H
// -----------------------------------------------------------------------
#include "windows.h"
#include <stdio.h>
// -----------------------------------------------------------------------
#define CONSOLETITLE "[ LinkMania Hack ] By Mecanik1337, Alin1337 and Ralukat Labs!"
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

};
// -----------------------------------------------------------------------
extern Console g_Console;
// -----------------------------------------------------------------------
#endif
// -----------------------------------------------------------------------