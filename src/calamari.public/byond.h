#pragma once
#include "../common/Pocket.h"
#include "../common/vmp.h"
#include <d3d9.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <windows.h>

struct Cid {
    unsigned char t1 = 0x0;
    unsigned long t2 = 0xFFFF;
};

struct Position {
    int X = 0;
    int Y = 0;
};

typedef void*(__thiscall* DungClient__Ctor)(void* pThis);
typedef int(__thiscall* GenMouseDownCommand)(void*, struct Cid a1, char const* a2, short a3, short a4, short a5, short a6, char a7, char const* a8, char const* a9);
typedef int(__thiscall* GenMouseUpCommand)(void*, struct Cid a1, char const*, short, short, short, short, char, char const*, char const*);
typedef int(__thiscall* GenClickCommand)(void*, struct Cid a1, char const*, short, short, short, short, char, char const*, char const*);
typedef int(__thiscall* GenMouseMoveCommand)(void*, struct Cid a1, char const*, short, short, short, short, char, char const*, char const*);
typedef DWORD*(__thiscall* GetCidLoc)(DWORD* DungClient, struct Cid* a1, DWORD a2, DWORD a3);
typedef char*(__thiscall* GetCidName)(void* DungClient, struct Cid a1);
typedef unsigned int(__fastcall* GetServerPort)(void* DungClient);
typedef char*(__fastcall* GetServerIP)(void* DungClient, void*, char* str);
typedef Cid(__thiscall* GetPlayerCid)(void* DungClient, Cid* a2);
typedef int(__thiscall* CIOutputCtrlPutText)(void* pThis, char* a1, char* a2);
typedef void(__thiscall* DSAddStat)(void*, char const*, char const*, struct Cid, unsigned short);
typedef void(__thiscall* CommandEvent)(void*, unsigned char, unsigned short, unsigned char);
typedef bool(__thiscall* IsByondMember)(void*, long);
typedef unsigned char(__thiscall* ParseCommand)(void*, char const*, unsigned char, unsigned char);
typedef int (*GetByondVersion)();
typedef int (*GetByondBuild)();
typedef unsigned int(__cdecl* Byond32)(unsigned int a1, unsigned int a2, unsigned int a3);
typedef char(__stdcall* GenerateCid)(char *a1, size_t a2);

namespace Calamari::Byond {
	extern void* ViewRangeXLoc;
	extern void* ViewRangeYLoc;
	extern void* Byond32Loc;

	extern LPDIRECT3DDEVICE9 DXDevice;
	extern HWND DirectXWnd;
	extern HWND InputWnd;

	extern void* DungClient;

	extern DungClient__Ctor DungClientCtor;
	extern GenMouseDownCommand GenMouseDownCommandFunc;
	extern GenMouseUpCommand GenMouseUpCommandFunc;
	extern GenClickCommand GenClickCommandFunc;
	extern GenMouseMoveCommand GenMouseMoveCommandFunc;
	extern GetCidLoc GetCidLocFunc;
	extern GetCidName GetCidNameFunc;
	extern GetServerPort GetServerPortFunc;
	extern GetServerIP GetServerIPFunc;
	extern GetPlayerCid GetPlayerCidFunc;
	extern CIOutputCtrlPutText CIOutputCtrlPutTextFunc;
	extern DSAddStat DSAddStatFunc;
	extern CommandEvent CommandEventFunc;
	extern ParseCommand ParseCommandFunc;
	extern Byond32 Byond32Func;
	extern IsByondMember DCIsByondMemberFunc;
	extern IsByondMember DPIsByondMemberFunc;

	extern int Major;
	extern int Minor;

	void LoadByondFunctions();

	int __declspec(dllexport) ByondMajor();
	int __declspec(dllexport) ByondMinor();
	std::string __declspec(dllexport) ByondVersion();
	std::unique_ptr<Position> __declspec(dllexport) GetViewRange();
	std::unique_ptr<Position> __declspec(dllexport) ConvertIntToPosition(int pos);
	std::unique_ptr<Position> __declspec(dllexport) GetTilePositionFromAbsolute(Position centerPosition, Position objectPosition);
	std::unique_ptr<Position> __declspec(dllexport) GetCidTileCoords(struct Cid target);
	std::unique_ptr<Position> __declspec(dllexport) GetCidAbsCoords(struct Cid target);
	std::unique_ptr<Position> __declspec(dllexport) CenterPosition();
	std::string __declspec(dllexport) CidName(Cid cid);
	std::unique_ptr<Cid> __declspec(dllexport) PlayerCid();
	void __declspec(dllexport) Click(Cid cid);
	std::string __declspec(dllexport) ServerIP();
	unsigned int __declspec(dllexport) ServerPort();
	void __declspec(dllexport) RunCommand(std::string cmd);
	std::unordered_map<std::shared_ptr<Cid>, std::string> __declspec(dllexport) GetObjectsOnScreen();
	std::unordered_map<std::shared_ptr<Cid>, std::string> __declspec(dllexport) GetMobsOnScreen();
}
