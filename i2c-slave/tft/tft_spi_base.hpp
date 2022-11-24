/**
 * @file tft_spi_base.hpp
 * @author UnnamedOrange
 * @brief Base class for SPI of TFT module on the tomato clock.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <cinttypes>

namespace modules
{
    /**
     * @brief Base class for SPI of TFT module on the tomato clock.
     * @note The SPI works with 8-bit data in mode 0. MISO is not used.
     */
    class _tft_spi_base
    {
    protected:
        constexpr static auto PIN_SPI_MOSI = PB_15;
        constexpr static auto PIN_SPI_MISO = NC;
        constexpr static auto PIN_SPI_SCLK = PB_13;
        constexpr static auto PIN_SPI_CS = PB_7;

    protected:
        /**
         * @brief The SPI works with 8-bit data.
         */
        constexpr static int spi_bits = 8;
        /**
         * @brief The SPI works in mode 0.
         */
        constexpr static int spi_mode = 0;
        /**
         * @brief The SPI works with 8-bit data.
         */
        using spi_bits_t = uint8_t;

    protected:
        constexpr static int spi_frequency = 20000000;
    };

    /**
     * @brief Dummy class for implementation of SPI.
     *
     * @tparam use_dma Specify @ref use_dma to use corresponding
     * implementation.
     */
    template <bool use_dma>
    class _tft_spi
    {
    };
} // namespace modules
