#include "../../sdk/byond.h"
#include "../../sdk/events.h"
#include "../../sdk/plugin.h"
#include "../../sdk/imgui/imgui.h"
#include "../plugin.aimbot/aimbotipc.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class Radar : public Plugin {
public:
    Radar();
	std::string GetName() override;
};
