/**
 * @file tft_debug_console.hpp
 * @author UnnamedOrange
 * @brief 将 TFT 屏幕作为调试控制台。
 * @note 该文件是为调试而写的临时文件，不对代码质量做保证。
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include <algorithm>
#include <array>
#include <cinttypes>
#include <deque>
#include <string>
#include <string_view>
#include <vector>

#include "tft_device.hpp"

namespace modules
{

    class tft_debug_console : public tft_device<false>
    {
    public:
        constexpr static int cx = 128;     // 横轴。
        constexpr static int cy = 160;     // 纵轴。
        constexpr static int cx_char = 6;  // 横轴。
        constexpr static int cy_char = 12; // 纵轴。
        constexpr static int n_char_per_line =
            cx / cx_char; // 每行最多显示的字符数。
        constexpr static int n_line = cy / cy_char; // 最多显示的行数。
        constexpr static int byte_length =
            ((cx_char + 7) / 8) * cy_char; // 单个字符长多少字节。
        constexpr static uint8_t font[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*" ",0*/
            0x00, 0x00, 0x04, 0x04, 0x04, 0x04,
            0x04, 0x00, 0x00, 0x04, 0x00, 0x00, /*"!",1*/
            0x14, 0x14, 0x0A, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*""",2*/
            0x00, 0x00, 0x0A, 0x0A, 0x1F, 0x0A,
            0x0A, 0x1F, 0x0A, 0x0A, 0x00, 0x00, /*"#",3*/
            0x00, 0x04, 0x0E, 0x15, 0x05, 0x06,
            0x0C, 0x14, 0x15, 0x0E, 0x04, 0x00, /*"$",4*/
            0x00, 0x00, 0x12, 0x15, 0x0D, 0x15,
            0x2E, 0x2C, 0x2A, 0x12, 0x00, 0x00, /*"%",5*/
            0x00, 0x00, 0x04, 0x0A, 0x0A, 0x36,
            0x15, 0x15, 0x29, 0x16, 0x00, 0x00, /*"&",6*/
            0x02, 0x02, 0x01, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"'",7*/
            0x10, 0x08, 0x08, 0x04, 0x04, 0x04,
            0x04, 0x04, 0x08, 0x08, 0x10, 0x00, /*"(",8*/
            0x02, 0x04, 0x04, 0x08, 0x08, 0x08,
            0x08, 0x08, 0x04, 0x04, 0x02, 0x00, /*")",9*/
            0x00, 0x00, 0x00, 0x04, 0x15, 0x0E,
            0x0E, 0x15, 0x04, 0x00, 0x00, 0x00, /*"*",10*/
            0x00, 0x00, 0x00, 0x08, 0x08, 0x3E,
            0x08, 0x08, 0x00, 0x00, 0x00, 0x00, /*"+",11*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x02, 0x02, 0x01, 0x00, /*",",12*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x3F,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"-",13*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x02, 0x00, 0x00, /*".",14*/
            0x00, 0x20, 0x10, 0x10, 0x08, 0x08,
            0x04, 0x04, 0x02, 0x02, 0x01, 0x00, /*"/",15*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x11,
            0x11, 0x11, 0x11, 0x0E, 0x00, 0x00, /*"0",16*/
            0x00, 0x00, 0x04, 0x06, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x0E, 0x00, 0x00, /*"1",17*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x08,
            0x04, 0x02, 0x01, 0x1F, 0x00, 0x00, /*"2",18*/
            0x00, 0x00, 0x0E, 0x11, 0x10, 0x0C,
            0x10, 0x10, 0x11, 0x0E, 0x00, 0x00, /*"3",19*/
            0x00, 0x00, 0x08, 0x0C, 0x0C, 0x0A,
            0x09, 0x1F, 0x08, 0x1C, 0x00, 0x00, /*"4",20*/
            0x00, 0x00, 0x1F, 0x01, 0x01, 0x0F,
            0x11, 0x10, 0x11, 0x0E, 0x00, 0x00, /*"5",21*/
            0x00, 0x00, 0x0C, 0x12, 0x01, 0x0D,
            0x13, 0x11, 0x11, 0x0E, 0x00, 0x00, /*"6",22*/
            0x00, 0x00, 0x1E, 0x10, 0x08, 0x08,
            0x04, 0x04, 0x04, 0x04, 0x00, 0x00, /*"7",23*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x0E,
            0x11, 0x11, 0x11, 0x0E, 0x00, 0x00, /*"8",24*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x19,
            0x16, 0x10, 0x09, 0x06, 0x00, 0x00, /*"9",25*/
            0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
            0x00, 0x00, 0x00, 0x04, 0x00, 0x00, /*":",26*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
            0x00, 0x00, 0x00, 0x04, 0x04, 0x00, /*";",27*/
            0x00, 0x00, 0x10, 0x08, 0x04, 0x02,
            0x02, 0x04, 0x08, 0x10, 0x00, 0x00, /*"<",28*/
            0x00, 0x00, 0x00, 0x00, 0x3F, 0x00,
            0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, /*"=",29*/
            0x00, 0x00, 0x02, 0x04, 0x08, 0x10,
            0x10, 0x08, 0x04, 0x02, 0x00, 0x00, /*">",30*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x08,
            0x04, 0x04, 0x00, 0x04, 0x00, 0x00, /*"?",31*/
            0x00, 0x00, 0x1C, 0x22, 0x29, 0x2D,
            0x2D, 0x1D, 0x22, 0x1C, 0x00, 0x00, /*"@",32*/
            0x00, 0x00, 0x04, 0x04, 0x0C, 0x0A,
            0x0A, 0x1E, 0x12, 0x33, 0x00, 0x00, /*"A",33*/
            0x00, 0x00, 0x0F, 0x12, 0x12, 0x0E,
            0x12, 0x12, 0x12, 0x0F, 0x00, 0x00, /*"B",34*/
            0x00, 0x00, 0x1E, 0x11, 0x01, 0x01,
            0x01, 0x01, 0x11, 0x0E, 0x00, 0x00, /*"C",35*/
            0x00, 0x00, 0x0F, 0x12, 0x12, 0x12,
            0x12, 0x12, 0x12, 0x0F, 0x00, 0x00, /*"D",36*/
            0x00, 0x00, 0x1F, 0x12, 0x0A, 0x0E,
            0x0A, 0x02, 0x12, 0x1F, 0x00, 0x00, /*"E",37*/
            0x00, 0x00, 0x1F, 0x12, 0x0A, 0x0E,
            0x0A, 0x02, 0x02, 0x07, 0x00, 0x00, /*"F",38*/
            0x00, 0x00, 0x1C, 0x12, 0x01, 0x01,
            0x39, 0x11, 0x12, 0x0C, 0x00, 0x00, /*"G",39*/
            0x00, 0x00, 0x33, 0x12, 0x12, 0x1E,
            0x12, 0x12, 0x12, 0x33, 0x00, 0x00, /*"H",40*/
            0x00, 0x00, 0x1F, 0x04, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x1F, 0x00, 0x00, /*"I",41*/
            0x00, 0x00, 0x3E, 0x08, 0x08, 0x08,
            0x08, 0x08, 0x08, 0x08, 0x09, 0x07, /*"J",42*/
            0x00, 0x00, 0x37, 0x12, 0x0A, 0x06,
            0x0A, 0x12, 0x12, 0x37, 0x00, 0x00, /*"K",43*/
            0x00, 0x00, 0x07, 0x02, 0x02, 0x02,
            0x02, 0x02, 0x22, 0x3F, 0x00, 0x00, /*"L",44*/
            0x00, 0x00, 0x3B, 0x1B, 0x1B, 0x1B,
            0x15, 0x15, 0x15, 0x35, 0x00, 0x00, /*"M",45*/
            0x00, 0x00, 0x3B, 0x12, 0x16, 0x16,
            0x1A, 0x1A, 0x12, 0x17, 0x00, 0x00, /*"N",46*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x11,
            0x11, 0x11, 0x11, 0x0E, 0x00, 0x00, /*"O",47*/
            0x00, 0x00, 0x0F, 0x12, 0x12, 0x0E,
            0x02, 0x02, 0x02, 0x07, 0x00, 0x00, /*"P",48*/
            0x00, 0x00, 0x0E, 0x11, 0x11, 0x11,
            0x11, 0x17, 0x19, 0x0E, 0x18, 0x00, /*"Q",49*/
            0x00, 0x00, 0x0F, 0x12, 0x12, 0x0E,
            0x0A, 0x12, 0x12, 0x37, 0x00, 0x00, /*"R",50*/
            0x00, 0x00, 0x1E, 0x11, 0x01, 0x06,
            0x08, 0x10, 0x11, 0x0F, 0x00, 0x00, /*"S",51*/
            0x00, 0x00, 0x1F, 0x15, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x0E, 0x00, 0x00, /*"T",52*/
            0x00, 0x00, 0x33, 0x12, 0x12, 0x12,
            0x12, 0x12, 0x12, 0x0C, 0x00, 0x00, /*"U",53*/
            0x00, 0x00, 0x33, 0x12, 0x12, 0x0A,
            0x0A, 0x0C, 0x04, 0x04, 0x00, 0x00, /*"V",54*/
            0x00, 0x00, 0x15, 0x15, 0x15, 0x15,
            0x0E, 0x0A, 0x0A, 0x0A, 0x00, 0x00, /*"W",55*/
            0x00, 0x00, 0x1B, 0x0A, 0x0A, 0x04,
            0x04, 0x0A, 0x0A, 0x1B, 0x00, 0x00, /*"X",56*/
            0x00, 0x00, 0x1B, 0x0A, 0x0A, 0x0A,
            0x04, 0x04, 0x04, 0x0E, 0x00, 0x00, /*"Y",57*/
            0x00, 0x00, 0x1F, 0x09, 0x08, 0x04,
            0x04, 0x02, 0x12, 0x1F, 0x00, 0x00, /*"Z",58*/
            0x1C, 0x04, 0x04, 0x04, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x04, 0x1C, 0x00, /*"[",59*/
            0x00, 0x02, 0x02, 0x04, 0x04, 0x04,
            0x08, 0x08, 0x08, 0x10, 0x10, 0x00, /*"\",60*/
            0x0E, 0x08, 0x08, 0x08, 0x08, 0x08,
            0x08, 0x08, 0x08, 0x08, 0x0E, 0x00, /*"]",61*/
            0x04, 0x0A, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"^",62*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, /*"_",63*/
            0x02, 0x04, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"`",64*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
            0x12, 0x1C, 0x12, 0x3C, 0x00, 0x00, /*"a",65*/
            0x00, 0x03, 0x02, 0x02, 0x02, 0x0E,
            0x12, 0x12, 0x12, 0x0E, 0x00, 0x00, /*"b",66*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
            0x12, 0x02, 0x12, 0x0C, 0x00, 0x00, /*"c",67*/
            0x00, 0x18, 0x10, 0x10, 0x10, 0x1C,
            0x12, 0x12, 0x12, 0x3C, 0x00, 0x00, /*"d",68*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
            0x12, 0x1E, 0x02, 0x1C, 0x00, 0x00, /*"e",69*/
            0x00, 0x18, 0x24, 0x04, 0x04, 0x1E,
            0x04, 0x04, 0x04, 0x1E, 0x00, 0x00, /*"f",70*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x3C,
            0x12, 0x0C, 0x02, 0x1C, 0x22, 0x1C, /*"g",71*/
            0x00, 0x03, 0x02, 0x02, 0x02, 0x0E,
            0x12, 0x12, 0x12, 0x37, 0x00, 0x00, /*"h",72*/
            0x00, 0x04, 0x04, 0x00, 0x00, 0x06,
            0x04, 0x04, 0x04, 0x0E, 0x00, 0x00, /*"i",73*/
            0x00, 0x08, 0x08, 0x00, 0x00, 0x0C,
            0x08, 0x08, 0x08, 0x08, 0x08, 0x07, /*"j",74*/
            0x00, 0x03, 0x02, 0x02, 0x02, 0x1A,
            0x0A, 0x06, 0x0A, 0x13, 0x00, 0x00, /*"k",75*/
            0x00, 0x07, 0x04, 0x04, 0x04, 0x04,
            0x04, 0x04, 0x04, 0x1F, 0x00, 0x00, /*"l",76*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
            0x15, 0x15, 0x15, 0x15, 0x00, 0x00, /*"m",77*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
            0x12, 0x12, 0x12, 0x37, 0x00, 0x00, /*"n",78*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
            0x12, 0x12, 0x12, 0x0C, 0x00, 0x00, /*"o",79*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
            0x12, 0x12, 0x12, 0x0E, 0x02, 0x07, /*"p",80*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
            0x12, 0x12, 0x12, 0x1C, 0x10, 0x38, /*"q",81*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
            0x06, 0x02, 0x02, 0x07, 0x00, 0x00, /*"r",82*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1E,
            0x02, 0x0C, 0x10, 0x1E, 0x00, 0x00, /*"s",83*/
            0x00, 0x00, 0x00, 0x04, 0x04, 0x1E,
            0x04, 0x04, 0x04, 0x1C, 0x00, 0x00, /*"t",84*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
            0x12, 0x12, 0x12, 0x3C, 0x00, 0x00, /*"u",85*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
            0x0A, 0x0A, 0x04, 0x04, 0x00, 0x00, /*"v",86*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x15,
            0x15, 0x0E, 0x0A, 0x0A, 0x00, 0x00, /*"w",87*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1B,
            0x0A, 0x04, 0x0A, 0x1B, 0x00, 0x00, /*"x",88*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x33,
            0x12, 0x12, 0x0C, 0x08, 0x04, 0x03, /*"y",89*/
            0x00, 0x00, 0x00, 0x00, 0x00, 0x1E,
            0x08, 0x04, 0x04, 0x1E, 0x00, 0x00, /*"z",90*/
            0x18, 0x08, 0x08, 0x08, 0x08, 0x0C,
            0x08, 0x08, 0x08, 0x08, 0x18, 0x00, /*"{",91*/
            0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
            0x08, 0x08, 0x08, 0x08, 0x08, 0x08, /*"|",92*/
            0x06, 0x04, 0x04, 0x04, 0x04, 0x08,
            0x04, 0x04, 0x04, 0x04, 0x06, 0x00, /*"}",93*/
            0x16, 0x09, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*"~",94*/
        };

    public:
        tft_debug_console()
        {
            reset();
            set_mode(tft_rs_t::index);
            write(0x11); // Sleep exit
            rtos::ThisThread::sleep_for(255ms);

            set_mode(tft_rs_t::index);
            write(0xB1);
            set_mode(tft_rs_t::data);
            write({0x00, 0x2C, 0x2D});

            set_mode(tft_rs_t::index);
            write(0xB2);
            set_mode(tft_rs_t::data);
            write({0x00, 0x2C, 0x2D});

            set_mode(tft_rs_t::index);
            write(0xB3);
            set_mode(tft_rs_t::data);
            write({0x00, 0x2C, 0x2D});

            set_mode(tft_rs_t::index);
            write(0xB4);
            set_mode(tft_rs_t::data);
            write(0x07);

            set_mode(tft_rs_t::index);
            write(0xC0);
            set_mode(tft_rs_t::data);
            write({0xA2, 0x02, 0x84});

            set_mode(tft_rs_t::index);
            write(0xC1);
            set_mode(tft_rs_t::data);
            write(0xC5);

            set_mode(tft_rs_t::index);
            write(0xC2);
            set_mode(tft_rs_t::data);
            write({0x0A, 0x00});

            set_mode(tft_rs_t::index);
            write(0xC3);
            set_mode(tft_rs_t::data);
            write({0x8A, 0x2A});

            set_mode(tft_rs_t::index);
            write(0xC4);
            set_mode(tft_rs_t::data);
            write({0x8A, 0xEE});

            set_mode(tft_rs_t::index);
            write(0xC5);
            set_mode(tft_rs_t::data);
            write(0x0E);

            set_mode(tft_rs_t::index);
            write(0x36);
            set_mode(tft_rs_t::data);
            write(0x00);

            set_mode(tft_rs_t::index);
            write(0xE0);
            set_mode(tft_rs_t::data);
            write({0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B,
                   0x23, 0x37, 0x00, 0x07, 0x03, 0x10});

            set_mode(tft_rs_t::index);
            write(0xE1);
            set_mode(tft_rs_t::data);
            write({0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e, 0x30, 0x30,
                   0x39, 0x3f, 0x00, 0x07, 0x03, 0x10});

            set_mode(tft_rs_t::index);
            write(0x2A);
            set_mode(tft_rs_t::data);
            write({0x00, 0x00, 0x00, 0x7F});

            set_mode(tft_rs_t::index);
            write(0x2B);
            set_mode(tft_rs_t::data);
            write({0x00, 0x00, 0x00, 0x9F});

            set_mode(tft_rs_t::index);
            write(0xF0);
            set_mode(tft_rs_t::data);
            write(0x01);

            set_mode(tft_rs_t::index);
            write(0xF6);
            set_mode(tft_rs_t::data);
            write(0x00);

            set_mode(tft_rs_t::index);
            write(0x3A);
            set_mode(tft_rs_t::data);
            write(0x05);

            set_mode(tft_rs_t::index);
            write(0x29);

            blt();
            rtos::ThisThread::sleep_for(50ms);
            set_led(true);

            thread_draw.start(std::bind(&tft_debug_console::draw_task, this));
        }

    private:
        std::array<std::array<uint16_t, cx>, cy> vram{}; // 按行存储。

    private:
        void set_region(uint8_t x_start, uint8_t y_start, uint8_t x_end,
                        uint8_t y_end)
        {
            set_mode(tft_rs_t::index);
            write(0x2A);
            set_mode(tft_rs_t::data);
            write({
                0x00,
                x_start,
                0x00,
                x_end,
            });
            set_mode(tft_rs_t::index);
            write(0x2B);
            set_mode(tft_rs_t::data);
            write({
                0x00,
                y_start,
                0x00,
                y_end,
            });
            set_mode(tft_rs_t::index);
            write(0x2c);
        }
        void blt()
        {
            set_region(0, 0, cx - 1, cy - 1);
            set_mode(tft_rs_t::index);
            write(0x2C);
            set_mode(tft_rs_t::data);
            write(vram.data(), cx * cy * sizeof(uint16_t));
        }

    private:
        // 在缓冲区上直接画字符。
        void draw_char_vram(char ch, int x, int y, uint16_t text_color,
                            uint16_t bg_color)
        {
            if (ch < 0x20 || ch > 0x7f)
                return; // 不支持的字符。

            text_color = (text_color << 8) + (text_color >> 8);
            bg_color = (bg_color << 8) + (bg_color >> 8);

            uint8_t idx_char = ch - 0x20;
            for (int y_char = 0; y_char < cy_char; y_char++)
            {
                if (y + y_char < 0)
                    continue;
                if (y + y_char >= cy)
                    break;
                uint8_t bits = font[idx_char * byte_length + y_char];
                for (int x_char = 0, bit_idx = 0; x_char < cx_char;
                     x_char++, bit_idx = bit_idx == 7 ? 0 : bit_idx + 1)
                {
                    if (x + x_char >= cx)
                        break;

                    if (bits & (1 << bit_idx))
                        vram[y + y_char][x + x_char] = text_color;
                    else if (bg_color != text_color)
                        vram[y + y_char][x + x_char] = bg_color;
                }
            }
        }
        // 在缓冲区上直接画字符串，不会自动换行。
        void draw_string_vram(std::string_view str, int x, int y,
                              uint16_t text_color, uint16_t bg_color)
        {
            if (y + cy_char < 0 || y >= cy)
                return;
            for (char ch : str)
            {
                if (x >= cx)
                    break;
                draw_char_vram(ch, x, y, text_color, bg_color);
                x += cx_char;
            }
        }

    private:
        rtos::Thread thread_draw;
        rtos::Mutex mutex_draw;
        rtos::ConditionVariable cv_draw{mutex_draw};
        void draw_task()
        {
            int cursor_counter = 0;
            bool draw_cursor = true;
            while (true)
            {
                rtos::ScopedMutexLock lock{mutex_draw};
                if (cv_draw.wait_for(
                        125ms, [this]() { return console.has_updated(); }))
                {
                    console.clear_update_tag();
                    cursor_counter = 7;
                    draw_cursor = true;
                }
                else
                {
                    console.update_status();
                    cursor_counter += 1;
                    if (cursor_counter >= 8)
                    {
                        draw_cursor = !draw_cursor;
                        cursor_counter = 4;
                    }
                }
                draw_console(draw_cursor);
            }
        }

    private:
        struct console_buffer
        {
            static constexpr size_t max_n_line = n_line * 2;
            std::deque<std::string> buffer;
            bool updated{};
            console_buffer() { clear(); }
            void clear()
            {
                buffer.clear();
                buffer.push_back("");
            }
            void print(std::string_view str, bool recursive_print = false)
            {
                for (char ch : str)
                {
                    if (ch == '\r')
                        continue;
                    else if (ch == '\n')
                        buffer.push_back("");
                    else
                        buffer.back().push_back(ch);
                }
                if (recursive_print)
                    return;
                for (auto it = buffer.begin(); it != buffer.end(); it++)
                {
                    if ((*it).substr(0, 3) == "[D]" ||
                        (*it).substr(0, 3) == "[F]")
                    {
                        bool flag = false;
                        for (auto& line_to_modify : buffer)
                        {
                            if (line_to_modify.substr(0, 3) == "[-]" ||
                                line_to_modify.substr(0, 3) == "[\\]" ||
                                line_to_modify.substr(0, 3) == "[|]" ||
                                line_to_modify.substr(0, 3) == "[/]")
                            {
                                if ((*it).substr(3) != line_to_modify.substr(3))
                                    continue;

                                if ((*it).substr(0, 3) == "[D]")
                                    line_to_modify[1] = '*';
                                else // Fail
                                    line_to_modify[1] = 'x';
                                flag = true;
                                break;
                            }
                        }
                        if (flag)
                            it = buffer.erase(it);
                        else
                        {
                            // print("[E] No matched task.\n", true);
                        }
                    }
                }
                while (buffer.size() > max_n_line)
                    buffer.pop_front();
                updated = true;
            }
            bool has_updated() const { return updated; }
            void clear_update_tag() { updated = false; }
            void update_status()
            {
                for (auto& line : buffer)
                {
                    if (line.substr(0, 3) == "[-]")
                        line[1] = '\\';
                    else if (line.substr(0, 3) == "[\\]")
                        line[1] = '|';
                    else if (line.substr(0, 3) == "[|]")
                        line[1] = '/';
                    else if (line.substr(0, 3) == "[/]")
                        line[1] = '-';
                }
            }
        } console;
        void draw_console(bool draw_cursor)
        {
            for (auto& line : vram)
                line.fill(0);
            int y =
                -cy_char *
                std::max(0, static_cast<int>(console.buffer.size()) - n_line);
            for (const auto& line : console.buffer)
            {
                uint16_t color = 0xFFFF;
                if (line.substr(0, 3) == "[I]")
                    color =
                        (0 >> 3) + (255 >> 2 << 5) + (0 >> 3 << 11); // B G R
                else if (line.substr(0, 3) == "[W]")
                    color =
                        (0 >> 3) + (255 >> 2 << 5) + (255 >> 3 << 11); // B G R
                else if (line.substr(0, 3) == "[E]" ||
                         line.substr(0, 3) == "[x]")
                    color =
                        (0 >> 3) + (0 >> 2 << 5) + (255 >> 3 << 11); // B G R
                else if (line.substr(0, 3) == "[-]" ||
                         line.substr(0, 3) == "[\\]" ||
                         line.substr(0, 3) == "[|]" ||
                         line.substr(0, 3) == "[/]")
                    color =
                        (0 >> 3) + (114 >> 2 << 5) + (230 >> 3 << 11); // B G R
                else if (line.substr(0, 3) == "[*]")
                    color =
                        (255 >> 3) + (255 >> 2 << 5) + (0 >> 3 << 11); // B G R
                draw_string_vram(line, 0, y, color, 0x0000);
                y += cy_char;
            }
            y -= cy_char;
            if (draw_cursor)
            {
                draw_char_vram('_', console.buffer.back().size() * cx_char, y,
                               0xFFFF, 0x0000);
            }
            blt();
        }

    public:
        template <typename... R>
        void printf(const char* format, R&&... args)
        {
            char buf[256];
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
            sprintf(buf, format, std::forward<R>(args)...);
#pragma GCC diagnostic pop
            rtos::ScopedMutexLock lock(mutex_draw);
            console.print(buf);
            cv_draw.notify_one();
        }

        void clear()
        {
            rtos::ScopedMutexLock lock(mutex_draw);
            console.clear();
            cv_draw.notify_one();
        }
    };
} // namespace modules
