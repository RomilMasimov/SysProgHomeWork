#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>

struct SearchParams
{
	TCHAR path[MAX_PATH];
	TCHAR searchStr[MAX_PATH];
	CRITICAL_SECTION criticalSection;
};

DWORD StartCLI();
DWORD WINAPI CLI(LPVOID param);
HANDLE* StartThreadSearch(LPCTSTR path, LPCTSTR searchStr);
DWORD WINAPI ThreadSearch(LPVOID threadParams);
LPTSTR ReadText(LPCTSTR path);
BOOL WriteTextLine(LPCTSTR path, LPCTSTR str);
BOOL Contains(LPCTSTR str1, LPCTSTR str2);

int main() {
	//TCHAR path[MAX_PATH];
	//_tprintf_s(_T("Enter path..> "));
	//_tscanf_s(_T("%s"), path, _countof(path));
	//
	//TCHAR str[MAX_PATH];
	//_tprintf_s(_T("Enter search str..> "));
	//_tscanf_s(_T("%s"), str, _countof(str));

	StartCLI();

	//  CreateThreadpoolWork()
	//HANDLE hThread = CreateThread(
	 //   NULL,
	 //   0,
	 //   ThreadFunction,
	 //   NULL,
	 //   0,
	 //   NULL
	//);
	//for (int i = 0; i < 20; ++i) {
	 //   Sleep(100);
	 //   printf("Main thread: %d\n", i);
	//}
	//WaitForSingleObject(hThread, INFINITE);
	//TerminateThread(hThread, 0);


	 //system("pause");
	return 0;
}

DWORD StartCLI()
{
	HANDLE hThread = CreateThread
	(
		NULL,
		0,
		CLI,
		NULL,
		0,
		NULL
	);
	WaitForSingleObject(hThread, INFINITE);
	return 0;
}

DWORD WINAPI CLI(LPVOID param)
{
	DWORD num = 0;
	HANDLE* hThreads = NULL;
	while (TRUE)
	{
		if (num == 0)
		{
			TCHAR path[MAX_PATH];
			_tprintf_s(_T("Enter path..> "));
			_tscanf_s(_T("%s"), path, _countof(path));

			TCHAR str[MAX_PATH];
			_tprintf_s(_T("Enter search str..> "));
			_tscanf_s(_T("%s"), str, _countof(path));

			hThreads = StartThreadSearch(path, str);

			_tprintf_s(_T("Search started\n"));
			num = 1;
		}
		else if (num == 1)
		{
			for (DWORD i = 0; i < _msize(hThreads) / sizeof(HANDLE); i++)
			{
				BOOL code = GetExitCodeThread(&hThreads[i], NULL);
				if (code != NULL)
				{
					_tprintf_s(_T("OK\n"));
				}
			}
		}
	}
	return 0;
}

HANDLE* StartThreadSearch(LPCTSTR path, LPCTSTR searchStr)
{
	WIN32_FIND_DATA fd;
	TCHAR dirPath[MAX_PATH];
	_tcscpy_s(dirPath, MAX_PATH, path);
	_tcscat_s(dirPath, MAX_PATH, _T("*"));
	HANDLE hFind = FindFirstFile(dirPath, &fd);

	DWORD dirCount = DirectoryCount(path);
	if (dirCount == 0)
	{
		LPTSTR nextPath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
		_tcscpy_s(nextPath, MAX_PATH, path);
		_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
		_tcscat_s(nextPath, MAX_PATH, _T("\\"));

		struct SearchParams* params = malloc(sizeof(struct SearchParams));
		_tcscpy_s(params->path, MAX_PATH, nextPath);
		_tcscpy_s(params->searchStr, MAX_PATH, searchStr);
		InitializeCriticalSection(&(params->criticalSection));

		HANDLE hThread = CreateThread
		(
			NULL,
			0,
			ThreadSearch,
			params,
			0,
			NULL
		);
		return hThread;
	}

	HANDLE* hThreads = (HANDLE*)calloc(dirCount, sizeof(HANDLE));
	do
	{
		DWORD count = 0;

		if (hFind == INVALID_HANDLE_VALUE ||
			_tcscmp(_T(".."), fd.cFileName) == 0 ||
			_tcscmp(_T("."), fd.cFileName) == 0)
		{
			continue;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			LPTSTR nextPath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
			_tcscpy_s(nextPath, MAX_PATH, path);
			_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
			_tcscat_s(nextPath, MAX_PATH, _T("\\"));

			struct SearchParams* params = malloc(sizeof(struct SearchParams));
			_tcscpy_s(params->path, MAX_PATH, nextPath);
			_tcscpy_s(params->searchStr, MAX_PATH, searchStr);
			InitializeCriticalSection(&(params->criticalSection));

			hThreads[count] = CreateThread
			(
				NULL,
				0,
				ThreadSearch,
				params,
				0,
				NULL
			);
			++count;
		}

	} while (FindNextFile(hFind, &fd));
	FindClose(hFind);
	return hThreads;
}

