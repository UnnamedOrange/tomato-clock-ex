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

I2CSlave i2c{PB_9, PB_8};

bool is_pause = false;
int32_t current_value = 0;
utils::system_clock::time_point time_previous{};
constexpr std::array<utils::system_clock::duration, 3> intervals = {16ms, 200ms,
                                                                    1000ms};
size_t current_interval_index = 0;

void pause()
{
    is_pause = !is_pause;
    if (is_pause)
        utils::console.printf("[-] Pause.\n");
    else
        utils::console.printf("[D] Pause.\n");
}
void reset()
{
    is_pause = false;
    utils::console.clear();
    utils::console.printf(R"(I2C Debugger
Button 1 - Pause
Button 2 - Reset
Button 3 - Rate
)");
}
void rate()
{
    current_interval_index++;
    if (current_interval_index >= intervals.size())
        current_interval_index = 0;
    utils::console.printf("[I] Rate: %d ms.\n",
                          intervals[current_interval_index].count());
}

int main()
{
    using namespace utils;

    // Initialize the console.
    reset();

    // Init I2C.
    i2c.address(0x10);

    // Init buttons.
    std::array<bool, 3> downs{};
    button_handler button1{PC_11};
    button_handler button2{PC_13};
    button_handler button3{PC_12};
    button1.set_callback([&]() { downs[0] = true; });
    button2.set_callback([&]() { downs[1] = true; });
    button3.set_callback([&]() { downs[2] = true; });

    // Set callbacks.
    std::array<std::function<void()>, 3> on_button{pause, reset, rate};

    while (true)
    {
        // Update buttons.
        {
            for (size_t i = 0; i < downs.size(); i++)
                if (downs[i])
                {
                    if (on_button[i])
                        on_button[i]();
                    downs[i] = false;
                }
        }

        // Check I2C.
        if (!is_pause)
        {
            int slave_action = i2c.receive();
            if (slave_action == I2CSlave::ReadAddressed)
            {
                // Do not respond if the interval is shorter than the threshold.
                if (system_clock::now() - time_previous >=
                    intervals[current_interval_index])
                {
                    current_value++;

                    utils::console.printf("[-] Write %d.\n", current_value);
                    int result;
                    result =
                        i2c.write(reinterpret_cast<const char*>(&current_value),
                                  sizeof(current_value));
                    if (!result)
                        utils::console.printf("[D] Write %d.\n", current_value);
                    else
                        utils::console.printf("[F] Write %d.\n", current_value);

                    time_previous = system_clock::now();
                }
            }
        }
    }
}
