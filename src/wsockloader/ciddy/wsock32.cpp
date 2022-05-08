#pragma once
#include "stdafx.h"
#include "../../common/vmp.h"

// wrappit gunk
HINSTANCE hLThis = nullptr;
HINSTANCE hL = nullptr;
FARPROC p[75] = { nullptr };
// wrappit gunk ends here
typedef void(*init_calamari)(HMODULE h);


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	VMP_BEGIN("DllMain");
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		#pragma region wrappit stuff
		hLThis = hModule;
		hL = LoadLibraryA(VMPSTR("C:\\Windows\\System32\\wsock32.dll"));
		if (!hL) return false;

		p[0] = GetProcAddress(hL, "AcceptEx");
		p[1] = GetProcAddress(hL, "EnumProtocolsA");
		p[2] = GetProcAddress(hL, "EnumProtocolsW");
		p[3] = GetProcAddress(hL, "GetAcceptExSockaddrs");
		p[4] = GetProcAddress(hL, "GetAddressByNameA");
		p[5] = GetProcAddress(hL, "GetAddressByNameW");
		p[6] = GetProcAddress(hL, "GetNameByTypeA");
		p[7] = GetProcAddress(hL, "GetNameByTypeW");
		p[8] = GetProcAddress(hL, "GetServiceA");
		p[9] = GetProcAddress(hL, "GetServiceW");
		p[10] = GetProcAddress(hL, "GetTypeByNameA");
		p[11] = GetProcAddress(hL, "GetTypeByNameW");
		p[12] = GetProcAddress(hL, "MigrateWinsockConfiguration");
		p[13] = GetProcAddress(hL, "NPLoadNameSpaces");
		p[14] = GetProcAddress(hL, "SetServiceA");
		p[15] = GetProcAddress(hL, "SetServiceW");
		p[16] = GetProcAddress(hL, "TransmitFile");
		p[17] = GetProcAddress(hL, "WEP");
		p[18] = GetProcAddress(hL, "WSAAsyncGetHostByAddr");
		p[19] = GetProcAddress(hL, "WSAAsyncGetHostByName");
		p[20] = GetProcAddress(hL, "WSAAsyncGetProtoByName");
		p[21] = GetProcAddress(hL, "WSAAsyncGetProtoByNumber");
		p[22] = GetProcAddress(hL, "WSAAsyncGetServByName");
		p[23] = GetProcAddress(hL, "WSAAsyncGetServByPort");
		p[24] = GetProcAddress(hL, "WSAAsyncSelect");
		p[25] = GetProcAddress(hL, "WSACancelAsyncRequest");
		p[26] = GetProcAddress(hL, "WSACancelBlockingCall");
		p[27] = GetProcAddress(hL, "WSACleanup");
		p[28] = GetProcAddress(hL, "WSAGetLastError");
		p[29] = GetProcAddress(hL, "WSAIsBlocking");
		p[30] = GetProcAddress(hL, "WSARecvEx");
		p[31] = GetProcAddress(hL, "WSASetBlockingHook");
		p[32] = GetProcAddress(hL, "WSASetLastError");
		p[33] = GetProcAddress(hL, "WSAStartup");
		p[34] = GetProcAddress(hL, "WSAUnhookBlockingHook");
		p[35] = GetProcAddress(hL, "WSApSetPostRoutine");
		p[36] = GetProcAddress(hL, "__WSAFDIsSet");
		p[37] = GetProcAddress(hL, "accept");
		p[38] = GetProcAddress(hL, "bind");
		p[39] = GetProcAddress(hL, "closesocket");
		p[40] = GetProcAddress(hL, "connect");
		p[41] = GetProcAddress(hL, "dn_expand");
		p[42] = GetProcAddress(hL, "gethostbyaddr");
		p[43] = GetProcAddress(hL, "gethostbyname");
		p[44] = GetProcAddress(hL, "gethostname");
		p[45] = GetProcAddress(hL, "getnetbyname");
		p[46] = GetProcAddress(hL, "getpeername");
		p[47] = GetProcAddress(hL, "getprotobyname");
		p[48] = GetProcAddress(hL, "getprotobynumber");
		p[49] = GetProcAddress(hL, "getservbyname");
		p[50] = GetProcAddress(hL, "getservbyport");
		p[51] = GetProcAddress(hL, "getsockname");
		p[52] = GetProcAddress(hL, "getsockopt");
		p[53] = GetProcAddress(hL, "htonl");
		p[54] = GetProcAddress(hL, "htons");
		p[55] = GetProcAddress(hL, "inet_addr");
		p[56] = GetProcAddress(hL, "inet_network");
		p[57] = GetProcAddress(hL, "inet_ntoa");
		p[58] = GetProcAddress(hL, "ioctlsocket");
		p[59] = GetProcAddress(hL, "listen");
		p[60] = GetProcAddress(hL, "ntohl");
		p[61] = GetProcAddress(hL, "ntohs");
		p[62] = GetProcAddress(hL, "rcmd");
		p[63] = GetProcAddress(hL, "recv");
		p[64] = GetProcAddress(hL, "recvfrom");
		p[65] = GetProcAddress(hL, "rexec");
		p[66] = GetProcAddress(hL, "rresvport");
		p[67] = GetProcAddress(hL, "s_perror");
		p[68] = GetProcAddress(hL, "select");
		p[69] = GetProcAddress(hL, "send");
		p[70] = GetProcAddress(hL, "sendto");
		p[71] = GetProcAddress(hL, "sethostname");
		p[72] = GetProcAddress(hL, "setsockopt");
		p[73] = GetProcAddress(hL, "shutdown");
		p[74] = GetProcAddress(hL, "socket");
		#pragma endregion

		HMODULE calamari = LoadLibraryA(VMPSTR("calamari.dll"));
		init_calamari init = (init_calamari)GetProcAddress(calamari, VMPSTR("a"));
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)init, hModule, 0, 0);
		SuspendThread(hModule);
	}

	return TRUE;
	VMP_END();
}


