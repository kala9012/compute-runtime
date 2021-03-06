/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/tools/source/sysman/memory/linux/os_memory_imp.h"

namespace L0 {

ze_result_t LinuxMemoryImp::getMemorySize(uint64_t &maxSize, uint64_t &allocSize) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t LinuxMemoryImp::getMemHealth(zes_mem_health_t &memHealth) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

OsMemory *OsMemory::create(OsSysman *pOsSysman) {
    LinuxMemoryImp *pLinuxMemoryImp = new LinuxMemoryImp();
    return static_cast<OsMemory *>(pLinuxMemoryImp);
}

} // namespace L0
