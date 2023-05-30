#pragma once
#include <iostream>
#include <Windows.h>
#include <comdef.h>
#include <TlHelp32.h>
#include <fstream>

using std::cout;
using std::cin;
using std::endl;




class Injector {
private:
	// attributes
	void* lpDllAddr;
	const char* dllPath;
	char* dllFullPath;
	const char* targetProc;
	DWORD pid = 0;
	// methods
	DWORD GetProcIDByName(const char* target);
	void GetProcID();
	void GetFullPath(const char* reletivePath);
	bool IsFileExist(const char* fileName);
	LPTHREAD_START_ROUTINE AllocWriteDll(HANDLE hProc);

public:
	// methods
	// constructor
	Injector(const char* target, const char* dllPath);
	Injector();
	// destructor
	~Injector();
	// using reflective injection method to load dll from memory rather than disk file. Thanks to Stephen Fewer
	void Inject();
};