/**
 * @file eeprom.hpp
 * @author UnnamedOrange
 * @brief Use internal EEPROM to store data.
 * @see
 * https://arduino.stackexchange.com/questions/83976/arduino-nano-33-ble-use-of-flash-to-store-data-how-to-expand-a-piece-of-sampl
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "mbed.h"

#include "FlashIAPBlockDevice.h"

#include <array>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "eeprom_default_config.hpp"

namespace modules
{
    /**
     * @brief Access the internal EEPROM with FlashIAPBlockDevice by mbed.
     * This class is in @b RAII style.
     */
    struct eeprom_device final : protected FlashIAPBlockDevice
    {
    private:
        using block_device_t = FlashIAPBlockDevice;

    public:
        /**
         * @brief Please define INTERNAL_FLASH_BASE_ADDRESS and
         * INTERNAL_FLASH_SIZE before including this header.
         */
        eeprom_device()
            : block_device_t{
                  reinterpret_cast<uint32_t>(INTERNAL_FLASH_BASE_ADDRESS),
                  static_cast<uint32_t>(INTERNAL_FLASH_SIZE)}
        {
            block_device_t::init();
        }
        ~eeprom_device()
        {
            block_device_t::deinit();
        }

    public:
        using block_device_t::erase;
        using block_device_t::get_erase_size;
        using block_device_t::get_program_size;
        using block_device_t::get_read_size;
        using block_device_t::program;
    };

    /**
     * @brief Data stored in EEPROM. You are supposed to define ONLY one object
     * and avoid writing to EEPROM too frequently. Use @b constexpr to define
     * the object and define it as a @b global or a @b static variable.
     *
     * @tparam T Type of data.
     *
     * @tparam read_block_size Read block size of the flash (EEPROM).
     * Please define @b EEPROM_READ_BLOCK_SIZE before including this header.
     *
     * @tparam program_block_size Program block size of the flash (EEPROM).
     * Please define @b EEPROM_PROGRAM_BLOCK_SIZE before including this header.
     *
     * @tparam erase_block_size Erase block size of the flash (EEPROM).
     * Please define @b EEPROM_ERASE_BLOCK_SIZE before including this header.
     *
     * @tparam padding_size Data is padded with some bytes to check whether
     * the data is valid. padding_size is the number of bytes to use.
     */
    template <typename T,
              size_t read_block_size = INTERNAL_FLASH_READ_BLOCK_SIZE,
              size_t program_block_size = INTERNAL_FLASH_PROGRAM_BLOCK_SIZE,
              size_t erase_block_size = INTERNAL_FLASH_ERASE_BLOCK_SIZE,
              size_t padding_size = 8>
    class eeprom
    {
    private:
        /**
         * @brief Padded data type.
         */
        struct data_t
        {
            uint8_t prefix[padding_size];
            T value;
            uint8_t suffix[padding_size];
            constexpr data_t() : prefix{}, value{}, suffix{}
            {
            }
            constexpr data_t(const T& data) : value{data}
            {
                for (size_t i = 0; i < padding_size; ++i)
                    prefix[i] = i;
                for (size_t i = 0; i < padding_size; ++i)
                    suffix[i] = i;
            }
        };
        /**
         * @brief Padded data type aligned to program_block_size.
         * @remark program_block_size may be smaller than 4, so a max is needed.
         */
        struct alignas(std::max(4u, program_block_size)) program_block_data_t
            : public data_t
        {
            using data_t::data_t;
        };
        /**
         * @brief Padded data type aligned to erase_block_size.
         */
        struct alignas(std::max(4u, erase_block_size)) erase_block_data_t
            : public data_t
        {
            using data_t::data_t;
        } _data;
        static_assert(std::is_trivial<T>::value, "Type T must be trivial.");

    private:
        /**
         * @brief Check if the data is stored in EEPROM.
         */
        void _check_address() const
        {
            // Because this->data is not constexpr as for C++ grammar,
            // this assertion cannot be replaced with static_assert.
            assert(IS_ADDRESS_IN_INTERNAL_FLASH(&_data));
        }

    public:
        /**
         * @brief Default constructor does not make the data valid.
         * @see valid()
         */
        constexpr eeprom() : _data{}
        {
        }
        /**
         * @brief Initialize the data with the given value.
         * This constructor makes the data valid.
         * @see valid()
         */
        constexpr eeprom(const T& initial_value)
        {
            // This assignment is done in compile time.
            // So stack overflow does not occur.
            _data = erase_block_data_t{initial_value};
        }

    public:
        /**
         * @brief Get the address of the data.
         *
         * @return const T* Pointer to the data. Note that it is unwriteable.
         */
        const T* address() const
        {
            _check_address();
            return &(_data.value);
        }
        /**
         * @brief Get the value of the data.
         *
         * @return const T& Value of the data. Note that it is unwriteable.
         */
        const T& value() const
        {
            _check_address();
            return _data.value;
        }
        /**
         * @brief Get the value of the data.
         *
         * @return const T& Value of the data. Note that it is unwriteable.
         */
        operator const T&() const
        {
            return value();
        }

    public:
        /**
         * @brief Check if the data is valid.
         * The data is valid if you have written to EEPROM,
         * or the converting constructor is used.
         */
        bool valid() const
        {
            for (size_t i = 0; i < padding_size; ++i)
                if (_data.prefix[i] != i)
                    return false;
            for (size_t i = 0; i < padding_size; ++i)
                if (_data.suffix[i] != i)
                    return false;
            return true;
        }
        /**
         * @brief Write the data to EEPROM.
         * This takes a long time and consumes the life of EEPROM.
         */
        void write(const T& value) const
        {
            _check_address();
            eeprom_device device;
            // Only store program_block_data_t to avoid stack overflow.
            auto data = program_block_data_t{value};
            // Block should be erased before programming.
            device.erase(reinterpret_cast<mbed::bd_addr_t>(
                             VIRTUAL_ADDRESS_TO_PHYCICAL_ADDRESS(&_data)),
                         sizeof(_data));
            // Only Program program_block_data_t to avoid stack overflow and
            // save time.
            device.program(
                &data,
                reinterpret_cast<mbed::bd_addr_t>(
                    VIRTUAL_ADDRESS_TO_PHYCICAL_ADDRESS(&_data)),
                sizeof(data)); // Program only sizeof(program_block_data_t).
        }
        /**
         * @brief Write the data to EEPROM.
         * This takes a long time and consumes the life of EEPROM.
         */
        const eeprom& operator=(const T& value) const
        {
            write(value);
            return *this;
        }
    };
} // namespace module
