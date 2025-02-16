
#include <Windows.h>
#include <stdio.h>
#include <string>
// This code fail, I can't find the shared handle on process explorer: https://www.youtube.com/watch?v=6IapJC71olI&list=PLVP36NCkbjNSgvFtqWtP0gKbFDtgz8RKJ&index=2
int main()
{
	HANDLE hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	printf("Handle: 0x%p\n", hEvent);

	PROCESS_INFORMATION pi;
	STARTUPINFO si = { sizeof(si) };
	std::wstring name(L"Notepad ");
	name += std::to_wstring((long long)hEvent);

	if (!::CreateProcess(nullptr, (WCHAR*)name.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
		printf("CreateProcess failed: %d\n", ::GetLastError());
		return 1;
	}

	printf("Process ID: %d\n", pi.dwProcessId);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	return 0;
}
