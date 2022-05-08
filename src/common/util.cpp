#include "util.h"
#include "../calamari.public/byond.h"

BOOL CALLBACK EnumChildProc(
	_In_ HWND hwnd,
	_In_ LPARAM lParam)
{
	VMP_BEGIN("EnumChildProc");
	std::vector<HWND> &windows = *(std::vector<HWND> *)lParam;
	char buf[0x100];
	GetClassNameA(hwnd, buf, 0x100);
	if (strcmp(buf, "Static") == 0 && IsWindowVisible(hwnd))
		windows.push_back(hwnd);
	return true;
	VMP_END();
}

// this selects the visible Static window with the biggest area.
void FindDirectXWindow(HWND parentHWND)
{
	VMP_BEGIN("FindDirectXWindow");
	std::vector<HWND> windows;
	EnumChildWindows(parentHWND, EnumChildProc, (LPARAM)&windows);
	HWND candidate = NULL;
	for (HWND win : windows)
	{
		if (IsWindowVisible(win))
		{
			if (!candidate)
				candidate = win;
			else
			{
				RECT cRect;
				RECT nRect;
				if (GetWindowRect(win, &nRect) && GetWindowRect(candidate, &cRect))
				{
					int nWidth = nRect.right - nRect.left;
					int nHeight = nRect.bottom - nRect.top;
					int cWidth = cRect.right - cRect.left;
					int cHeight = cRect.bottom - cRect.top;
					if ((nWidth * nHeight) > (cWidth * cHeight))
						candidate = win;
				}
			}
		}
	}
	if (candidate)
		Calamari::Byond::DirectXWnd = candidate;
#ifdef _DEBUG
	printf("DirectXWnd is %p\n", Calamari::Byond::DirectXWnd);
#endif
	VMP_END();
}

struct HandleData
{
	unsigned long ProcessID;
	HWND WindowHandle;
};

inline BOOL IsMainWindow(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	VMP_BEGIN("EnumWindowsCallback");
	HandleData &data = *(HandleData *)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);
	if (data.ProcessID != process_id || !IsMainWindow(handle))
		return TRUE;
	data.WindowHandle = handle;
	return FALSE;
	VMP_END();
}

HWND FindMainWindow(unsigned long process_id)
{
	VMP_BEGIN("FindMainWindow");
	HandleData data;
	data.ProcessID = process_id;
	data.WindowHandle = 0;
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.WindowHandle;
	VMP_END();
}

std::vector<std::string> FindFilesWithExt(std::string glob)
{
	VMP_BEGIN("FindFilesWithExt");
	std::vector<std::string> out;
	HANDLE hFind;
	WIN32_FIND_DATA FindFileData;
	hFind = FindFirstFileA(glob.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			out.push_back(std::string(FindFileData.cFileName));
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
	}
	return out;
	VMP_END();
}
