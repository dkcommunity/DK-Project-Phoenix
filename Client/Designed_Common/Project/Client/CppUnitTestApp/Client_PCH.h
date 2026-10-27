////////////////////////////////////////////////////////////////////////////////
//	created:	2004/12/22
//	file base:	client_pch.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable : 4290)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)
#pragma warning(disable : 4786)

#pragma warning(push)

#include <algorithm>
#include <assert.h>
#include <bitset>
#include <deque>
#include <fstream>
#include <iostream.h>
#include <list>
#include <map>
#include <string>
#include <vector>

#pragma pack(push, 8)
#include <DDraw.h>
#include <Digitalv.h>
#include <MMSystem.h>
#include <windows.h>

#pragma pack(pop)
#include <fcntl.h>
#include <io.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#pragma warning(pop)

using std::bitset;
using std::deque;
using std::list;
using std::map;
using std::string;
using std::vector;