#pragma region more wrappit stuff, yay
// AcceptEx
extern "C" __declspec(naked) void __stdcall __E__0__()
{
	__asm
	{
		jmp p[0 * 4];
	}
}

// EnumProtocolsA
extern "C" __declspec(naked) void __stdcall __E__1__()
{
	__asm
	{
		jmp p[1 * 4];
	}
}

// EnumProtocolsW
extern "C" __declspec(naked) void __stdcall __E__2__()
{
	__asm
	{
		jmp p[2 * 4];
	}
}

// GetAcceptExSockaddrs
extern "C" __declspec(naked) void __stdcall __E__3__()
{
	__asm
	{
		jmp p[3 * 4];
	}
}

// GetAddressByNameA
extern "C" __declspec(naked) void __stdcall __E__4__()
{
	__asm
	{
		jmp p[4 * 4];
	}
}

// GetAddressByNameW
extern "C" __declspec(naked) void __stdcall __E__5__()
{
	__asm
	{
		jmp p[5 * 4];
	}
}

// GetNameByTypeA
extern "C" __declspec(naked) void __stdcall __E__6__()
{
	__asm
	{
		jmp p[6 * 4];
	}
}

// GetNameByTypeW
extern "C" __declspec(naked) void __stdcall __E__7__()
{
	__asm
	{
		jmp p[7 * 4];
	}
}

// GetServiceA
extern "C" __declspec(naked) void __stdcall __E__8__()
{
	__asm
	{
		jmp p[8 * 4];
	}
}

// GetServiceW
extern "C" __declspec(naked) void __stdcall __E__9__()
{
	__asm
	{
		jmp p[9 * 4];
	}
}

// GetTypeByNameA
extern "C" __declspec(naked) void __stdcall __E__10__()
{
	__asm
	{
		jmp p[10 * 4];
	}
}

// GetTypeByNameW
extern "C" __declspec(naked) void __stdcall __E__11__()
{
	__asm
	{
		jmp p[11 * 4];
	}
}

// MigrateWinsockConfiguration
extern "C" __declspec(naked) void __stdcall __E__12__()
{
	__asm
	{
		jmp p[12 * 4];
	}
}

// NPLoadNameSpaces
extern "C" __declspec(naked) void __stdcall __E__13__()
{
	__asm
	{
		jmp p[13 * 4];
	}
}

// SetServiceA
extern "C" __declspec(naked) void __stdcall __E__14__()
{
	__asm
	{
		jmp p[14 * 4];
	}
}

// SetServiceW
extern "C" __declspec(naked) void __stdcall __E__15__()
{
	__asm
	{
		jmp p[15 * 4];
	}
}

// TransmitFile
extern "C" __declspec(naked) void __stdcall __E__16__()
{
	__asm
	{
		jmp p[16 * 4];
	}
}

// WEP
extern "C" __declspec(naked) void __stdcall __E__17__()
{
	__asm
	{
		jmp p[17 * 4];
	}
}

