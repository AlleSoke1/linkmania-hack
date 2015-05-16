// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //

/* Get Address */
#include <libloa
DWORD GetAddress(DWORD addie, LPCSTR module)

{

	if (GetModuleHandle(module) != NULL)

	{

		DWORD addie1 = (DWORD)GetModuleHandle(module);

		DWORD addie2 = addie;

		DWORD address = addie1 + addie2;

		return address;

	}

	else

		return NULL;

}