#pragma once


// keep this defined if you're going to release it to the public
#define RELEASE_PUBLIC

//#define DEBUG_CONSOLE


//#define CID_DEBUGGER
//#define PACKET_DEBUGGER

#ifdef RELEASE_PUBLIC
//	#define USE_BLACKLIST
//	#define USE_ACTIVATION
	#define USE_VMP
#endif