// WSAAsyncGetHostByAddr
extern "C" __declspec(naked) void __stdcall __E__18__()
{
	__asm
	{
		jmp p[18 * 4];
	}
}

// WSAAsyncGetHostByName
extern "C" __declspec(naked) void __stdcall __E__19__()
{
	__asm
	{
		jmp p[19 * 4];
	}
}

// WSAAsyncGetProtoByName
extern "C" __declspec(naked) void __stdcall __E__20__()
{
	__asm
	{
		jmp p[20 * 4];
	}
}

// WSAAsyncGetProtoByNumber
extern "C" __declspec(naked) void __stdcall __E__21__()
{
	__asm
	{
		jmp p[21 * 4];
	}
}

// WSAAsyncGetServByName
extern "C" __declspec(naked) void __stdcall __E__22__()
{
	__asm
	{
		jmp p[22 * 4];
	}
}

// WSAAsyncGetServByPort
extern "C" __declspec(naked) void __stdcall __E__23__()
{
	__asm
	{
		jmp p[23 * 4];
	}
}

// WSAAsyncSelect
extern "C" __declspec(naked) void __stdcall __E__24__()
{
	__asm
	{
		jmp p[24 * 4];
	}
}

// WSACancelAsyncRequest
extern "C" __declspec(naked) void __stdcall __E__25__()
{
	__asm
	{
		jmp p[25 * 4];
	}
}

// WSACancelBlockingCall
extern "C" __declspec(naked) void __stdcall __E__26__()
{
	__asm
	{
		jmp p[26 * 4];
	}
}

// WSACleanup
extern "C" __declspec(naked) void __stdcall __E__27__()
{
	__asm
	{
		jmp p[27 * 4];
	}
}

// WSAGetLastError
extern "C" __declspec(naked) void __stdcall __E__28__()
{
	__asm
	{
		jmp p[28 * 4];
	}
}

// WSAIsBlocking
extern "C" __declspec(naked) void __stdcall __E__29__()
{
	__asm
	{
		jmp p[29 * 4];
	}
}

// WSARecvEx
extern "C" __declspec(naked) void __stdcall __E__30__()
{
	__asm
	{
		jmp p[30 * 4];
	}
}

// WSASetBlockingHook
extern "C" __declspec(naked) void __stdcall __E__31__()
{
	__asm
	{
		jmp p[31 * 4];
	}
}

// WSASetLastError
extern "C" __declspec(naked) void __stdcall __E__32__()
{
	__asm
	{
		jmp p[32 * 4];
	}
}

// WSAStartup
extern "C" __declspec(naked) void __stdcall __E__33__()
{
	__asm
	{
		jmp p[33 * 4];
	}
}

// WSAUnhookBlockingHook
extern "C" __declspec(naked) void __stdcall __E__34__()
{
	__asm
	{
		jmp p[34 * 4];
	}
}

// WSApSetPostRoutine
extern "C" __declspec(naked) void __stdcall __E__35__()
{
	__asm
	{
		jmp p[35 * 4];
	}
}

// __WSAFDIsSet
extern "C" __declspec(naked) void __stdcall __E__36__()
{
	__asm
	{
		jmp p[36 * 4];
	}
}

// accept
extern "C" __declspec(naked) void __stdcall __E__37__()
{
	__asm
	{
		jmp p[37 * 4];
	}
}

// bind
extern "C" __declspec(naked) void __stdcall __E__38__()
{
	__asm
	{
		jmp p[38 * 4];
	}
}

// closesocket
extern "C" __declspec(naked) void __stdcall __E__39__()
{
	__asm
	{
		jmp p[39 * 4];
	}
}

// connect
extern "C" __declspec(naked) void __stdcall __E__40__()
{
	__asm
	{
		jmp p[40 * 4];
	}
}

// dn_expand
extern "C" __declspec(naked) void __stdcall __E__41__()
{
	__asm
	{
		jmp p[41 * 4];
	}
}

// gethostbyaddr
extern "C" __declspec(naked) void __stdcall __E__42__()
{
	__asm
	{
		jmp p[42 * 4];
	}
}

// gethostbyname
extern "C" __declspec(naked) void __stdcall __E__43__()
{
	__asm
	{
		jmp p[43 * 4];
	}
}

// gethostname
extern "C" __declspec(naked) void __stdcall __E__44__()
{
	__asm
	{
		jmp p[44 * 4];
	}
}

