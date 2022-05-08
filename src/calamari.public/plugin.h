#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct EventCall
{
	std::string Event;
    bool Overriding;
	void* Function;
	class Plugin* Plugin;
};

struct LogEntry
{
    std::string pluginName;
    time_t timestamp;
    std::string text;
};

class Plugin
{
public:
	std::string Name;
	HMODULE Library;
	std::vector<EventCall *> Events;
	__declspec(dllexport) Plugin();
	__declspec(dllexport) ~Plugin();
	__declspec(dllexport) virtual std::string GetName();
	__declspec(dllexport) bool AddEvent(std::string event, void *function, bool overriding = false);
	__declspec(dllexport) bool RemoveEvent(std::string event);
	__declspec(dllexport) bool AddKeyHandler(std::string name, int keyCode, void* handler);
	__declspec(dllexport) bool RemoveKeyHandler(std::string name);
	__declspec(dllexport) void Log(std::string text);
	__declspec(dllexport) std::string GetConfig(std::string what, std::string default = "");
	__declspec(dllexport) void SetConfig(std::string what, std::string data);
	__declspec(dllexport) void* IPCSend(std::string target, void* data);
	__declspec(dllexport) virtual void* IPCRecv(Plugin* from, void* data);
};