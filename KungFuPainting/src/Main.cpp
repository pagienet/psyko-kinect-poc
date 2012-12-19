#include <iostream>
#include <windows.h>
#include "core/debug.h"
#include "KungFuPOC.h"

psyko::KungFuPOC application;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif
	application.SetWindowTitle(L"Kung Fu Paint POC");
	int code = application.Init(hInstance, pCmdLine, nCmdShow);
	if (code) {
		std::stringstream str;
		str << "Initialization failed with error code: 0x" << std::hex << code;
		Alert(str.str());
		return code;
	}
	code = application.Run();
	
	if (code) {	
		std::stringstream str;
		str << "Initialization failed with error code: 0x" << std::hex << code;
		Alert(str.str());
	}

	return code;
}