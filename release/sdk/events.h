#pragma once

#include "byond.h"
#include "plugin.h"
#include <string>
#include <unordered_map>
#include <d3d9.h>
#include <d3dtypes.h>

#define EVENT_RENDER		"Render"
#define EVENT_LATERENDER	"LateRender"
#define EVENT_PUTTEXT		"PutText"
#define EVENT_STAT			"Stat"
#define EVENT_CLICK			"Click"
#define EVENT_MOUSEMOVE		"MouseMove"
#define EVENT_MOUSEUP		"MouseUp"
#define EVENT_MOUSEDOWN		"MouseDown"

#define BUTTON_LEFT		0
#define BUTTON_RIGHT	1
#define BUTTON_MIDDLE	2

// NOTE: RenderEvents will either happen in DX Present or DX EndScene, depending on user confiuguration.
typedef void(*RenderEvent)(Plugin* plugin, LPDIRECT3DDEVICE9 dev, const RECT* client);
typedef void(*LateRenderEvent)(Plugin* plugin, LPDIRECT3DDEVICE9 dev, const RECT* client);
typedef void(*PutTextEvent)(Plugin* plugin, std::string text);
typedef void(*StatEvent)(Plugin* plugin, std::string menu, std::string text);
// If any of these return NULL, or are non-overriding, the functions will continue normally.
typedef Cid*(*ClickEvent)(Plugin* plugin, struct Cid target, int button, std::unordered_map<std::string, bool> modifiers);
typedef Cid*(*MouseMoveEvent)(Plugin* plugin, struct Cid target);
typedef Cid*(*MouseUpEvent)(Plugin* plugin, struct Cid target, int button, std::unordered_map<std::string, bool> modifiers);
typedef Cid*(*MouseDownEvent)(Plugin* plugin, struct Cid target, int button, std::unordered_map<std::string, bool> modifiers);
