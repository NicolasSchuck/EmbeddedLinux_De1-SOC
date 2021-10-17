/*
 * blinkder_devmem_test.h
 *
 *  Created on: Sep 11, 2021
 *      Author: nicolas
 */

#ifndef SRC_BLINKDER_DEVMEM_TEST_H_
#define SRC_BLINKDER_DEVMEM_TEST_H_


#define GPIO_TEST_SYSFS_ENTRY "/sys/bus/platform/devices/ff240000.test"
#define GPIO_TEST_PROCFS_ENTRY "/proc/device-tree/sopc@0/bridge@0xff200000/test@0x100040000"
// Offset address of the gpio_test registers
#define GPIO_TEST_REG_SWITCHES_OFFSET 0x0
#define GPIO_TEST_REG_LEDS_OFFSET 0x0


//
// usage string
//
#define USAGE_STR "\
\n\
Usage: gpio_test_devmem [ONE-OPTION-ONLY]\n\
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
