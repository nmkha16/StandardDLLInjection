#include "Injector.h"
#include <filesystem>

// constructor
Injector::Injector(const char* target, const char* dllPath) {
	this->targetProc = target;
	this->dllPath = dllPath;
}

Injector::Injector()
{
}

// destructor
Injector::~Injector(){
	delete[] dllPath;
	delete[] dllFullPath;
	delete[] targetProc;
}

DWORD Injector::GetProcIDByName(const char* target){
	DWORD procID = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {
			do {
				// convert wchar to bstr
				_bstr_t bStr(procEntry.szExeFile);
				// then convert bstr to const char* for comparision
				const char* procEntryExePath(bStr);

				if (!strcmp(procEntryExePath, target)) {
					procID = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procID;
}

void Injector::GetProcID() {
	cout << "Looking for " << targetProc << " pid...";
	while (!pid) {
		pid = GetProcIDByName(targetProc);
		Sleep(100);
	}
	cout << "Found " << targetProc << " pid: " << pid << endl;
}

void Injector::GetFullPath(const char* relativePath) {
	dllFullPath = new char[MAX_PATH];
	GetFullPathNameA(relativePath, MAX_PATH, dllFullPath, nullptr);
}

bool Injector::IsFileExist(const char* fileName) {
	// ifstream will close itself upon existing function
	return static_cast<bool>(std::ifstream(fileName));
}

LPTHREAD_START_ROUTINE Injector::AllocWriteDll(HANDLE hProc) {
	unsigned int writeLen = 0;
	LPVOID loadLibAddr = nullptr;
	
	lpDllAddr = VirtualAllocEx(hProc, nullptr, strlen(dllPath)+1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (lpDllAddr) {
		WriteProcessMemory(hProc, lpDllAddr, dllPath, strlen(dllPath)+1, nullptr);

		loadLibAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

		// free the memory allocation for our dll path
		VirtualFreeEx(hProc, lpDllAddr, strlen(this->dllPath) + 1, MEM_RELEASE);
	}

	return (LPTHREAD_START_ROUTINE)loadLibAddr;
}

void Injector::Inject() {
	// check if dll file exists
	if (!IsFileExist(dllPath)) {
		cout << "Cannot find dll. Please check your directory!" << endl;
		return;
	}
	//// get full path for dll
	//GetFullPath(dllPath);
	//dllPath = dllFullPath;

	// get pid for target process
	GetProcID();

	// open handle to target process
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	HANDLE rThread = nullptr;
	if (hProc) {
		LPTHREAD_START_ROUTINE lpStartExecAddr = AllocWriteDll(hProc);
		if (lpStartExecAddr) {
			rThread = CreateRemoteThread(hProc, nullptr, 0, lpStartExecAddr, lpDllAddr, 0, nullptr);

			if (rThread) {
				// wait until inject execution complete
				WaitForSingleObject(rThread, INFINITE);
			}
		}
	}
	CloseHandle(hProc);
}

