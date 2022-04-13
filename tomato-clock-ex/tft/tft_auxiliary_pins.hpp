/**
 * @file tft_auxiliary_pins.hpp.hpp
 * @author UnnamedOrange
 * @brief Utility functions for auxiliary pins (except SPI pins) for TFT module
 * on the tomato clock.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <cinttypes>

namespace modules
{
    enum tft_rs_t
    {
        index = 0,
        data = 1,
    };

    /**
     * @brief Utility functions for auxiliary pins (except SPI pins) for TFT
     * module on the tomato clock.
     */
    class _tft_auxiliary_pins
    {
    private:
        constexpr static auto PIN_LED = PB_1;
        mbed::DigitalOut _led{PIN_LED};

        constexpr static auto PIN_RST = PC_2;
        mbed::DigitalOut _rst{PIN_RST};

        constexpr static auto PIN_RS = PB_2;
        mbed::DigitalOut _rs{PIN_RS}; // Index or data.

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
         * @note This function is to sleep for 125ms;
         */
        void reset()
        {
            _rst = 0;
            rtos::ThisThread::sleep_for(5ms);
            _rst = 1;
            rtos::ThisThread::sleep_for(120ms);
        }

    public:
        /**
         * @brief Tell the TFT to receive index or data via SPI.
         *
         * @param mode tft_rs_t::index or tft_rs_t::data.
         */
        void set_mode(tft_rs_t mode)
        {
            _rs = static_cast<int>(mode);
        }
    };
} // namespace modules
