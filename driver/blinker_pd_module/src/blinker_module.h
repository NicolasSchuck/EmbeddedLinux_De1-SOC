/*
 * blinker_module.h
 *
 *  Created on: Sep 13, 2021
 *      Author: nicolas
 */

#ifndef BLINKER_MODULE_H_
#define BLINKER_MODULE_H_


/* Common useful macros */
#define SYSTEM_BUS_WIDTH (8)

#define __IO_CALC_ADDRESS_NATIVE(BASE, REGNUM)\
        ((void *)(((unsigned char*)BASE) + ((REGNUM) * (SYSTEM_BUS_WIDTH/8))))

#define IORD(BASE, REGNUM) \
        * (unsigned long*)(__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM)))

#define IOWR(BASE, REGNUM, DATA) \
        * (unsigned long*)(__IO_CALC_ADDRESS_NATIVE ((BASE), (REGNUM))) = (DATA)

/* Blinker peripheral parameters */
#define BLINKER_BASE 0xff240000
#define BLINKER_SIZE PAGE_SIZE

/* SPEED register */
#define BLINKER_SPEED_REG 0

#define IOADDR_BLINKER_SPEED(base)      __IO_CALC_ADDRESS_NATIVE(base, BLINKER_SPEED_REG)

#define IOWR_BLINKER_SPEED(base, data)  IOWR(base, BLINKER_SPEED_REG, data)

#define BLINKER_SPEED_MSK (0x0f)

/* CONFIG register */
#define BLINKER_CONFIG_REG 1
#define IOADDR_BLINKER_CONFIG(base)    __IO_CALC_ADDRESS_NATIVE(base, BLINKER_CONFIG_REG)

#define IOWR_BLINKER_CONFIG(base, data) IOWR(base, BLINKER_CONFIG_REG, data)
#define BLINKER_CONFIG_MSK          0x07
#define BLINKER_START_MSK           0x01
#define BLINKER_STOP_MSK            0xfe
#define BLINKER_BLINK_MSK           0x02
#define BLINKER_SHIFT_MSK           0xfd
#define BLINKER_IENA_MSK            0x04
#define BLINKER_IDIS_MSK            0xfb

/* LEDS register */
#define BLINKER_LEDS_REG    0

#define IOADDR_BLINKER_LEDS(base)   __IO_CALC_ADDRESS_NATIVE(base, BLINKER_LEDS_REG)

#define IORD_BLINKER_LEDS(base)     IORD(base, BLINKER_LEDS_REG)
#define BLINKER_LEDS_MSK            0x0f

/* STATE register */
#define BLINKER_STATE_REG 1
#define IOADDR_BLINKER_STATE(base)  __IO_CALC_ADDRESS_NATIVE(base, BLINKER_STATE_REG)

#define IORD_BLINKER_STATE(base)    IORD(base, BLINKER_STATE_REG)
#define BLINKER_STATE_MSK           0xff

/* ioctl values */
#define OUR_IOC_TYPE        0xee
#define IOC_SET_SPEED       _IOW(OUT_IOC_TYPE, 0, u8)
#define IOC_SET_RUNNING     _IO(OUR_IOC_TYPE, 1)
#define IOC_CLEAR_RUNNING   _IO(OUR_IOC_TYPE, 2)
#define IOC_ENA_INT         _IO(OUR_IOC_TYPE, 6)
#define IOC_DIS_INT         _IO(OUR_IOC_TYPE, 7)
#define IOC_SET_MODE        _IOW(OUR_IOC_TYPE, 3, u8)
#define IOC_GET_LEDS        _IOR(OUR_IOC_TYPE, 5, u8)

#endif /* BLINKER_MODULE_H_ */
