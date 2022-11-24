/**
 * @file tft_spi_impl_2.hpp
 * @author UnnamedOrange
 * @brief Implement SPI of TFT module with DMA provided by HAL.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <cinttypes>

#include "tft_spi_base.hpp"

namespace modules
{
    /**
     * @brief Implement SPI of TFT module with DMA provided by HAL.
     * @note The SPI works with 8-bit data in mode 0. MISO is not used.
     */
    template <>
    class _tft_spi<true> : public _tft_spi_base
    {
        // TODO: Implement.
    };
} // namespace modules
