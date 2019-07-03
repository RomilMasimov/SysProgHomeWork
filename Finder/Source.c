#include <windows.h>
#include <stdio.h>
#include <tchar.h>

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

VOID foo(LPCWSTR path, INT offset) {
	WIN32_FIND_DATA fd;
	TCHAR mask[MAX_PATH];
	_tcscpy_s(mask, MAX_PATH, path);
	_tcscat_s(mask, MAX_PATH, _T("\\*.*"));
	HANDLE hFind = FindFirstFile(mask, &fd);
	do {
		//Sleep(50);
		if (_tcscmp(_T(".."), fd.cFileName) == 0 || _tcscmp(_T("."), fd.cFileName) == 0) {
			continue;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			for (INT i = 0; i < offset * 2; ++i) {
				printf(" ");
			}
			//_tprintf(_T("%*sDIR\n"), -70 + offset * 2, fd.cFileName);
			TCHAR nextPath[MAX_PATH];
			_tcscpy_s(nextPath, MAX_PATH, path);
			_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
			_tcscat_s(nextPath, MAX_PATH, _T("\\"));
			foo(nextPath, offset + 1);
		}
		else {
			for (int i = 0; i < offset * 2; ++i) {
				printf(" ");
			}
			ULARGE_INTEGER ul;
			ul.HighPart = fd.nFileSizeHigh;
			ul.LowPart = fd.nFileSizeLow;
			ULONGLONG fileSize = ul.QuadPart;

			LPVOID buffer[1024];
			DWORD bufferSize = (unsigned)_countof(buffer);
			DWORD readDataSize = 0;
			DWORD count = fileSize / bufferSize;
			for (int i = 0; i < count; i++)
			{
				SetFilePointer(hFind, bufferSize * i, 0, FILE_BEGIN);
				ReadFile(hFind, buffer, bufferSize, &readDataSize, NULL); 
				_tprintf(_T("%s"), buffer);
			}
			CloseHandle(hFind);

			_tprintf(_T("%*s%ul\n"), -70 + offset * 2, fd.cFileName, fileSize);
		}
	} while (FindNextFile(hFind, &fd));
}

int main() {
	TCHAR path[MAX_PATH];
	_tprintf_s(_T("Enter..> "));
	_tscanf_s(_T("%s"), path, (unsigned)_countof(path));
	foo(path, 0);

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


	return 0;
}