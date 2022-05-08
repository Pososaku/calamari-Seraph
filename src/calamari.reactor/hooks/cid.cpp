#include "hooks.h"

CreateFile_A Calamari::Hooks::CreateFileAFunc = NULL;

bool __stdcall Calamari::Hooks::GetVolumeInformationA(
	LPCSTR lpRootPathName,
	LPSTR lpVolumeNameBuffer,
	DWORD nVolumeNameSize,
	LPDWORD lpVolumeSerialNumber,
	LPDWORD lpMaximumComponentLength,
	LPDWORD lpFileSystemFlags,
	LPSTR lpFileSystemNameBuffer,
	DWORD nFileSystemNameSize)
{
	VMP_BEGIN("Calamari::Hooks::GetVolumeInformationA");
	bool result; // eax

	result = strstr(lpRootPathName, VMPSTR("C:\\"));
	if (lpVolumeSerialNumber)
	{
#ifdef CID_DEBUGGER
		printf("%lX", Calamari::Reactor::Serial);
#endif
		*lpVolumeSerialNumber = Calamari::Reactor::Serial;
	}
	if (!result)
	{
		return result;
	}

	result = 1;
	return result;
	VMP_END();
}

HANDLE __stdcall Calamari::Hooks::CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	VMP_BEGIN("Calamari::Hooks::CreateFileA");
	if (strstr(lpFileName, VMPSTR("\\\\.\\PhysicalDrive")) || strstr(lpFileName, VMPSTR("\\\\.\\SCSI")))
	{
		return (HANDLE)-1;
	}
	return CreateFileAFunc(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
	VMP_END();
}
