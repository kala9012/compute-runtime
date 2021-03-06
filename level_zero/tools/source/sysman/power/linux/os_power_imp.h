/*
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "shared/source/helpers/non_copyable_or_moveable.h"

#include "level_zero/tools/source/sysman/power/os_power.h"

namespace L0 {

class SysfsAccess;
class PlatformMonitoringTech;
class LinuxPowerImp : public OsPower, NEO::NonCopyableOrMovableClass {
  public:
    ze_result_t getEnergyCounter(uint64_t &energy) override;
    ze_result_t getEnergyThreshold(zes_energy_threshold_t *pThreshold) override;
    ze_result_t setEnergyThreshold(double threshold) override;
    bool isPowerModuleSupported() override;
    LinuxPowerImp(OsSysman *pOsSysman);
    LinuxPowerImp() = default;
    ~LinuxPowerImp() override = default;

  protected:
    SysfsAccess *pSysfsAccess = nullptr;
    PlatformMonitoringTech *pPmt = nullptr;
};
} // namespace L0
