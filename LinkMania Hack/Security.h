// ================================================================= //
// # LinkMania Hack For MuOnline	                               # //
// # Project Started Mecanik1337 and Alin1337                      # //
// # This file is part of the LinkMania Hack Project               # //
// # www.Addicted2.ro || Ralukat Labs                              # //
// ================================================================= //
#ifndef __SECURITY_H__
#define __SECURITY_H__

#pragma comment(lib, "rpcrt4.lib")
#define MacCount 30

// Security
class CSecurity
{
public:
	bool Init();
	bool Checking(LPCSTR MacID);
};

extern CSecurity Security;

#endif