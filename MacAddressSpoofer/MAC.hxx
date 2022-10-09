#pragma once
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include <Windows.h>
#include <iphlpapi.h>
#include <tchar.h>
#include <netcon.h>
//this is important , becasue window.h defines MAX which conflicts 
#undef max
//-----------------------------------------------

#pragma comment(lib, "IPHLPAPI.lib")
using namespace std;

#include "encryptor.hxx"

class CMAC
{
public:
	CMAC();

	string GenerateMAC();
	void showAdapterList();
	unordered_map<string, string> getAdapters();
	void SpoofMAC();
	void RestartAdapters();

	bool HadAdministrativePermissions();
};

