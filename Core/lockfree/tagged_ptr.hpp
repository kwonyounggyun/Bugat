#pragma once

#ifndef BUGAT_LOCKFREE_TAGGED_PTR_INCLUDED
#define BUGAT_LOCKFREE_TAGGED_PTR_INCLUDED

#include <atomic>

#if defined(__x86_64__) && (defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) || ATOMIC_INT128_LOCK_FREE == 2)
#include "tagged_ptr_dcas.hpp"
#define TAGGED_PTR_MAX_ALIGN 16
#define CAN_USE_DWCAS 1
#else
#include "tagged_ptr_compress.hpp"
#define CAN_USE_DWCAS 0
#define TAGGED_PTR_MAX_ALIGN std::max_align_t
#endif

#endif