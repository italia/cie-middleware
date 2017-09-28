#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlhost.h>
#include <atlstr.h>
#include "util/defines.h"
#include "util/array.h"
#include "util/allocator.h"
#include "util/log.h"
#include "util/funccallinfo.h"
#include "util/util.h"
#include "util/utilexception.h"
#include "util/defines_err.h"

#ifdef _DEBUG
#define ODS(s) OutputDebugString(s)
#else
#define ODS(s)
#endif

extern ByteArray baNXP_ATR;
extern ByteArray baGemalto_ATR;
extern ByteArray baGemalto2_ATR;
