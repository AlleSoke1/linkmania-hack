#include "stdafx.h"
#include <vector>
#include <iostream>
#include "OTOKILL.h"


void AutoKillThread()
{
	
}

void dumpMobList()
{
	std::cout << "Mob List Dump -> ";
	for (std::vector<int>::iterator it = mobList.begin(); it != mobList.end(); ++it)
	{
		std::cout << *it << ",";
	}
	std::cout << std::endl;
}