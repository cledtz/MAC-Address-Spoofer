#include <iostream>
#include <string>
using namespace std;
#include <Windows.h>

#include "MAC.hxx"

auto main() -> int
{
	SetConsoleTitleA(_("https://github.com/cledtz/MAC-Address-Spoofer"));

	cout << _("Open source at: https://github.com/cledtz/MAC-Address-Spoofer") << endl << endl;

	cout << _("--> Starting spoofing process") << endl << endl;

	CMAC* MAC = new CMAC();

	MAC->HadAdministrativePermissions();

	MAC->SpoofMAC();

	int me = MessageBoxA(NULL, _("Would you like to restart your adapters [REQUIRED TO APPLY CHANGES]: (Yes - continue\\No - skip)"), _("Selection"), MB_ICONQUESTION | MB_YESNO);
	if (me == IDYES)
	{
		MAC->RestartAdapters();
	}

	cout << endl << _("--> DONE.") << endl;

	system(_("pause >nul"));
	//Sleep(-1);
}