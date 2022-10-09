#include "MAC.hxx"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

CMAC::CMAC()
{
	//Seeding for random numbers
	srand((unsigned)time(0));
}

string CMAC::GenerateMAC()
{
	stringstream temp;
	int number = 0;
	string result;

	for (int i = 0; i < 6; i++)
	{
		number = rand() % 254;
		temp << setfill('0') << setw(2) << hex << number;
		if (i != 5)
		{
			temp << "-";
		}
	}
	result = temp.str();

	for (auto& c : result)
	{
		c = toupper(c);
	}

	return result;
}

void CMAC::showAdapterList()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	UINT i;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) 
	{
		cout << _("Error: Allocating memory needed to call GetAdaptersinfo.") << endl;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) 
		{
			cout << _("Error: Allocating memory needed to call GetAdaptersinfo") << endl;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			cout << _("\n\tComboIndex: \t") << pAdapter->ComboIndex << endl;
			cout << _("\tAdapter Name: \t") << pAdapter->AdapterName << endl;
			cout << _("\tAdapter Description: \t") << pAdapter->Description << endl;
			cout << _("\tAdapter Address: \t");
			for (i = 0; i < pAdapter->AddressLength; i++) 
			{
				if (i == (pAdapter->AddressLength - 1))
					printf("%.2X\n", (int)pAdapter->Address[i]);
				else
					printf("%.2X-", (int)pAdapter->Address[i]);
			}
			cout << _("\tIP Address: \t") << pAdapter->IpAddressList.IpAddress.String << endl;
			cout << _("\tIP Mask: \t") << pAdapter->IpAddressList.IpMask.String << endl;
			cout << _("\tGateway: \t") << pAdapter->GatewayList.IpAddress.String << endl;
			pAdapter = pAdapter->Next;
		}
	}
	else 
	{
		cout << _("GetAdaptersInfo failed with error: ") << dwRetVal << endl;
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
}

unordered_map<string, string> CMAC::getAdapters()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	unordered_map<string, string> result;
	stringstream temp;
	string str_mac;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) 
	{
		cerr << _("[!] Error: Allocating memory needed to call GetAdaptersInfo") << endl;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL) 
		{
			cerr << _("[!] Error: Adapter is NULL after calling GetAdaptersInfo") << endl;
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			for (UINT i = 0; i < pAdapter->AddressLength; i++) 
			{
				temp << setfill('0') << setw(2) << hex << (int)pAdapter->Address[i];
				if (i != pAdapter->AddressLength - 1)
				{
					temp << "-";
				}
			}
			str_mac = temp.str();
			temp.str("");
			temp.rdbuf();
			for (auto& c : str_mac)
			{
				c = toupper(c);
			}

			result.insert({ pAdapter->Description, str_mac });
			pAdapter = pAdapter->Next;
		}
	}
	else 
	{
		cout << _("[!] GetAdaptersInfo failed with error: ") << dwRetVal << endl;
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return result;
}

void CMAC::SpoofMAC()
{
	vector <string> list;
	unordered_map<string, string> AdapterDetails = getAdapters();
	for (auto& itm : AdapterDetails)
	{
		list.push_back(itm.first);
	}

	//cout << "\n[+]List of Available Adapters: " << endl;
	int range = 0;
	for (auto itm = list.begin(); itm != list.end(); itm++)
	{
		//cout << '\t' << range + 1 << ")" << *itm << endl;
		range++;
	}


	int selection = 1;

	while(selection < range + 1) 
	{

		cout << _("[+] Adapter: ") << list.at(selection - 1) << endl;
		cout << _("[+] Old MAC: ") << AdapterDetails.at(list.at(selection - 1)) << endl;

		string wstr(list.at(selection - 1).begin(), list.at(selection - 1).end());
		const char* wAdapterName = wstr.c_str();

		bool bRet = false;
		HKEY hKey = NULL;
		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, _("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}"), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwIndex = 0;
			TCHAR Name[1024];
			DWORD cName = 1024;
			while (RegEnumKeyExA(hKey, dwIndex, Name, &cName, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				HKEY hSubKey = NULL;
				if (RegOpenKeyExA(hKey, Name, 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS)
				{
					BYTE Data[1204];
					DWORD cbData = 1024;
					if (RegQueryValueExA(hSubKey, _("DriverDesc"), NULL, NULL, Data, &cbData) == ERROR_SUCCESS)
					{

						if (_tcscmp((TCHAR*)Data, wAdapterName) == 0)
						{
							string temp = GenerateMAC();
							string newMAC = temp;
							temp.erase(std::remove(temp.begin(), temp.end(), '-'), temp.end());

							string wstr_newMAC(temp.begin(), temp.end());
							const char* newMACAddr = wstr_newMAC.c_str();


							if (RegSetValueExA(hSubKey, _("NetworkAddress"), 0, REG_SZ, (const BYTE*)newMACAddr, sizeof(TCHAR) * ((DWORD)_tcslen(newMACAddr) + 1)) == ERROR_SUCCESS)
							{
								cout << _("[+] New MAC: ") << newMAC << endl;
							}
						}
					}
					RegCloseKey(hSubKey);
				}
				cName = 1024;
				dwIndex++;
			}
			RegCloseKey(hKey);

		}
		else
		{
			cout << _("[!] Error: Failed to access registery.") << endl;
		}
		cout << " " << endl;

		selection++;
	}
}

void CMAC::RestartAdapters()
{
	cout << _("[+] Disabling adapters...") << endl;
	system(_("WMIC PATH WIN32_NETWORKADAPTER WHERE PHYSICALADAPTER=TRUE CALL DISABLE >nul 2>&1")); // >nul 2>&1
	cout << _("[+] Enabling adapters...") << endl;
	system(_("WMIC PATH WIN32_NETWORKADAPTER WHERE PHYSICALADAPTER=TRUE CALL ENABLE >nul 2>&1"));
}

bool CMAC::HadAdministrativePermissions()
{
	BOOL isMember;
	PSID administratorsGroup = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;

	AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroup);
	CheckTokenMembership(nullptr, administratorsGroup, &isMember);
	if (!isMember)
	{
		cout << _("Error: Please start the program with administrative permissions.");
		Sleep(-1);
	}

	return true;
}