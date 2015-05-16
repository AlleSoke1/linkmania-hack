// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#define SECURITY_WIN32
#define _CRT_SECURE_NO_DEPRECATE
#include "StdAfx.h"
#include <stdio.h>
#include "windows.h"
#include "g_Console.h"
#include <stdlib.h>
#include "TEnum.h"
#include "Defines.h"
//#include "Security.h"
using  namespace std;
// -----------------------------------------------------------------------
Console g_Console;
// -----------------------------------------------------------------------
DWORD PiD;
// -----------------------------------------------------------------------

Console::Console()
{
	// ----
}
// -----------------------------------------------------------------------

Console::~Console()
{
	// ----
}
// -----------------------------------------------------------------------

void __stdcall LoggerCore(PVOID pVoid)
{
	char Temp[1024];
	// ----
	
	// ----
	while(true)
	{
		Sleep(100);
		g_Console.AddMessageToConsole(Temp);
		g_Console.LoadConsoleCommands(Temp);
	}
}
// -----------------------------------------------------------------------

void Console::InitCore()
{
	AllocConsole();
	SetConsoleTitleA(CONSOLETITLE);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LoggerCore, 0, 0, 0);
	// ----
	//printf("MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n", 
	//buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	this->ConsoleOutput(1,"#######################################################");
	this->ConsoleOutput(4,"[ LinkMania Hack ] Developed by (Alin1337) and (Mecanik1337)");
	this->ConsoleOutput(4,"[ LinkMania Hack ] www.Addicted2.ro and Ralukat Labs");
	this->ConsoleOutput(4,"[ LinkMania Hack ] Version: 1.0.0.1");
	this->ConsoleOutput(1,"#######################################################");

	//this->ConsoleOutput(4,"[ ---MAC--- ] %02X-%02X-%02X-%02X-%02X-%02X", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
}
// -----------------------------------------------------------------------

void Console::ConsoleOutput(int Color , const char* Format, ...)
{
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	// ----
	char Message[1024];
	char MessageOutPut[2048];
	DWORD dwBytes;
	// ----
	HANDLE Handle		 = GetStdHandle(STD_OUTPUT_HANDLE);
	char CorrectDate[11] = {0};
	// ----
	va_list pArguments;
	va_start(pArguments, Format);
	vsprintf_s(Message , Format , pArguments);
	va_end(pArguments);
	// ----

	sprintf_s(CorrectDate , "[%02d:%02d:%02d]" , Time.wHour , Time.wMinute , Time.wSecond);
	// ----
	sprintf_s(MessageOutPut , "%s %s\n" , CorrectDate , Message);
	// ----
	switch(Color)
	{
		// Color Red Console.
		case CNSL_E::CONSOLE_RED:
		{
			SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		}
		break;
		// Color Green Console.
		case CNSL_E::CONSOLE_GREEN:
		{
			SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}
		break;
		// Color Blue Console.
		case CNSL_E::CONSOLE_BLUE:
		{
			SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		}
		break;
		// Color Cyan Console.
		case CNSL_E::CONSOLE_CYAN:
		{
			SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		}
		break;
		// Color Yellow Console.
		case CNSL_E::CONSOLE_YELLOW:
		{
			SetConsoleTextAttribute(this->Handle(FALSE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}
		break;
	}
	// ----
	WriteFile(Handle , MessageOutPut , strlen(MessageOutPut) , &dwBytes , NULL);
}
// -----------------------------------------------------------------------



int Console::AddMessageToConsole(char* Buffer)
{
	char Text[1024];
	DWORD dwBytes;
	// ----
	memset(Text,   0x00, 1024);
	memset(Buffer, 0x00, 1024);
	// ----
	ReadFile(Handle(TRUE), Text, 1024, &dwBytes, NULL);
	// ----
	strncpy(Buffer, Text, strlen(Text) - 2);
	// ----
	return dwBytes;
}
// -----------------------------------------------------------------------

HANDLE Console::Handle(BOOL gImput)
{
	if(gImput == TRUE)
	{
		return GetStdHandle(STD_INPUT_HANDLE);
	}
	// ----
	else
	{
		return GetStdHandle(STD_OUTPUT_HANDLE);
	}
}
// -----------------------------------------------------------------------


void Console::LoadConsoleCommands(char* gImput)
{
	char Temp[1024]		= {0};
	// ----
	if(!strncmp(gImput, "/clear" , 7))
	{
		system("cls");
		return;
	}
	// ----
	else if((!strncmp(gImput, "/exit", 5)) ||
			(!strncmp(gImput, "/quit", 5)))
	{
		exit(1);
		return;
	}
	else if ((!strncmp(gImput, "/test", 5)))
	{
		bReturn = 1;
	}
	else if ((!strncmp(gImput, "/dump", 5)))
	{
		ShowTraffic = 1;
	}

	else if ((!strncmp(gImput, "/writedump", 5)))
	{
		SaveTraffic = 1;
	}
	
}

// -----------------------------------------------------------------------
extern Console g_Console;
// -----------------------------------------------------------------------