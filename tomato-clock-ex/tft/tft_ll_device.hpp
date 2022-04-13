/**
 * @file tft_ll_device.hpp
 * @author UnnamedOrange
 * @brief Low-level class for TFT module on the tomato clock.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <cinttypes>
#include <functional>

namespace modules
{
    /**
     * @brief Low-level class for TFT module on the tomato clock.
     * This class is almost hardware-independent.
     * @note The SPI works with 8-bit data in mode 3. MISO is not used.
     */
    class tft_ll_device
    {
    private:
        constexpr static auto PIN_LED = PB_1;
        mbed::DigitalOut _led{PIN_LED};

        constexpr static auto PIN_RST = PC_2;
        mbed::DigitalOut _rst{PIN_RST};

        constexpr static auto PIN_RS = PB_2;
        mbed::DigitalOut _rs{PIN_RS}; // Index or data.
        enum rs_t
        {
            index = 0,
            data = 1,
        };

        constexpr static auto PIN_SPI_MOSI = PB_15;
        constexpr static auto PIN_SPI_MISO = NC;
        constexpr static auto PIN_SPI_SCLK = PB_13;
        constexpr static auto PIN_SPI_CS = PB_7;
        mbed::SPI _tft_spi{PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCLK, PIN_SPI_CS,
                           mbed::use_gpio_ssel};

    public:
        /**
         * @brief The SPI works with 8-bit data.
         */
        constexpr static int spi_bits = 8;
        /**
         * @brief The SPI works in mode 3.
         */
        constexpr static int spi_mode = 3;
        /**
         * @brief The SPI works with 8-bit data.
         */
        using spi_bits_t = uint8_t;
        tft_ll_device()
        {
            _tft_spi.format(spi_bits, spi_mode);
        }

    public:
        /**
         * @brief Turn the backlight on or off.
         */
        void set_led(bool is_on)
        {
            _led = is_on;
        }

    public:
        /**
         * @brief Hardware reset the TFT.
         * @note This function is to sleep for 150ms;
         */
        void reset()
        {
            _rst = 0;
            rtos::ThisThread::sleep_for(100ms);
            _rst = 1;
            rtos::ThisThread::sleep_for(50ms);
        }

    public:
        /**
         * @brief Tell the TFT to receive index via SPI.
         */
        void index_mode()
        {
            _rs = static_cast<int>(rs_t::index);
        }
        /**
         * @brief Tell the TFT to receive data via SPI.
         */
        void data_mode()
        {
            _rs = static_cast<int>(rs_t::data);
        }

    private:
        /**
         * @brief Lock the SPI.
         */
        void _lock()
        {
            _tft_spi.lock();
        }
        /**
         * @brief Unlock the SPI.
         */
        void _unlock()
        {
            _tft_spi.unlock();
        }
        /**
         * @brief Write once to the SPI. Lock should be set @b mannually.
         *
         * @param data Data to write.
         */
        void _write_without_lock(spi_bits_t data)
        {
            _tft_spi.write(data);
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
            _lock();
            for (const spi_bits_t& data : iterable)
                _tft_spi.write(data);
            _unlock();
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
