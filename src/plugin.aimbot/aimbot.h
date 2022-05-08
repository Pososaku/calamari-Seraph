#include "../calamari.public/byond.h"
#include "../calamari.public/events.h"
#include "../calamari.public/plugin.h"
#include "../imgui/imgui.h"
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
