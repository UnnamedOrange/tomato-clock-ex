// Copyright (c) UnnamedOrange. Licensed under the MIT License.
// See the LICENSE file in the repository root for full license text.

#include "mbed.h"

#include <array>
#include <chrono>
#include <functional>
#include <queue>
#include <type_traits>
#include <vector>

#include "esp8266/esp8266.hpp"
#include "tft/GUI.h"
#include "tft/Lcd_Driver.h"
#include "tft/Picture.hpp"

using namespace std::chrono;

/**
 * @brief Use HAL_GetTick() to implement now().
 */
class HAL_Clock : public Kernel::Clock
{
public:
    static time_point now()
    {
        return time_point(duration(HAL_GetTick()));
    }
};
using sys_clock = HAL_Clock;

/**
 * @brief Cache of the current tick count.
 * @deprecated Now the implementation has been changed.
 *
 * @remarks It is not allowed to use Kernel::Clock::now() in ISR, so this is an
 * alternative.
 * @see
 * https://os.mbed.com/docs/mbed-os/v6.15/mbed-os-api-doxy/namespacertos_1_1_kernel.html#a19d37a6ee49d4e132e82c3c4f3d0ca90
 */
// sys_clock::time_point g_now = sys_clock::now();

/**
 * @brief Do not cache the current tick count because the implementation is
 * updated.
 */
#define g_now() (sys_clock::now())

/**
 * @brief Play sounds via buzzer.
 */
class audio_player
{
public:
    struct beat
    {
        uint16_t t;
        sys_clock::duration duration;
    };
    static constexpr sys_clock::duration _default_duration = 400ms;

private:
    std::queue<beat> beats;
    /**
     * @brief The start time of the current beat.
     * 0 means there is no beat.
     */
    sys_clock::time_point start_time{};

    DigitalOut& en;
    PwmOut& pwm;

public:
    audio_player(DigitalOut& en, PwmOut& out) : en(en), pwm(out)
    {
    }

private:
    void _play_current() const
    {
        pwm.period_us(beats.front().t);
        pwm.pulsewidth_us(beats.front().t / 2);
    }
    void _stop() const
    {
        pwm.pulsewidth_us(0);
    }

public:
    /**
     * @brief Call this in update().
     */
    void on_update()
    {
        if (beats.empty())
        {
            // Assert.
            // start_time = 0;
        }
        else
        {
            // No beat is playing.
            if (!start_time.time_since_epoch().count())
            {
                start_time = g_now();
                _play_current();
            }
            else // Check whether to transfer states.
            {
                auto duration = g_now() - start_time;
                if (duration >= beats.front().duration)
                {
                    beats.pop();
                    if (beats.empty()) // No beat here, stop.
                    {
                        start_time = sys_clock::time_point{};
                        _stop();
                    }
                    else // Play the next beat.
                    {
                        start_time = g_now();
                        _play_current();
                    }
                }
            }
        }
    }

public:
    /**
     * @brief Play the beats now.
     *
     * @param beats An array of uint16_t.
     * @details There is a queue.
     */
    template <typename iterable_t>
    void push_beats(const iterable_t& beats)
    {
        for (const auto& beat : beats)
            this->beats.push({beat, _default_duration});
    }
    void set_mute(bool is_mute)
    {
        // Note en is ~EN.
        en = is_mute;
    }
};

/**
 * @brief Eliminate jitters and manage the callback function of a button.
 *
 * @remarks Now this class based on IRQs. But this feature is not ruled.
 */
