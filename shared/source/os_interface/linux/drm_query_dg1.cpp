/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/os_interface/linux/drm_engine_mapper.h"
#include "shared/source/os_interface/linux/memory_info_impl.h"

#include "drm_neo.h"

#include <fstream>

namespace NEO {

int Drm::getMaxGpuFrequency(HardwareInfo &hwInfo, int &maxGpuFrequency) {
    maxGpuFrequency = 0;
    std::string clockSysFsPath = getSysFsPciPath();

    clockSysFsPath += "/gt_max_freq_mhz";

    std::ifstream ifs(clockSysFsPath.c_str(), std::ifstream::in);
    if (ifs.fail()) {
        return -1;
    }

    ifs >> maxGpuFrequency;
    ifs.close();
    return 0;
}

bool Drm::queryEngineInfo() {
    return true;
}

bool Drm::queryMemoryInfo() {
    auto length = 0;
    auto dataQuery = this->query(DRM_I915_QUERY_MEMORY_REGIONS, length);
    auto data = reinterpret_cast<drm_i915_query_memory_regions *>(dataQuery.get());
    if (data) {
        this->memoryInfo.reset(new MemoryInfoImpl(data->regions, data->num_regions));
        return true;
    }
    return false;
}

unsigned int Drm::bindDrmContext(uint32_t drmContextId, uint32_t deviceIndex, aub_stream::EngineType engineType) {
    return DrmEngineMapper::engineNodeMap(engineType);
}

int Drm::bindBufferObject(uint32_t vmHandleId, BufferObject *bo) {
    return 0;
}

int Drm::unbindBufferObject(uint32_t vmHandleId, BufferObject *bo) {
    return 0;
}

} // namespace NEO
