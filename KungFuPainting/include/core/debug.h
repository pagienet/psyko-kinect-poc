#ifndef _PSYKO_DEBUG_
#define _PSYKO_DEBUG_

#include <assert.h>
#include <sstream>
#include <iostream>

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#define ASSERT(condition, message) \
	assert(condition && message)

#ifdef _DEBUG
#ifdef WIN32

#define DEBUG_TRACE(message) { \
	std::ostringstream sstream; \
	sstream << "[trace] " << message << std::endl; \
	OutputDebugStringA(sstream.str().c_str()); \
	std::cout << "[trace] " << message << std::endl; \
}

#endif // WIN32
#else //!_DEBUG
#define DEBUG_TRACE(message) {}
#endif //_DEBUG

inline void Alert(const std::string& message)
{
#ifdef WIN32
	HWND hwnd = FindWindowEx( NULL, NULL, L"PsykoWindow", NULL );
	MessageBoxA(hwnd, message.c_str(), "Alert", MB_OK);
#endif
}

#endif