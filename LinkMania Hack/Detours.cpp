#include "Defines.h"



// -------------------------------------------------------------------
// Functie pentru detour...
void DetourAndErrorCheck(PVOID* pFunctionToDetour, PVOID pReplacement, const char* functionName)
{
	char* debugString = new char[256];
	ZeroMemory(debugString, 256);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(pFunctionToDetour, pReplacement);
	int errorCode = DetourTransactionCommit();
	if (errorCode == NO_ERROR)
	{
#ifdef DEBUG
		FILE* FileHwnd1;
		FileHwnd1 = fopen("LinkManiaHackERROR.log", "a+");
		fprintf(FileHwnd1, "------------ LOG --------------- \n");
		fprintf(FileHwnd1, "Address of %s: 0x%x\n", functionName);
		fprintf(FileHwnd1, "Address of replacement function: 0x%x\n", pFunctionToDetour);
		fprintf(FileHwnd1, "%s() was detoured successfully \n", pReplacement, functionName);
		fclose(FileHwnd1);
		OutputDebugString(debugString);
#endif
	}
	else
	{
		if (errorCode == ERROR_INVALID_DATA)
			MessageBoxA(NULL, "ERROR_INVALID_DATA returned", "ERROR", MB_OK);
		else if (errorCode == ERROR_INVALID_OPERATION)
			MessageBoxA(NULL, "ERROR_INVALID_OPERATION returned", "ERROR", MB_OK);
		else
#ifdef DEBUG
			MessageBoxA(NULL, "Other error code returned", "ERROR", MB_OK);
		sprintf_s(debugString, 255, "%s() was not able to be detoured\n"
			"Address of %s: 0x%x\n"
			"Address of replacement function: 0x%x", functionName,
			functionName, pFunctionToDetour, pReplacement);
		FILE* FileHwnd1;
		FileHwnd1 = fopen("LinkManiaHackERROR.log", "a+");
		fprintf(FileHwnd1, "------------ LOG --------------- \n");
		fprintf(FileHwnd1, "%s() was not able to be detoured \n", functionName);
		fprintf(FileHwnd1, "Address of %s: 0x%x \n", functionName);
		fprintf(FileHwnd1, "Address of replacement function: 0x%x \n", pFunctionToDetour, pReplacement);
		fclose(FileHwnd1);

#endif
		OutputDebugString(debugString);
	}
	delete[] debugString;
}

// -------------------------------------------------------------------
// Functie pentru detour... gasire functie
void CheckAndDetach(PVOID* pDetouredFunction, PVOID pReplacement, const char* functionName)
{
	if (pDetouredFunction != NULL)
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)pDetouredFunction, pReplacement);
		DetourTransactionCommit();
	}
	else
	{
		int errorSize = 64 + strlen(functionName);
		char* errorString = new char[errorSize];
		ZeroMemory(errorString, 32);
		MessageBoxA(NULL, " could not be detached\n Was it detoured in the first place?", "ERROR", MB_OK);
		OutputDebugString(errorString);
		delete[] errorString;
	}
}