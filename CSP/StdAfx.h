#pragma once

#include <windows.h>
#include <atlbase.h>
#include <atlhost.h>
#include <atlstr.h>
#include "defines.h"
#include "array.h"
#include "allocator.h"
#include "log.h"
#include "funccallinfo.h"
#include "util.h"
#include "utilexception.h"
#include "defines_err.h"

#ifdef _DEBUG
#define ODS(s) OutputDebugString(s)
#else
#define ODS(s)
#endif