// getnetbyname
extern "C" __declspec(naked) void __stdcall __E__45__()
{
	__asm
	{
		jmp p[45 * 4];
	}
}

// getpeername
extern "C" __declspec(naked) void __stdcall __E__46__()
{
	__asm
	{
		jmp p[46 * 4];
	}
}

// getprotobyname
extern "C" __declspec(naked) void __stdcall __E__47__()
{
	__asm
	{
		jmp p[47 * 4];
	}
}

// getprotobynumber
extern "C" __declspec(naked) void __stdcall __E__48__()
{
	__asm
	{
		jmp p[48 * 4];
	}
}

// getservbyname
extern "C" __declspec(naked) void __stdcall __E__49__()
{
	__asm
	{
		jmp p[49 * 4];
	}
}

// getservbyport
extern "C" __declspec(naked) void __stdcall __E__50__()
{
	__asm
	{
		jmp p[50 * 4];
	}
}

// getsockname
extern "C" __declspec(naked) void __stdcall __E__51__()
{
	__asm
	{
		jmp p[51 * 4];
	}
}

// getsockopt
extern "C" __declspec(naked) void __stdcall __E__52__()
{
	__asm
	{
		jmp p[52 * 4];
	}
}

// htonl
extern "C" __declspec(naked) void __stdcall __E__53__()
{
	__asm
	{
		jmp p[53 * 4];
	}
}

// htons
extern "C" __declspec(naked) void __stdcall __E__54__()
{
	__asm
	{
		jmp p[54 * 4];
	}
}

// inet_addr
extern "C" __declspec(naked) void __stdcall __E__55__()
{
	__asm
	{
		jmp p[55 * 4];
	}
}

// inet_network
extern "C" __declspec(naked) void __stdcall __E__56__()
{
	__asm
	{
		jmp p[56 * 4];
	}
}

// inet_ntoa
extern "C" __declspec(naked) void __stdcall __E__57__()
{
	__asm
	{
		jmp p[57 * 4];
	}
}

// ioctlsocket
extern "C" __declspec(naked) void __stdcall __E__58__()
{
	__asm
	{
		jmp p[58 * 4];
	}
}

// listen
extern "C" __declspec(naked) void __stdcall __E__59__()
{
	__asm
	{
		jmp p[59 * 4];
	}
}

// ntohl
extern "C" __declspec(naked) void __stdcall __E__60__()
{
	__asm
	{
		jmp p[60 * 4];
	}
}

// ntohs
extern "C" __declspec(naked) void __stdcall __E__61__()
{
	__asm
	{
		jmp p[61 * 4];
	}
}

// rcmd
extern "C" __declspec(naked) void __stdcall __E__62__()
{
	__asm
	{
		jmp p[62 * 4];
	}
}

// recv
extern "C" __declspec(naked) void __stdcall __E__63__()
{
	__asm
	{
		jmp p[63 * 4];
	}
}

// recvfrom
extern "C" __declspec(naked) void __stdcall __E__64__()
{
	__asm
	{
		jmp p[64 * 4];
	}
}

// rexec
extern "C" __declspec(naked) void __stdcall __E__65__()
{
	__asm
	{
		jmp p[65 * 4];
	}
}

// rresvport
extern "C" __declspec(naked) void __stdcall __E__66__()
{
	__asm
	{
		jmp p[66 * 4];
	}
}

// s_perror
extern "C" __declspec(naked) void __stdcall __E__67__()
{
	__asm
	{
		jmp p[67 * 4];
	}
}

// select
extern "C" __declspec(naked) void __stdcall __E__68__()
{
	__asm
	{
		jmp p[68 * 4];
	}
}

// send
extern "C" __declspec(naked) void __stdcall __E__69__()
{
	__asm
	{
		jmp p[69 * 4];
	}
}

// sendto
extern "C" __declspec(naked) void __stdcall __E__70__()
{
	__asm
	{
		jmp p[70 * 4];
	}
}

// sethostname
extern "C" __declspec(naked) void __stdcall __E__71__()
{
	__asm
	{
		jmp p[71 * 4];
	}
}

// setsockopt
extern "C" __declspec(naked) void __stdcall __E__72__()
{
	__asm
	{
		jmp p[72 * 4];
	}
}

// shutdown
extern "C" __declspec(naked) void __stdcall __E__73__()
{
	__asm
	{
		jmp p[73 * 4];
	}
}

// socket
extern "C" __declspec(naked) void __stdcall __E__74__()
{
	__asm
	{
		jmp p[74 * 4];
	}
}

#pragma endregion
