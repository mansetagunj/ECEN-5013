/*
 * board_identification.c
 *
 *  Created on: 23-Apr-2018
 *      Author: Gunj Manseta
 */

#include <stdint.h>
#include <stdbool.h>

#define BOARD_UID_SHIFT 24
//TODO: Move the below constant strings to somewhere suitable
const char * const BOARD_TYPE = "TM4C1294XL";
const char * const OS = "FreeRTOS";
const char * const CODE_VERSION = "v1.0";
const char * const UID = "guma9188";
const uint32_t BOARD_UID = (0xA5U<<BOARD_UID_SHIFT);
