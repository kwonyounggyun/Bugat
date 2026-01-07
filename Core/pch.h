// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00 // Windows 10 이상을 타겟으로 설정
#endif

// add headers that you want to pre-compile here
#include "framework.h"

#endif //PCH_H
