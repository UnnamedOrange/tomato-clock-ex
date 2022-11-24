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

    public:
        /**
         * @brief Write once to the SPI.
         *
         * @param data One piece of data to write.
         * Note that type of data is int rather than spi_bits_t.
         * @return int Response from the SPI. Always -1.
         */
        int write(int data)
        {
            _select();
            int ret = _spi.write(data);
            _deselect();
            return ret;
        }
        /**
         * @brief Write a sequence to the SPI.
         *
         * @param data Pointer to the buffer to write.
         * Note that type of the elements is not constrainted.
         * @param size Size of the buffer.
         */
        void write(const void* data, size_t size)
        {
            _select();
            _spi.write(reinterpret_cast<const char*>(data),
                       static_cast<int>(size), nullptr, 0);
            _deselect();
        }
        /**
         * @brief Write a sequence to the SPI.
         *
         * @param native_array Data in a native array to write.
         */
        template <size_t size>
        void write(const spi_bits_t (&native_array)[size])
        {
            write(native_array, size);
        }
        /**
         * @brief Write a sequence to the SPI.
         *
         * @param array_object Data in an array like object to write.
         */
        template <typename array_like_t>
        void write(const array_like_t& array_object)
        {
            static_assert(std::is_same<typename array_like_t::value_type,
                                       spi_bits_t>::value,
                          "array_like_t must be an array of spi_bits_t.");
            write(array_object.data(), array_object.size());
        }
    };
} // namespace modules
