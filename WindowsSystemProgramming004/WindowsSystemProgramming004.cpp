#include <windows.h>
#include <stdio.h>
#include <conio.h>

// This is failed: https://www.youtube.com/watch?v=jNGVjfSrrrs&list=PLVP36NCkbjNSgvFtqWtP0gKbFDtgz8RKJ&index=14

#define PRIVATE_NAMESPACE L"MyPrivateNamespace"

void Read(void* p);
void Write(void* p);

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
	HANDLE hBD = ::CreateBoundaryDescriptor(L"MyDescriptor", 0);
	if (!hBD) {
		PrintError("CreateBoundaryDescriptor", ::GetLastError());
		return 1;
	}

	BYTE sid[SECURITY_MAX_SID_SIZE];
	auto psid = (PSID)sid;
	DWORD sidLen = sizeof(sid);
	if (!::CreateWellKnownSid(WinLocalSystemSid, nullptr, psid, &sidLen)) {
		PrintError("CreateWellKnownSid", ::GetLastError());
		return 1;
	}
	if (!::AddSIDToBoundaryDescriptor(&hBD, psid)) {
		PrintError("AddSIDToBoundaryDescriptor", ::GetLastError());
		return 1;
	}

	HANDLE hNamespace = ::CreatePrivateNamespace(nullptr, hBD, PRIVATE_NAMESPACE);
	if (!hNamespace) {
		PrintError("CreatePrivateNamespace", ::GetLastError());
		hNamespace = ::OpenPrivateNamespace(hBD, PRIVATE_NAMESPACE);
		if (!hNamespace) {
			PrintError("OpenPrivateNamespace", ::GetLastError());
			return 1;
		}
	}

	HANDLE hMemMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 1 << 16, PRIVATE_NAMESPACE L"\\MySharedMemory");
	if (!hMemMap) {
		PrintError("CreateFileMapping", ::GetLastError());
		return 1;
	}
	void* p = ::MapViewOfFile(hMemMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	printf("Address: 0x%p\n", p);

	bool quit = false;
	while (!quit) {
		printf("1=Write, 2=Read, 3=Quit: ");
		int selection = _getch();
		printf("%c\n", selection);
		switch (selection) {
		case '1':
			Write(p);
			break;
		case '2':
			Read(p);
			break;
		case '3':
			quit = true;
			break;
		}
	}
	::UnmapViewOfFile(p);
	::CloseHandle(hMemMap);
	return 0;
}

void Read(void* p) {
	printf("%s\n", (const char*)p);
}

void Write(void* p) {
	printf(">> ");
	char buffer[128];
	gets_s(buffer);
	strcpy_s((char*)p, _countof(buffer), buffer);
}