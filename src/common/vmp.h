#pragma once

#include "calamari_config.h"

#ifdef USE_VMP
#include "VMProtectSDK.h"
#define VMP_BEGIN(x) VMProtectBegin(x)
#define VMP_END() VMProtectEnd()
#define VMPSTR(x) VMProtectDecryptStringA(x)
#else
#define VMP_BEGIN(x)
#define VMP_END()
#define VMPSTR(x) x
#endif
