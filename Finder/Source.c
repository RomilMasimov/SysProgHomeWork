#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <malloc.h>

struct SearchParams
{
	TCHAR searchPath[MAX_PATH];
	TCHAR searchStr[MAX_PATH];
	TCHAR resultPath[MAX_PATH];
	CRITICAL_SECTION criticalSection;
};
struct CheckStatusParam
{
	DWORD* cliState;
	HANDLE* hThreads;
};

HANDLE SartCheckSearchStatus(struct CheckStatusParam* param);
DWORD WINAPI CheckSearchStatus(struct CheckStatusParam* param);
HANDLE StartCLI();
DWORD WINAPI CLI(LPVOID param);
HANDLE* StartThreadSearch(LPTSTR searchPath, LPTSTR resultPath, LPTSTR searchStr);
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

	HANDLE hThread = StartCLI();
	WaitForSingleObject(hThread, INFINITE);

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

HANDLE StartCLI()
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
	return hThread;
}

DWORD WINAPI CLI(LPVOID param)
{
	DWORD cliState = 0;
	HANDLE* hThreads = NULL;

	TCHAR searchPath[MAX_PATH];
	TCHAR resultPath[MAX_PATH];
	TCHAR str[MAX_PATH];
	while (TRUE)
	{
		if (cliState == 0)
		{
			_tprintf_s(_T("Enter search path..> "));
			_tscanf_s(_T("%s"), searchPath, _countof(searchPath));

			_tprintf_s(_T("Enter result path..> "));
			_tscanf_s(_T("%s"), resultPath, _countof(resultPath));

			_tprintf_s(_T("Enter search string..> "));
			_tscanf_s(_T("%s"), str, _countof(str));

			hThreads = StartThreadSearch(searchPath, resultPath, str);
			if (hThreads != NULL)
			{
				struct CheckStatusParam* param = malloc(sizeof(struct CheckStatusParam));
				param->cliState = &cliState;
				param->hThreads = hThreads;
				SartCheckSearchStatus(param);

				_tprintf_s(_T("Search started\n"));
				cliState = 1;
			}
		}
		else if(cliState == 1)
		{
			TCHAR chr[2];
			_tprintf_s(_T("Stop(s)..> "));
			_tscanf_s(_T("%s"), chr, _countof(chr));
			if (chr[0] == _T('s'))
			{
				WORD count = _msize(hThreads) / sizeof(HANDLE); 
				for (DWORD i = 0; i < count; ++i)
				{
					TerminateThread(hThreads[i], NULL);
				}
				cliState = 2;
			}
		
		}
		else if (cliState == 2)
		{
			_tprintf_s(_T("Search ended\nResult path: %sresult.txt\n"), resultPath);
			cliState = 0;
		}

		//if (hThreads != NULL)
		//{
		//	DWORD count = _msize(hThreads) / sizeof(HANDLE);
		//	DWORD finishedCount = 0;
		//	for (DWORD i = 0; i < count; ++i)
		//	{
		//		DWORD code;
		//		if (GetExitCodeThread(hThreads[i], &code))
		//		{
		//			if (code != STILL_ACTIVE)
		//			{
		//				++finishedCount;
		//			}
		//		}
		//	}
		//	if (finishedCount == count)
		//	{
		//		_tprintf_s(_T("Search ended\nResult path: %s\\result.txt\n"), resultPath);
		//		num = 0;
		//	}
		//}
	}
	return 0;
}

HANDLE SartCheckSearchStatus(struct CheckStatusParam* param)
{
	HANDLE hThread = CreateThread
	(
		NULL,
		0,
		CheckSearchStatus,
		param,
		0,
		NULL
	);
	return hThread;
}

DWORD WINAPI CheckSearchStatus(struct CheckStatusParam* param)
{
	DWORD count = _msize(param->hThreads) / sizeof(HANDLE);
	while (TRUE)
	{
		DWORD finishedCount = 0;
		for (DWORD i = 0; i < count; ++i)
		{
			DWORD code;
			if (GetExitCodeThread(param->hThreads[i], &code))
			{
				if (code != STILL_ACTIVE)
				{
					++finishedCount;
				}
			}
		}
		if (finishedCount == count)
		{
			*(param->cliState) = 2;
			return 0;
		}
	}
	return -1;
}

HANDLE* StartThreadSearch(LPTSTR searchPath, LPTSTR resultPath, LPTSTR searchStr)
{
	WIN32_FIND_DATA fd;
	TCHAR dirPath[MAX_PATH];
	_tcscpy_s(dirPath, MAX_PATH, searchPath);
	_tcscat_s(dirPath, MAX_PATH, _T("*"));
	HANDLE hFind = FindFirstFile(dirPath, &fd);

	DWORD dirCount = DirectoryCount(searchPath);
	if (dirCount == 0)
	{
		//LPTSTR nextPath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
		//_tcscpy_s(nextPath, MAX_PATH, path);
		//_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
		//_tcscat_s(nextPath, MAX_PATH, _T("\\"));

		struct SearchParams* params = malloc(sizeof(struct SearchParams));
		_tcscpy_s(params->searchPath, MAX_PATH, searchPath);
		_tcscpy_s(params->searchStr, MAX_PATH, searchStr);
		_tcscpy_s(params->resultPath, MAX_PATH, resultPath);
		InitializeCriticalSection(&(params->criticalSection));

		HANDLE* hThread = (HANDLE*)malloc(sizeof(HANDLE));
		*hThread = CreateThread
		(
			NULL,
			0,
			ThreadSearch,
			params,
			0,
			NULL
		);
		FindClose(hFind);
		return hThread;
	}

	HANDLE* hThreads = (HANDLE*)calloc(dirCount, sizeof(HANDLE));
	DWORD count = 0;
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
			LPTSTR nextPath = (LPTSTR)calloc(MAX_PATH, sizeof(TCHAR));
			_tcscpy_s(nextPath, MAX_PATH, searchPath);
			_tcscat_s(nextPath, MAX_PATH, fd.cFileName);
			_tcscat_s(nextPath, MAX_PATH, _T("\\"));

			struct SearchParams* params = malloc(sizeof(struct SearchParams));
			_tcscpy_s(params->searchPath, MAX_PATH, nextPath);
			_tcscpy_s(params->resultPath, MAX_PATH, resultPath);
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
	LPTSTR path = params.searchPath;
	TCHAR resultFile[MAX_PATH];
	_tcscpy_s(resultFile, MAX_PATH, params.resultPath);
	_tcscat_s(resultFile, MAX_PATH, "result.txt");
	LPTSTR searchStr = params.searchStr;

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
			_tcscpy_s(nextParams->searchPath, MAX_PATH, nextPath);
			_tcscpy_s(nextParams->resultPath, MAX_PATH, params.resultPath);
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
					WriteTextLine(resultFile, nextPath);
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