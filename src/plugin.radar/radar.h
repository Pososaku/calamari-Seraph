#include "../calamari.public/byond.h"
#include "../calamari.public/events.h"
#include "../calamari.public/plugin.h"
#include "../plugin.aimbot/aimbotipc.h"
#include "../imgui/imgui.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class Radar : public Plugin {
public:
    Radar();
	std::string GetName() override;
};