DWORD WINAPI ThreadSearch(LPVOID param)
{
	struct SearchParams params = *((struct SearchParams*)param);
	LPCTSTR path = params.path;
	LPCTSTR searchStr = params.searchStr;

	WIN32_FIND_DATA fd;
	//LPTSTR filePath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
	TCHAR filePath[MAX_PATH];
	_tcscpy_s(filePath, MAX_PATH, path);
	_tcscat_s(filePath, MAX_PATH, _T("*"));
	HANDLE hFind = FindFirstFile(filePath, &fd);

	do
	{
		if (hFind == INVALID_HANDLE_VALUE ||
			_tcscmp(_T(".."), fd.cFileName) == 0 ||
			_tcscmp(_T("."), fd.cFileName) == 0)
		{
			continue;
		}
		LPTSTR nextPath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
		_tcscpy_s(nextPath, MAX_PATH, path);
		_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_tcscat_s(nextPath, MAX_PATH, _T("\\"));
			struct SearchParams* nextParams = malloc(sizeof(struct SearchParams));
			_tcscpy_s(nextParams->path, MAX_PATH, nextPath);
			_tcscpy_s(nextParams->searchStr, MAX_PATH, searchStr);
			nextParams->criticalSection = params.criticalSection;
			ThreadSearch(&nextParams);
			free(nextParams);
		}
		else
		{
			LPTSTR fileText = ReadText(nextPath);
			if (fileText != NULL)
			{
				BOOL cResult = Contains(fileText, searchStr);
				free(fileText);
				if (cResult)
				{
					EnterCriticalSection(&(params.criticalSection));
					WriteTextLine("C:\\Temp\\result.txt", nextPath);
					LeaveCriticalSection(&(params.criticalSection));
				}
			}

		}
		//_tcscpy_s(filePath, MAX_PATH, path);
	} while (FindNextFile(hFind, &fd));
	FindClose(hFind);

	return 0;
}

DWORD DirectoryCount(LPCTSTR path)
{
	DWORD result = 0;

	WIN32_FIND_DATA fd;
	TCHAR dirPath[MAX_PATH];
	_tcscpy_s(dirPath, MAX_PATH, path);
	_tcscat_s(dirPath, MAX_PATH, _T("*"));
	HANDLE hFind = FindFirstFile(dirPath, &fd);

	do
	{
		if (hFind == INVALID_HANDLE_VALUE ||
			_tcscmp(_T(".."), fd.cFileName) == 0 ||
			_tcscmp(_T("."), fd.cFileName) == 0)
		{
			continue;
		}
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			++result;
		}

	} while (FindNextFile(hFind, &fd));
	FindClose(hFind);

	return result;
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
		DWORD strSize = lstrlen(str) * sizeof(TCHAR);
		TCHAR nextLne = _T('\n');

		SetFilePointer(hFile, 0, NULL, FILE_END);
		BOOL wfResult = WriteFile(hFile, str, strSize, NULL, NULL);
		wfResult = WriteFile(hFile, &nextLne, sizeof(TCHAR), NULL, NULL);
		CloseHandle(hFile);

		return wfResult;
	}
	return FALSE;
}

BOOL Contains(LPCTSTR str1, LPCTSTR str2)
{
	LPTSTR result = strstr(str1, str2);
	return result ? TRUE : FALSE;
}