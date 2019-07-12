#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>

DWORD WINAPI ThreadFunction(LPVOID param) {
	for (INT i = 0; i < 100; ++i) {
		Sleep(500);
		printf("\tSecond thread: %d\n", i);
	}
	return 0;
}

//// __cdecl __stdcall
//unsigned long __stdcall ThreadFunction1(void* param) {
//    for (INT i = 0; i < 100; ++i) {
//        Sleep(500);
//        printf("\tSecond thread: %d\n", i);
//    }
//    return 0;
//}

VOID Search(LPCTSTR path, LPCTSTR searchStr/*, INT offset*/) {
	WIN32_FIND_DATA fd;
	TCHAR mask[MAX_PATH];
	_tcscpy_s(mask, MAX_PATH, path);
	_tcscat_s(mask, MAX_PATH, _T("\\*.*"));
	HANDLE hFind = FindFirstFile(path, &fd);

	//do {
		//Sleep(50);
		//if (_tcscmp(_T(".."), fd.cFileName) == 0 || _tcscmp(_T("."), fd.cFileName) == 0) {
			//continue;
		//}
	//if (hFind != INVALID_HANDLE_VALUE)
	//{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			//for (INT i = 0; i < offset * 2; ++i) {
			//	printf(" ");
			//}
			//TCHAR nextPath[MAX_PATH];
			//_tcscpy_s(nextPath, MAX_PATH, path);
			//_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
			//_tcscat_s(nextPath, MAX_PATH, _T("\\"));
			//Search(nextPath/*, offset + 1*/);
		}
		else {
#pragma region MyRegion


			//for (int i = 0; i < offset * 2; ++i) {
			//	printf(" ");
			//}
			//ULARGE_INTEGER ul;
			//ul.HighPart = fd.nFileSizeHigh;
			//ul.LowPart = fd.nFileSizeLow;
			//ULONGLONG fileSize = ul.QuadPart;
			//SetFilePointer(hFind, 0, 0, FILE_BEGIN);
			//_tprintf(_T("%*s%ul\n"), -70 + offset * 2, fd.cFileName, fileSize);
#pragma endregion

			LPTSTR fileText = ReadText(path);
			BOOL cResult = Contains(fileText, searchStr);
			if (cResult)
			{
				_tprintf_s("%s", path);
			}

		}
		//} while (FindNextFile(hFind, &fd));
	//}
}

LPTSTR ReadText(LPCTSTR path)
{
	HANDLE hFile = CreateFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD fileLenght = GetFileSize(hFile, NULL);
		DWORD bufferSize = (fileLenght + 1) * (sizeof(TCHAR));
		LPTSTR buffer = (LPTSTR)calloc(fileLenght + 1, sizeof(TCHAR));
		BOOL rfResult = ReadFile(hFile, buffer, bufferSize, NULL, NULL);
		CloseHandle(hFile);

		if (rfResult == TRUE)
		{
			return buffer;
		}
	}
	return NULL;
}

BOOL Contains(LPCTSTR str1, LPCTSTR str2)
{
	LPTSTR result = strstr(str1, str2);
	return result ? TRUE : FALSE;
}

int main() {
	TCHAR path[MAX_PATH];
	_tprintf_s(_T("Enter path..> "));
	_tscanf_s(_T("%s"), path, sizeof(path));

	TCHAR str[MAX_PATH];
	_tprintf_s(_T("Enter search str..> "));
	_tscanf_s(_T("%s"), str, sizeof(str));

	Search(path, str);

   //  CreateThreadpoolWork()
   /*HANDLE hThread = CreateThread(
	   NULL,
	   0,
	   ThreadFunction,
	   NULL,
	   0,
	   NULL
   );
   for (int i = 0; i < 20; ++i) {
	   Sleep(100);
	   printf("Main thread: %d\n", i);
   }
   WaitForSingleObject(hThread, INFINITE);

   TerminateThread(hThread, 0);*/

	system("pause");
	return 0;
}