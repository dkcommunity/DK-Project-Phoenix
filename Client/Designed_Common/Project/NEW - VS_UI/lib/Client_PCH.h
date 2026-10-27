////////////////////////////////////////////////////////////////////////////////
//	created:	2004/12/22
//	file base:	client_pch.h11
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning(disable : 4290)
#pragma warning(disable : 4018)
#pragma warning(disable : 4244)
#pragma warning(disable : 4018)
#pragma warning(disable : 4786)
#pragma warning(disable : 4103)
#pragma warning(disable : 4049)

#pragma warning(error : 4067)

#pragma warning(push)

#include <algorithm>
#include <assert.h>
#include <bitset>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

#include <atltypes.h>
#include "ContentsFilter.h"
#pragma pack(push, 8)
#include <DDraw.h>

#include <MMSystem.h>
#include <Digitalv.h>
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
