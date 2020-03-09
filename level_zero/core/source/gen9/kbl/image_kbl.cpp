/*
 * Copyright (C) 2019-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/gen9/hw_cmds.h"
#include "shared/source/gen9/hw_info.h"

#include "level_zero/core/source/image_hw.inl"

namespace L0 {

template <>
struct ImageProductFamily<IGFX_KABYLAKE> : public ImageCoreFamily<IGFX_GEN9_CORE> {
    using ImageCoreFamily::ImageCoreFamily;
};

static ImagePopulateFactory<IGFX_KABYLAKE, ImageProductFamily<IGFX_KABYLAKE>> populateKBL;

} // namespace L0