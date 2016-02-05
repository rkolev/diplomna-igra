#pragma once
#define _WINSOCKAPI_ //prevent windows.h from including winsock.h
#include <WinSock2.h>
#pragma comment(lib, "wsock32.lib")
#include <Math.h>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/vorbis.h>
#include <ClanLib/gl.h>
#include "enums.h"
#include "./util/external/pugixml.hpp"

typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long s64;


// TODO think of something
#define GRAVITY 0.1f
#define JUMP_BOOST 1.5f
#define PLAYER_ACTION_COUNT 3

using namespace std;

// TODO config
const u16 RESOLUTION_X = 1120;
const u16 RESOLUTION_Y = 544;

// TODO config
const u16 ServerPort = 3000;
const u16 ClientPort = 3001;
const u32 ProtocolId = 0x12345678;
const u32 TimeOut = 10000;

struct ObjectData
{
	// TODO what do with this?
	s8 name[256];
	s32 posX; 
	s32 posY;
};

struct MessageData
{
	// TODO can we improve this?
	u8 data[512];
	u32 size;
};

// TODO refactor
typedef std::list<ObjectData> ObjectDataList;