/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief I2C Slave Example.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <array>
#include <chrono>
#include <functional>
#include <random>

#include "mbed.h"

#include "tft/tft_debug_console.hpp"

namespace utils
{
    modules::tft_debug_console console;

    /**
     * @brief Use HAL_GetTick() to implement now().
     */
    class HAL_Clock : public Kernel::Clock
    {
    public:
        static time_point now() { return time_point(duration(HAL_GetTick())); }
    };
    using system_clock = HAL_Clock;

/**
 * @brief Do not cache the current tick count because the implementation is
 * updated.
 */
#define g_now() (system_clock::now())

    /**
     * @brief Eliminate jitters and manage the callback function of a button.
     *
     * @remarks Now this class based on IRQs. But this feature is not ruled.
     */
    class button_handler : protected InterruptIn
    {
    private:
        std::function<void()> m_callback;
        system_clock::time_point m_last_pressed{};
        void on_fall()
        {
            // Respond only if the interval is longer than the threshold.
            if (g_now() - m_last_pressed >= 150ms)
            {
                if (m_callback)
                    m_callback();
                m_last_pressed = g_now();
            }
        }

    public:
        using InterruptIn::InterruptIn;
        void set_callback(std::function<void()> callback)
        {
            m_callback = callback;
            InterruptIn::fall(std::bind(&button_handler::on_fall, this));
        }
    };

} // namespace utils

int num;
I2CSlave i2c{PB_9, PB_8};

int main()
{
    using namespace utils;
    using system_clock = Kernel::Clock;

    // Init I2C.
    {
    }

    // Init buttons.
    std::array<bool, 3> downs{};
    button_handler button1{PC_11};
    button_handler button2{PC_13};
    button_handler button3{PC_12};
    button1.set_callback([&]() { downs[0] = true; });
    button2.set_callback([&]() { downs[1] = true; });
    button3.set_callback([&]() { downs[2] = true; });

    // TODO: Set callbacks.
    std::array<std::function<void()>, 3> on_button{};

    while (true)
    {
        for (size_t i = 0; i < downs.size(); i++)
            if (downs[i])
            {
                if (on_button[i])
                    on_button[i]();
                downs[i] = false;
            }
    }
}
