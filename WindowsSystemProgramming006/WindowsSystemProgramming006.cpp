
#include <windows.h>
#include <stdio.h>

// Works

int main()
{
	HANDLE hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	printf("Handle: 0x%p\n", hEvent);

	DWORD targetProcessId = 32444; // Replace it.
	HANDLE hProcess = ::OpenProcess(PROCESS_DUP_HANDLE, FALSE, targetProcessId);
	if (!hProcess) {
		printf("OpenProcess failed: %d\n", ::GetLastError());
		return 1;
	}
	HANDLE hTarget;
	if (::DuplicateHandle(::GetCurrentProcess(), hEvent, hProcess, &hTarget, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		printf("Handle duplicated: 0x%p\n", hTarget);
		::CloseHandle(hTarget);
	}
	else {
		printf("DuplicateHandle failed: %d\n", ::GetLastError());
	}
	return 0;
}
