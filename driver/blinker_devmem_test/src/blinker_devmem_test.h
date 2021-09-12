/*
 * blinkder_devmem_test.h
 *
 *  Created on: Sep 11, 2021
 *      Author: nicolas
 */

#ifndef SRC_BLINKDER_DEVMEM_TEST_H_
#define SRC_BLINKDER_DEVMEM_TEST_H_


#define BLINKER_SYSFS_ENTRY "/sys/bus/platform/devices/ff240000.blinker"
#define BLINKER_PROCFS_ENTRY "/proc/device-tree/sopc@0/bridge@0xff200000/blinker@0x100040000"
// Offset address of the blinker registers
#define BLINKER_REG_SPEED_OFFSET 0x0
#define BLINKER_REG_CONFIG_OFFSET 0x1
#define BLINKER_REG_LEDS_OFFSET 0x0
#define BLINKER_REG_STATE_OFFSET 0x1


//
// usage string
//
#define USAGE_STR "\
\n\
Usage: blinker_devmem [ONE-OPTION-ONLY]\n\
-d, --write_speed speed_value\n\
-c, --write_config config_value\n\
-s, --read_state\n\
-l, --read_leds\n\
-h, --help\n\
\n\
"
//
// help string
//
#define HELP_STR "\
\n\
Only one of the following options may be passed in per invocation:\n\
\n\
-d, --write_speed speed_value\n\
Set the speed of blink\n\
15: max speed\n\
1: min speed\n\
\n\
-c, --write_config config_value\n\
Set the operation mode. Bit pattern: 000000ms.\n\
m: 0 (sweep) 1 (blink)\n\
s: 0 (stopped) 1 (running)\n\
\n\
-s, --read_state\n\
Read the current configuration and speed.\n\
\n\
-l, --read_leds\n\
Read the current leds position.\n\
\n\
-h, --help\n\
Display this help message.\n\
\n\
"



#endif /* SRC_BLINKDER_DEVMEM_TEST_H_ */
