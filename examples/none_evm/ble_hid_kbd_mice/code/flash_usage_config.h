/**
 * Copyright (c) 2020, Freqchip
 * 
 * All rights reserved.
 * 
 * 
 */

#ifndef _FLASH_USAGE_CONFIG_H
#define _FLASH_USAGE_CONFIG_H

/*
 * MACROS (∫Í∂®“Â)
 */

/*
 * the following MACROS is used to define flash space used in SDK.
 * user can change these values according their applications.
 */
#define FOR_4M_FLASH		// such as FR8016HA FR8012HA
//#define FOR_2M_FLASH		// such as FR8012HB

#ifdef FOR_4M_FLASH
	#define JUMP_TABLE_STATIC_KEY_OFFSET    0x7F000
	#define BLE_BONDING_INFO_SAVE_ADDR      0x7D000
	#define BLE_REMOTE_SERVICE_SAVE_ADDR    0x7E000
#endif	// FOR_4M_FLASH

#ifdef FOR_2M_FLASH
	#define JUMP_TABLE_STATIC_KEY_OFFSET    0x3F000
	#define BLE_BONDING_INFO_SAVE_ADDR      0x3D000
	#define BLE_REMOTE_SERVICE_SAVE_ADDR    0x3E000
#endif	//FOR_2M_FLASH

/*
 * uncomment this MACRO if user need protect flash from unexpected erase or write operation
 */
//#define FLASH_PROTECT

#endif	// _FLASH_USAGE_CONFIG_H

