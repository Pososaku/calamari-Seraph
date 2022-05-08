#include "../../sdk/byond.h"
#include "../../sdk/events.h"
#include "../../sdk/plugin.h"
#include "../../sdk/imgui/imgui.h"
#include "aimbotipc.h"

#include <chrono>
#include <iostream>
#include <math.h>
#include <sstream>

#include <shared_mutex>
#include <unordered_map>


class Aimbot : public Plugin {
public:
	Aimbot();
	std::string GetName() override;
	void* IPCRecv(Plugin* from, void* data) override;
};
