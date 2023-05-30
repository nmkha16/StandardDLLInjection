#include "Injector.h"

#define DEFAULT_EXTENSION L".exe"
const char* GetFileNameFromPath(char* input);

int main() {
	//----------------------------------------------------------------------------------------------
	// SET OPENFILE DIALOGUE PROPERTIES
	OPENFILENAME    ofn;
	const wchar_t* FilterSpec = L"Application Files(*.exe)\0*.exe\0All Files(*.*)\0*.*\0";
	const wchar_t* Title = L"Open....";
	wchar_t szFileName[MAX_PATH];
	wchar_t szFileTitle[MAX_PATH];
	int Result;
	char exeFilePath[MAX_PATH]; // Selected file and path
	char dllFilePath[MAX_PATH];

	*szFileName = 0;
	*szFileTitle = 0;

	/* fill in non-variant fields of OPENFILENAME struct. */
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = GetFocus();
	ofn.lpstrFilter = FilterSpec;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = L"."; // Initial directory.
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrTitle = Title;
	ofn.lpstrDefExt = DEFAULT_EXTENSION;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	//----------------------------------------------------------------------------------------------

	cout << "Selecting Application path" << endl;
	Title = L"Selecting Application...";
	size_t numOfCharConverted = 0;
	if (GetOpenFileName((LPOPENFILENAME)&ofn)) {
		wcstombs_s(&numOfCharConverted,exeFilePath, ofn.lpstrFile, MAX_PATH);
	}
	
	cout << "Selecting DLL path" << endl;
	Title = L"Selecting DLL...";
	if (GetOpenFileName((LPOPENFILENAME)&ofn)) {
		wcstombs_s(&numOfCharConverted,dllFilePath, ofn.lpstrFile, MAX_PATH);
	}


	Injector inject(GetFileNameFromPath(exeFilePath), dllFilePath);
	inject.Inject();

	return 0;
}

const char* GetFileNameFromPath(char* input) {
	char* pfile;
	pfile = input + strlen(input);
	for (; pfile > input; pfile--)
	{
		if ((*pfile == '\\') || (*pfile == '/'))
		{
			pfile++;
			break;
		}
	}
	return pfile;
}