#include <windows.h>
#include <strsafe.h>
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

void PrintSystemInfo() {
	SYSTEM_INFO si;
	::GetNativeSystemInfo(&si);

	printf("Processors: %u\n", si.dwNumberOfProcessors);
	printf("Page Size: %u bytes\n", si.dwPageSize);
	printf("Processor Type: %d\n", si.dwProcessorType);
	printf("Processor Mask: %zX\n", si.dwActiveProcessorMask);
	printf("Processor Architecture: %d\n", si.wProcessorArchitecture);

}


int main()
{
	// Prevent multiple instances of the application
	HANDLE hMutex = ::CreateMutex(nullptr, TRUE, L"SingleInstanceMutex");
	if (!hMutex)
	{
		PrintError("CreateMutex", ::GetLastError());
		return 1;
	}
	else if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("Another instance of the application is already running\n");
		return 1;
	}

	PrintSystemInfo();

	DWORD currentProcessId = GetCurrentProcessId();
	printf("Current Process ID: %d\n", currentProcessId);

	// HANDLE hProcess = GetCurrentProcess();
	HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, currentProcessId);
	if (hProcess == NULL) {
		PrintError("OpenProcess", GetLastError());
		return 1;
	}

	// 32-bit - Project Property - Linker - System - Enable Large Addresses
	// dumpbin /headers path\to\exe
	// Search for "Application can handle large (>2GB) addresses"

	unsigned int x = 4294967295; // Maximum value for 32-bit unsigned int
	printf("printf for unsigned and signed int are different: %u %d\n", x, x);

	WCHAR path[MAX_PATH];
	::GetWindowsDirectory(path, _countof(path));
	printf("Windows directory: %ws\n", path);

	WCHAR name[MAX_COMPUTERNAME_LENGTH];
	DWORD size = _countof(name);
	printf("Computer Name Max Length: %u\n", MAX_COMPUTERNAME_LENGTH);
	printf("Computer name length (Before passing to ::GetComputerName): %u\n", size);
	if (::GetComputerName(name, &size)) {
		printf("Computer name: %ls\n", name);
		printf("len is updated to: %u\n", size);
	}
	else {
		PrintError("GetComputerName", ::GetLastError());
	}
	CloseHandle(hProcess); // Not really needed, but good practice

	size = _countof(name);
	if (::GetUserName(name, &size)) {
		printf("User name: %ls\n", name);
		printf("len is updated to: %u\n", size);
	}
	else {
		PrintError("GetUserName", ::GetLastError());
	}


	// SHELLEXECUTEINFO sei = { sizeof(sei) };
	SHELLEXECUTEINFO sei;
	memset(&sei, 0, sizeof(sei));
	sei.cbSize = sizeof(sei);

	sei.lpFile = L"c:\\windows\\win.ini";
	sei.lpVerb = L"open";
	sei.nShow = SW_SHOW;
	::ShellExecuteEx(&sei);

	WCHAR buffer[128];
	printf("_countof and sizeof are different: %d %u\n", _countof(buffer), sizeof(buffer));
	::StringCchPrintfW(buffer, _countof(buffer), L"Hello, Windows! from process %u", currentProcessId);
	::MessageBox(nullptr, buffer, L"Hello", MB_OK | MB_ICONINFORMATION);


	CONSOLE_CURSOR_INFO cci;
	if (::GetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &cci)) {
		cci.bVisible = FALSE;
		::SetConsoleCursorInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &cci);
	}
	else {
		PrintError("GetConsoleCursorInfo", ::GetLastError());
	}

	PERFORMACE_INFORMATION pi;
	printf("\n");

	for (;;) {
		if (!::GetPerformanceInfo(&pi, sizeof(pi))) {
			PrintError("GetPerformanceInfo", ::GetLastError());
			break;
		}
		printf("Processes: %4u\n", pi.ProcessCount);
		printf("Threads: %5u\n", pi.ThreadCount);
		printf("Handles: %7u\n", pi.HandleCount);
		printf("Committed: %7zu KB\n", pi.CommitTotal >> 10);
		printf("Kernel Pool: %5zu KB\n", pi.KernelTotal >> 10);
		::Sleep(1000);
		printf("\r\033[5A");
	}

}