class button_handler : protected InterruptIn
{
private:
    std::function<void()> m_callback;
    sys_clock::time_point m_last_pressed{};
    void on_fall()
    {
        // Respond only if the interval is longer than the threshold.
        if (g_now() - m_last_pressed >= 50ms)
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

/**
 * @brief Define the songs.
 *
 * @remarks It is not possible to define arrays in Main class.
 */
constexpr std::array<uint16_t, 4> _song_1{
    2273, 2551, 3401, 3030 // 6 5 2 3
};
constexpr std::array<uint16_t, 4> _song_2{
    3401, 3030, 2273, 2551 // 2 3 6 5
};

class Main
{
    /**
     * @brief Define the LED pins.
     */
private:
    static constexpr auto LEDR = PC_10;
    static constexpr auto LEDG = PA_5;
    static constexpr auto LEDB = PD_2;

    /**
     * @brief Define the songs.
     */
private:
    /**
     * @brief Define the durations.
     */
private:
    static constexpr auto _max_duration = 60min;
    static constexpr auto _default_duration_work = 25min;
    static constexpr auto _default_duration_rest = 5min;
    static constexpr auto _default_step = 5min;

    /**
     * @brief Define the Mbed pin objects.
     */
private:
    DigitalOut m_led_r{LEDR};
    DigitalOut m_led_b{LEDB};
    DigitalOut m_led_g{LEDG};
    button_handler m_button_left{PC_12};
    button_handler m_button_middle{PC_13};
    button_handler m_button_right{PC_11};
    DigitalOut m_buzzer_en{PC_3}; // ~EN of the buzzer.
    PwmOut m_buzzer_pwm{PA_15};

    esp8266 m_esp8266; // Pins are defined in the header.

    /**
     * @brief Define the states of the program.
     */
private:
    uint8_t m_num_tomatoes{}; // The number of tomatoes.
    enum class state_t
    {
        work,
        rest,
    } m_state{};
    sys_clock::time_point
        m_current_start_time{}; // Start time of the current period.
    sys_clock::duration m_current_duration{}; // Duration of the current period.

    /**
     * @brief Define auxiliary states.
     */
private:
    enum class state_drawing_t
    {
        idle = 0,
        red_tomato,
        green_tomato,
    } m_state_drawing{};

    /**
     * @brief audio.
     */
private:
    audio_player m_audio{m_buzzer_en, m_buzzer_pwm};
    void play_audio(u8 index)
    {
        switch (index)
        {
        case 0:
            m_audio.push_beats(_song_1);
            break;
        case 1:
            m_audio.push_beats(_song_2);
            break;
        }
    }

    /**
     * @brief GUI and main flows.
     */
private:
    void gui_show_welcome()
    {
        Lcd_Clear(WHITE);
        show_pic(gImage_PKU, 0, 12, 128, 145);
    }
    void gui_draw_background()
    {
        Lcd_Clear(WHITE);
        show_pic(gImage_bg, 0, 0, 128, 90);
    }
    void gui_show_message(state_t index)
    {
        if (index == state_t::work)
            Gui_DrawFont_GBK16(36, 62, BLUE, WHITE,
                               reinterpret_cast<const u8*>("Working"));
        else
            Gui_DrawFont_GBK16(36, 62, BLUE, WHITE,
                               reinterpret_cast<const u8*>("Resting"));
    }
    u8 TX(u8 n)
    {
        if (n < 5)
            return 32 * (n - 1);
        else
            return 32 * (n - 5);
    }
    u8 TY(u8 n)
    {
        if (n < 5)
            return 90;
        else
            return 125;
    }
    int8_t caches[4] = {-1, -1, -1, -1};
    void gui_show_time(sys_clock::duration remaining)
    {
        int8_t min = duration_cast<seconds>(remaining).count() / 60;
        int8_t sec = duration_cast<seconds>(remaining).count() % 60;
        int8_t digits[4];
        digits[3] = min / 10;
        digits[2] = min % 10;
        digits[1] = sec / 10;
        digits[0] = sec % 10;
        for (int i = 3; ~i; i--)
        {
            if (digits[i] != caches[i])
            {
                constexpr int x[]{93, 69, 32, 8};
                Gui_DrawFont_Num32(x[i], 15, GRAY2, WHITE, digits[i]);
                caches[i] = digits[i];
            }
        }
    }

    void transfer_state()
    {
        switch (m_state)
        {
        case state_t::work:
        {
            m_state = state_t::rest;
            if (m_num_tomatoes == 8)
            {
                m_num_tomatoes = 0;
                Lcd_ClearHalf(WHITE);
            }
            m_num_tomatoes++;
            if (g_now() >= m_current_start_time + m_current_duration)
                m_state_drawing = state_drawing_t::red_tomato;
            else
                m_state_drawing = state_drawing_t::green_tomato;

            gui_show_message(m_state);
            m_led_g = 1;
            m_led_b = 0;
            play_audio(0);
            break;
        }
        case state_t::rest:
        {
            m_state = state_t::work;
            gui_show_message(m_state);
            m_led_g = 0;
            m_led_b = 1;
            play_audio(1);
            break;
        }
        }
        m_current_start_time = g_now();
        m_current_duration = m_state == state_t::work ? _default_duration_work
                                                      : _default_duration_rest;
    }
    auto _get_remaining_time() const
    {
        return m_current_start_time + m_current_duration - g_now();
    }
    void on_time_up()
    {
        auto remaining = _get_remaining_time();
        if (remaining + _default_step < _max_duration)
            m_current_duration += _default_step;
    }
    void on_time_down()
    {
        auto remaining = _get_remaining_time();
        if (remaining - _default_step > 0s)
            m_current_duration -= _default_step;
    }
    void update()
    {
        auto remaining = _get_remaining_time();
        int8_t min = duration_cast<seconds>(remaining).count() / 60;
        int8_t sec = duration_cast<seconds>(remaining).count() % 60;
        if (remaining <= 0s)
            transfer_state();

        m_audio.on_update();
    }
    void draw()
    {
        auto remaining = _get_remaining_time();
        gui_show_time(remaining);
        if (m_state_drawing != state_drawing_t::idle)
        {
            show_pic(m_state_drawing == state_drawing_t::red_tomato
                         ? Red_tomato
                         : Green_tomato,
                     TX(m_num_tomatoes), TY(m_num_tomatoes),
                     TX(m_num_tomatoes) + 30, TY(m_num_tomatoes) + 32);
            m_state_drawing = state_drawing_t::idle;
        }
    }

private:
    bool setup_wifi()
    {
        auto wrapper = [&](const std::string& reply) {
            printf("%s\n", reply.c_str());
            return reply.find("OK") != std::string::npos;
        };

        string_view ssid = "87654321";
        string_view password = "87654321";
        if (!wrapper(m_esp8266.reset()))
            return false;
        wrapper(m_esp8266.get_version());
        wrapper(m_esp8266.set_mode(3));
        wrapper(m_esp8266.set_mux_mode(0));
        wrapper(m_esp8266.list_ap_raw());
        wrapper(m_esp8266.join_ap(ssid, password));
        wrapper(m_esp8266.get_ip());
        wrapper(m_esp8266.get_connection_status());
        return true;
    }

public:
    Main()
    {
        // Light up RGB LEDs.
        m_led_g = 0;
        m_led_r = 0;
        m_led_b = 0;

        // Initialize the LCD and light it up on startup.
        Lcd_Init(0);
        Lcd_On();

        // Show the welcome screen.
        gui_show_welcome();

        // Setup WiFi.
        if (!setup_wifi())
            rtos::ThisThread::sleep_for(1s);

        // Draw the background.
        gui_draw_background();

        // Initialize the states.
        m_state = state_t::work;
        m_current_start_time = g_now();
        m_current_duration = _default_duration_work;
        m_num_tomatoes = 0;

        // Show the main screen and activate the peripherals.
        gui_show_message(m_state);
        m_led_g = 0;
        m_led_r = 1;
        m_led_b = 1;
        m_audio.set_mute(false); // Enable the buzzer.

        // Setup the interrupts.
        m_button_middle.set_callback(std::bind(&Main::transfer_state, this));
        m_button_left.set_callback(std::bind(&Main::on_time_up, this));
        m_button_right.set_callback(std::bind(&Main::on_time_down, this));

        // Update the screen.
        while (true)
        {
            update();
            draw();
            rtos::ThisThread::sleep_for(100ms);
        }
    }
};

/**
 * @brief Run an instance of the main application.
 *
 * @tparam T The application class.
 * @param args The arguments to pass to the application.
 */
template <typename T, typename... R>
void run_instance(R&&... args)
{
    // A minimal OS (bare-metal) is used.
    delete new T(std::forward<R>(args)...);
    // Returning in main is not allowed.
    // https://os.mbed.com/docs/mbed-os/v6.15/bare-metal/using-small-c-libraries.html
    while (true)
        sleep();
}

int main()
{
    run_instance<Main>();
}
