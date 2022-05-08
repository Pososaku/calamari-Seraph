#pragma once

#include "../reactor.h"
#include "../../calamari.public/byond.h"
#include "../../calamari.public/events.h"
#include "../../calamari.public/plugin.h"
#include <d3d9.h>

typedef long(__stdcall* DXReset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
typedef long(__stdcall* DXEndScene)(LPDIRECT3DDEVICE9);
typedef long(__stdcall* DXPresent)(LPDIRECT3DDEVICE9, const RECT*, const RECT*, HWND, const RGNDATA*);

typedef HANDLE(__stdcall* CreateFile_A)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);

#ifdef PACKET_DEBUGGER
typedef bool(__thiscall* NetMsgDecode)(void *NetMsg, unsigned int key);
#endif

namespace Calamari::Hooks
{

	extern DXReset ResetFunc;
	extern DXEndScene EndSceneFunc;
	extern DXPresent PresentFunc;
	extern WNDPROC WndProcFunc;
	extern CreateFile_A CreateFileAFunc;
#ifdef PACKET_DEBUGGER
	extern NetMsgDecode NetMsgDecodeFunc;
#endif
	extern bool DXInitialized;

	BOOL EnumProcessModules(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
    long __stdcall Present(LPDIRECT3DDEVICE9 device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
    long __stdcall EndScene(LPDIRECT3DDEVICE9 device);
    long __stdcall Reset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* param);

#ifdef PACKET_DEBUGGER
	int __fastcall Decode(void* NetMsg, void* DONOTUSE, unsigned int key);
#endif

	int __fastcall MouseDown(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9);
	int __fastcall MouseUp(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9);
	int __fastcall Click(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9);
	int __fastcall MouseMove(void* DungClient, void* DONOTUSE, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9);

	int __fastcall PutText(void* pThis, void* DONOTUSE, char* a1, char* a2);
	void __fastcall Stat(void* pThis, void* DONOTUSE, char const* a1, char const* a2, struct Cid a3, unsigned short a4);

	void* __fastcall DungClientCtor(void* pThis);

	bool __fastcall IsByondMember(void* DungClient, void* DONOTUSE, long who_cares);

	unsigned int __cdecl Byond32(unsigned int a1, unsigned int a2, unsigned int a3);

	LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool __stdcall GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize);
	HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
}
