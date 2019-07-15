#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>

LPTSTR ReadText(LPCTSTR path);
BOOL WriteTextLine(LPCTSTR path, LPCTSTR str);
BOOL Contains(LPCTSTR str1, LPCTSTR str2);

struct SearchParams
{
	LPCTSTR path;
	LPCTSTR searchStr;
};

DWORD WINAPI StartThreadSearch(LPVOID param) {
	//int num = *((int*)param);
	//SearchParams params = *((SearchParams*)param);

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

int main() {
	TCHAR path[MAX_PATH];
	_tprintf_s(_T("Enter path..> "));
	_tscanf_s(_T("%s"), path, _countof(path));
	
	TCHAR str[MAX_PATH];
	_tprintf_s(_T("Enter search str..> "));
	_tscanf_s(_T("%s"), str, _countof(path));

	Search(path, str);
	//WriteText(path, str);

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

VOID Search(LPCTSTR path, LPCTSTR searchStr/*, INT offset*/)
{
	WIN32_FIND_DATA fd;
	//LPTSTR filePath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
	TCHAR filePath[MAX_PATH];
	_tcscpy_s(filePath, MAX_PATH, path);
	_tcscat_s(filePath, MAX_PATH, _T("*"));
	HANDLE hFind = FindFirstFile(filePath, &fd);

	do 
	{
		//Sleep(50);
		if (hFind == INVALID_HANDLE_VALUE ||
			_tcscmp(_T(".."), fd.cFileName) == 0 ||
			_tcscmp(_T("."), fd.cFileName) == 0) 
		{
			continue;
		}
	//if (hFind != INVALID_HANDLE_VALUE)
		//{
		LPTSTR nextPath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
		_tcscpy_s(nextPath, MAX_PATH, path);
		_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
		{
			//for (INT i = 0; i < offset * 2; ++i) {
			//	printf(" ");
			//}	
			_tcscat_s(nextPath, MAX_PATH, _T("\\"));
			Search(nextPath, searchStr/*, offset + 1*/);
		}
		else
		{
			//for (int i = 0; i < offset * 2; ++i) {
			//	printf(" ");
			//}
			//ULARGE_INTEGER ul;
			//ul.HighPart = fd.nFileSizeHigh;
			//ul.LowPart = fd.nFileSizeLow;
			//ULONGLONG fileSize = ul.QuadPart;
			//SetFilePointer(hFind, 0, 0, FILE_BEGIN);
			//_tprintf(_T("%*s%ul\n"), -70 + offset * 2, fd.cFileName, fileSize);

			LPTSTR fileText = ReadText(nextPath);
			if (fileText != NULL)
			{
				BOOL cResult = Contains(fileText, searchStr);
				free(fileText);
				if (cResult)
				{
					WriteTextLine("C:\\Users\\Romil\\Desktop\\result.txt", nextPath);
				}
			}

		}
		_tcscpy_s(filePath, MAX_PATH, path);
	} while (FindNextFile(hFind, &fd));
	FindClose(hFind);
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

BOOL WriteTextLine(LPCTSTR path, LPCTSTR str)
{
	HANDLE hFile = CreateFile(
		path,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		LPTSTR writeStr = (LPTSTR)calloc(lstrlen(str) + 1, sizeof(TCHAR));
		DWORD strSize = (lstrlen(str) + 1) * (sizeof(TCHAR));
		_tcscat_s(writeStr, strSize, str);
		*(writeStr + lstrlen(writeStr)) = _T('\n');

		SetFilePointer(hFile, 0, NULL, FILE_END);
		BOOL wfResult = WriteFile(hFile, writeStr, strSize, NULL, NULL);
		CloseHandle(hFile);
		free(writeStr);

		return wfResult;
	}
	return FALSE;
}

BOOL Contains(LPCTSTR str1, LPCTSTR str2)
{
	LPTSTR result = strstr(str1, str2);
	return result ? TRUE : FALSE;
}