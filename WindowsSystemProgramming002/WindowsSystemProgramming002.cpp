
#define BUILD_WINDOWS
#include <windows.h>
#include <stdio.h>
#include <VersionHelpers.h>
#include <Psapi.h>

void PrintError(const char* functionName, DWORD errorMessageID) {
	LPSTR messageBuffer = NULL;
	size_t size = FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	if (size > 0) {
		fprintf(stderr, "%s failed with error %lu: %.*s\n", functionName, errorMessageID, (int)size, messageBuffer);
	}
	else {
		fprintf(stderr, "%s failed with error %lu, and FormatMessageA failed with error %lu\n", functionName, errorMessageID, GetLastError());
	}

	if (messageBuffer) {
		LocalFree(messageBuffer);
	}
}


int main()
{
	::SetPriorityClass(::GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	OSVERSIONINFO vi = { sizeof(vi) };
	::GetVersionEx(&vi); // deprecated, need BUILD_WINDOWS defined.
	printf("Windows version: %u.%u.%u\n", vi.dwMajorVersion, vi.dwMinorVersion, vi.dwBuildNumber);

	// To show the correct Windows version, you need to create manifest.xml and add manifest.xml to project.
	// Project Configuration -> Manifest Tool -> Input and Output -> Additional Manifest Files
	printf("Is Windows 10 or later: %s\n", IsWindows10OrGreater() ? "Yes" : "No");
	printf("Is Windows 8.1 or later: %s\n", IsWindows8Point1OrGreater() ? "Yes" : "No");
	printf("Is Windows 8 or later: %s\n", IsWindows8OrGreater() ? "Yes" : "No");
	printf("Is Windows 7 or later: %s\n", IsWindows7OrGreater() ? "Yes" : "No");
	printf("Is Windows Vista or later: %s\n", IsWindowsVistaOrGreater() ? "Yes" : "No");
	printf("Is Windows XP or later: %s\n", IsWindowsXPOrGreater() ? "Yes" : "No");


	HANDLE hEvent = ::CreateEvent(nullptr, TRUE, FALSE, L"MyEvent");
	if (hEvent == nullptr) {
		PrintError("CreateEvent", ::GetLastError());
	}
	else {
		printf("Event object created\n");
		// Start 2 instance without debugger check with Process Explorer for event object sharing, but with 2 handles.
		// The share is because of the name of the event object are identical.
		Sleep(INFINITE); 
		::SetEvent(hEvent);
		::CloseHandle(hEvent);
	}

}