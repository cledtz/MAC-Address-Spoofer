#include <iostream>
#include <string>
using namespace std;
#include <Windows.h>

#include "MAC.hxx"

auto main() -> int
{
	CMAC* MAC = new CMAC();

	MAC->HadAdministrativePermissions();

	MAC->SpoofMAC();
	MAC->RestartAdapters();

	cout << _("--> DONE.") << endl;

	Sleep(-1);
}