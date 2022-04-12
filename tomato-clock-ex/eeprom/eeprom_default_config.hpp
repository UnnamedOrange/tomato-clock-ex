/**
 * @file eeprom_default_config.hpp
 * @author UnnamedOrange
 * @brief Define the default configuration for the eeprom library.
 * @remark You can change the configuration by defining the following macros:
 * @remark - INTERNAL_FLASH_BASE_ADDRESS
 * @remark - INTERNAL_FLASH_SIZE
 * @remark - INTERNAL_FLASH_READ_BLOCK_SIZE
 * @remark - INTERNAL_FLASH_PROGRAM_BLOCK_SIZE
 * @remark - INTERNAL_FLASH_ERASE_BLOCK_SIZE
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

/**
 * @brief Memory mapping of internal flash. STM32F401RE by default.
 */

#ifndef INTERNAL_FLASH_BASE_ADDRESS
#define INTERNAL_FLASH_BASE_ADDRESS ((void*)0x08000000)
#endif

#ifndef INTERNAL_FLASH_SIZE
#define INTERNAL_FLASH_SIZE ((size_t)0x80000)
#endif

/**
 * @brief Parameters of internal flash. STM32F401RE by default.
 */

#ifndef INTERNAL_FLASH_READ_BLOCK_SIZE
#define INTERNAL_FLASH_READ_BLOCK_SIZE ((size_t)1)
#endif

#ifndef INTERNAL_FLASH_PROGRAM_BLOCK_SIZE
#define INTERNAL_FLASH_PROGRAM_BLOCK_SIZE ((size_t)1)
#endif

#ifndef INTERNAL_FLASH_ERASE_BLOCK_SIZE
#define INTERNAL_FLASH_ERASE_BLOCK_SIZE ((size_t)16384)
#endif

/**
 * @brief Utils for checking the address.
 */
#define INTERNAL_FLASH_END_ADDRESS                                             \
    ((void*)((size_t)INTERNAL_FLASH_BASE_ADDRESS + INTERNAL_FLASH_SIZE))
#define IS_ADDRESS_IN_INTERNAL_FLASH(address)                                  \
    ((void*)INTERNAL_FLASH_BASE_ADDRESS <= (address) &&                        \
     (address) < INTERNAL_FLASH_END_ADDRESS)

/**
 * @brief Utils for converting the address.
 */
#define VIRTUAL_ADDRESS_TO_PHYCICAL_ADDRESS(address)                           \
    ((void*)((size_t)address - (size_t)INTERNAL_FLASH_BASE_ADDRESS))
