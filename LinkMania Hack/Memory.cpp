// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

#include "Defines.h"


/* Write MEMORY */
DWORD WriteMemory(const LPVOID lpAddress, const LPVOID lpBuf, const UINT uSize)
{
	DWORD dwErrorCode = 0;
	DWORD dwOldProtect = 0;
	// ----
	int iRes = VirtualProtect(lpAddress, uSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	memcpy(lpAddress, lpBuf, uSize);
	// ----
	DWORD dwBytes = 0;
	// ----
	iRes = VirtualProtect(lpAddress, uSize, dwOldProtect, &dwBytes);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	return 0x00;
}
/* Read MEMORY */
DWORD ReadMemory(const LPVOID lpAddress, LPVOID lpBuf, const UINT uSize)
{
	DWORD dwErrorCode = 0;
	DWORD dwOldProtect = 0;
	// ----
	int iRes = VirtualProtect(lpAddress, uSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	memcpy(lpBuf, lpAddress, uSize);
	// ----
	DWORD dwBytes = 0;
	// ----
	iRes = VirtualProtect(lpAddress, uSize, dwOldProtect, &dwBytes);
	// ----
	if (iRes == 0)
	{
		dwErrorCode = GetLastError();
		return dwErrorCode;
	}
	// ----
	return 0x00;
}
/* Set BYTE */
DWORD SetByte(const LPVOID dwOffset, const BYTE btValue)
{
	return WriteMemory(dwOffset, (LPVOID)& btValue, sizeof(BYTE));
}
/* Get BYTE */
DWORD GetByte(const LPVOID dwOffset, BYTE & btValue)
{
	return ReadMemory(dwOffset, (LPVOID)btValue, sizeof(BYTE));
}