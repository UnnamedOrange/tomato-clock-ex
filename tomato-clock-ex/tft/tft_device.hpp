/**
 * @file tft_device.hpp
 * @author UnnamedOrange
 * @brief Low level driver for TFT module.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include "tft_auxiliary_pins.hpp"
#include "tft_spi_base.hpp"
#include "tft_spi_impl_1.hpp"
#include "tft_spi_impl_2.hpp"

namespace modules
{
    /**
     * @brief Low level driver for TFT module.
     *
     * @tparam use_dma true for DMA implementation, false for non-DMA
     * implementation.
     */
    template <bool use_dma>
    class tft_device : public _tft_auxiliary_pins, public _tft_spi<use_dma>
    {
    };
} // namespace modules
