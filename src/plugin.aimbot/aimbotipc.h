#define AIMBOT_GET_TARGET		0x0
#define AIMBOT_SET_TARGET		0x1
#define AIMBOT_UNSET_TARGET		0x2
#define AIMBOT_ADD_FRIEND		0x3
#define AIMBOT_REMOVE_FRIEND	0x4
#define AIMBOT_GET_FRIENDS		0x5

struct AimbotIPC
{
    unsigned char msg;
	void* data;
};