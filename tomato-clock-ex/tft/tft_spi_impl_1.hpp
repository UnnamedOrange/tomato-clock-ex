/**
 * @file tft_spi_impl_1.hpp
 * @author UnnamedOrange
 * @brief Implement SPI of TFT module with mbed::SPI.
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
     * @brief Implement SPI of TFT module with mbed::SPI.
     * @note The SPI works with 8-bit data in mode 0. MISO is not used.
     */
    template <>
    class _tft_spi<false> : public _tft_spi_base
    {
    private:
        mbed::SPI _spi{PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCLK};
        // Set CS manually or it fails in release profile.
        mbed::DigitalOut _cs{PIN_SPI_CS};

    public:
        _tft_spi()
        {
            _spi.format(spi_bits, spi_mode);
            _spi.frequency(spi_frequency);
        }

    private:
        /**
         * @brief Select the SPI.
         */
        void _select()
        {
            _spi.lock();
            _cs = 0;
        }
        /**
         * @brief Deselect the SPI.
         */
        void _deselect()
        {
            _cs = 1;
            _spi.unlock();
        }
        /**
         * @brief Write once to the SPI. Lock should be set @b mannually.
         *
         * @param data Data to write.
         */
        void _write_without_lock(spi_bits_t data)
        {
            _spi.write(data);
        }

    public:
        /**
         * @brief Write a sequence to the SPI. Lock is set @b automatically.
         *
         * @param iterable An iterable object with elements of type spi_bits_t.
         */
        template <typename iterable_t>
        void write(const iterable_t& iterable)
        {
            _select();
            for (const spi_bits_t& data : iterable)
                _spi.write(data);
            _deselect();
        }
        /**
         * @brief Write a sequence to the SPI. Lock is set @b automatically.
         *
         * @param iterable An iterable object with elements of type @ref
         * spi_bits_t.
         */
        void write(const std::initializer_list<spi_bits_t>& list)
        {
            write<std::initializer_list<spi_bits_t>>(list);
        }
    };
} // namespace modules
