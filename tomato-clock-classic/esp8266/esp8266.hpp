/**
 * @file esp8266.hpp
 * @author UnnamedOrange
 * @brief Use ESP8266 with bare-metal profile.
 * @version 0.0.0
 * @date 2022-04-08
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <string>
#include <chrono>
#include <string_view>

using namespace std::literals;

/**
 * @brief Define the pins of ESP8266.
 * @remark You can redefine the pins before including this header.
 */
#ifndef ESP8266_TX
#define ESP8266_TX PC_7
#endif // ESP8266_TX
#ifndef ESP8266_RX
#define ESP8266_RX PC_6
#endif // ESP8266_RX
#ifndef ESP8266_EN
#define ESP8266_EN PC_8
#endif // ESP8266_EN
#ifndef ESP8266_RST
#define ESP8266_RST PC_9
#endif // ESP8266_RST

class esp8266
{
public:
    using pin_name_t = PinName;
    static constexpr pin_name_t _pin_tx{ESP8266_RX};   // Exchange the tx and rx.
    static constexpr pin_name_t _pin_rx{ESP8266_TX};   // Exchange the tx and rx.
    static constexpr pin_name_t _pin_en{ESP8266_EN};   // 1 means enabled.
    static constexpr pin_name_t _pin_rst{ESP8266_RST}; // 1 means do not reset.

private:
    static constexpr auto _default_reply_timeout{1000ms};

private:
    BufferedSerial esp{_pin_tx, _pin_rx, 115200}; // The default buadrate of ESP8266 is 115200.
    DigitalOut en{_pin_en};                       // 1 means enabled.
    DigitalOut rst{_pin_rst};                     // 1 means do not reset.

public:
    esp8266()
    {
        // Initialize EN and RST on startup.
        en = 1;  // 1 means enabled.
        rst = 1; // 1 means do not reset.
    }

private:
    /**
     * @brief Write AT command.
     *
     * @param command Complete AT command.
     * @return The number of bytes written.
     * @see BufferedSerial::write
     */
    auto _write_command(const std::string_view command)
    {
        return esp.write(command.data(), command.size());
    }
    /**
     * @brief Get reply from ESP8266. It waits for a specific time and reads all the buffer.
     *
     * @param timeout The time to wait for the reply. You should set it to an estimated value.
     * @return std::string The reply in string.
     */
    std::string _get_reply(std::chrono::milliseconds timeout = _default_reply_timeout)
    {
        std::string ret;
        ThisThread::sleep_for(timeout); // Sleep for a specific time.
        while (esp.readable())          // And then get all the buffer.
        {
            char ch;
            esp.read(&ch, 1);
            ret.push_back(ch);
        }
        return ret;
    }

public:
    /**
     * @brief Write AT command and get reply.
     *
     * @param command Complete AT command.
     * @param timeout The time to wait for the reply. You should set it to an estimated value. The default value is 1000ms.
     * @return std::string The reply in string.
     */
    std::string write_and_get_reply(const std::string_view command, std::chrono::milliseconds timeout = _default_reply_timeout)
    {
        _write_command(command);
        return _get_reply(timeout);
    }

public:
    /**
     * @brief Send "AT+RST\\r\\n" to reset ESP8266.
     *
     * @return std::string The reply.
     */
    std::string reset()
    {
        return write_and_get_reply("AT+RST\r\n", 100ms);
    }
    /**
     * @brief Send "AT+GMR\\r\\n" to get the version of ESP8266.
     *
     * @return std::string The reply.
     */
    std::string get_version()
    {
        return write_and_get_reply("AT+GMR\r\n", 100ms);
    }
    /**
     * @brief Send "AT+CWMODE=x\\r\\n" to set the mode of ESP8266.
     *
     * @param mode 1 means station mode. 2 means softAP mode. 3 means station+softAP mode.
     * @return std::string The reply.
     */
    std::string set_mode(int mode)
    {
        return write_and_get_reply("AT+CWMODE="s + std::to_string(mode) + "\r\n", 100ms);
    }
    /**
     * @brief Send "AT+CIPMUX=x\\r\\n" to set the multiplexing mode of ESP8266.
     *
     * @param mode 0 means single connection. 1 means multiple connections.
     * @return std::string The reply.
     */
    std::string set_mux_mode(int mode)
    {
        return write_and_get_reply("AT+CIPMUX="s + std::to_string(mode) + "\r\n", 100ms);
    }
    /**
     * @brief Send "AT+CWLAP\\r\\n" to list the access points.
     *
     * @return std::string The reply.
     */
    std::string list_ap_raw()
    {
        return write_and_get_reply("AT+CWLAP\r\n", 1000ms);
    }
    /**
     * @brief Send "AT+CWJAP=\"SSID\",\"PASSWORD\"\\r\\n" to join an access point.
     *
     * @param ssid The SSID of the access point.
     * @param password The password of the access point.
     * @return std::string The reply.
     */
    std::string join_ap(std::string_view ssid, std::string_view password)
    {
        return write_and_get_reply("AT+CWJAP=\""s + ssid.data() + "\",\"" + password.data() + "\"\r\n", 5000ms);
    }
    /**
     * @brief Send "AT+CIFSR\\r\\n" to get the IP address.
     *
     * @return std::string The reply.
     */
    std::string get_ip()
    {
        return write_and_get_reply("AT+CIFSR\r\n", 100ms);
    }
    /**
     * @brief Send "AT+CIPSTATUS\\r\\n" to get the connection status.
     *
     * @return std::string The reply.
     */
    std::string get_connection_status()
    {
        return write_and_get_reply("AT+CIPSTATUS\r\n", 100ms);
    }
};
