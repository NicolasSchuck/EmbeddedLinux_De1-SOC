/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 * Copyright 2019 NXP
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONFIG_ARMV7_SECURE_BASE	OCRAM_BASE_S_ADDR

#define CONFIG_SYS_FSL_CLK

#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_DEEP_SLEEP

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 16 * 1024 * 1024)

#define CONFIG_SYS_INIT_RAM_ADDR	OCRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	OCRAM_SIZE

#ifndef __ASSEMBLY__
unsigned long get_board_sys_clk(void);
unsigned long get_board_ddr_clk(void);
#endif

#if defined(CONFIG_QSPI_BOOT) || defined(CONFIG_SD_BOOT_QSPI)
#define CONFIG_SYS_CLK_FREQ		100000000
#define CONFIG_DDR_CLK_FREQ		100000000
#define CONFIG_QIXIS_I2C_ACCESS
#else
#define CONFIG_SYS_CLK_FREQ		get_board_sys_clk()
#define CONFIG_DDR_CLK_FREQ		get_board_ddr_clk()
#endif

#ifdef CONFIG_RAMBOOT_PBL
#define CONFIG_SYS_FSL_PBL_PBI	board/freescale/ls1021aqds/ls102xa_pbi.cfg
#endif

#ifdef CONFIG_SD_BOOT
#ifdef CONFIG_SD_BOOT_QSPI
#define CONFIG_SYS_FSL_PBL_RCW	\
	board/freescale/ls1021aqds/ls102xa_rcw_sd_qspi.cfg
#else
#define CONFIG_SYS_FSL_PBL_RCW	\
	board/freescale/ls1021aqds/ls102xa_rcw_sd_ifc.cfg
#endif

#define CONFIG_SPL_MAX_SIZE		0x1a000
#define CONFIG_SPL_STACK		0x1001d000
#define CONFIG_SPL_PAD_TO		0x1c000

#define CONFIG_SYS_SPL_MALLOC_START	(CONFIG_SYS_TEXT_BASE + \
		CONFIG_SYS_MONITOR_LEN)
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x100000
#define CONFIG_SPL_BSS_START_ADDR	0x80100000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000
#define CONFIG_SYS_MONITOR_LEN		0xc0000
#endif

#ifdef CONFIG_NAND_BOOT
#define CONFIG_SYS_FSL_PBL_RCW	board/freescale/ls1021aqds/ls102xa_rcw_nand.cfg

#define CONFIG_SPL_MAX_SIZE		0x1a000
#define CONFIG_SPL_STACK		0x1001d000
#define CONFIG_SPL_PAD_TO		0x1c000

#define CONFIG_SYS_NAND_U_BOOT_SIZE	(400 << 10)
#define CONFIG_SYS_NAND_U_BOOT_OFFS	CONFIG_SPL_PAD_TO
#define CONFIG_SYS_NAND_PAGE_SIZE	2048
#define CONFIG_SYS_NAND_U_BOOT_DST	CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_SPL_MALLOC_START	0x80200000
#define CONFIG_SYS_SPL_MALLOC_SIZE	0x100000
#define CONFIG_SPL_BSS_START_ADDR	0x80100000
#define CONFIG_SPL_BSS_MAX_SIZE		0x80000
#define CONFIG_SYS_MONITOR_LEN		0x80000
#endif

#define CONFIG_DDR_SPD
#define SPD_EEPROM_ADDRESS		0x51
#define CONFIG_SYS_SPD_BUS_NUM		0

#ifndef CONFIG_SYS_FSL_DDR4
#define CONFIG_SYS_DDR_RAW_TIMING
#endif
#define CONFIG_DIMM_SLOTS_PER_CTLR	1
#define CONFIG_CHIP_SELECTS_PER_CTRL	4

#define CONFIG_SYS_DDR_SDRAM_BASE	0x80000000UL
#define CONFIG_SYS_SDRAM_BASE		CONFIG_SYS_DDR_SDRAM_BASE

#define CONFIG_DDR_ECC
#ifdef CONFIG_DDR_ECC
#define CONFIG_ECC_INIT_VIA_DDRCONTROLLER
#define CONFIG_MEM_INIT_VALUE           0xdeadbeef
#endif

/*
 * IFC Definitions
 */
#if !defined(CONFIG_QSPI_BOOT) && !defined(CONFIG_SD_BOOT_QSPI)
#define CONFIG_FSL_IFC
#define CONFIG_SYS_FLASH_BASE		0x60000000
#define CONFIG_SYS_FLASH_BASE_PHYS	CONFIG_SYS_FLASH_BASE

#define CONFIG_SYS_NOR0_CSPR_EXT	(0x0)
#define CONFIG_SYS_NOR0_CSPR	(CSPR_PHYS_ADDR(CONFIG_SYS_FLASH_BASE_PHYS) | \
				CSPR_PORT_SIZE_16 | \
				CSPR_MSEL_NOR | \
				CSPR_V)
#define CONFIG_SYS_NOR1_CSPR_EXT	(0x0)
#define CONFIG_SYS_NOR1_CSPR	(CSPR_PHYS_ADDR(CONFIG_SYS_FLASH_BASE_PHYS \
				+ 0x8000000) | \
				CSPR_PORT_SIZE_16 | \
				CSPR_MSEL_NOR | \
				CSPR_V)
#define CONFIG_SYS_NOR_AMASK		IFC_AMASK(128 * 1024 * 1024)

#define CONFIG_SYS_NOR_CSOR		(CSOR_NOR_ADM_SHIFT(4) | \
					CSOR_NOR_TRHZ_80)
#define CONFIG_SYS_NOR_FTIM0		(FTIM0_NOR_TACSE(0x4) | \
					FTIM0_NOR_TEADC(0x5) | \
					FTIM0_NOR_TEAHC(0x5))
#define CONFIG_SYS_NOR_FTIM1		(FTIM1_NOR_TACO(0x35) | \
					FTIM1_NOR_TRAD_NOR(0x1a) | \
					FTIM1_NOR_TSEQRAD_NOR(0x13))
#define CONFIG_SYS_NOR_FTIM2		(FTIM2_NOR_TCS(0x4) | \
					FTIM2_NOR_TCH(0x4) | \
					FTIM2_NOR_TWPH(0xe) | \
					FTIM2_NOR_TWP(0x1c))
#define CONFIG_SYS_NOR_FTIM3		0

#define CONFIG_SYS_FLASH_QUIET_TEST
#define CONFIG_FLASH_SHOW_PROGRESS	45
#define CONFIG_CFI_FLASH_USE_WEAK_ACCESSORS
#define CONFIG_SYS_WRITE_SWAPPED_DATA

#define CONFIG_SYS_MAX_FLASH_BANKS	2	/* number of banks */
#define CONFIG_SYS_MAX_FLASH_SECT	1024	/* sectors per device */
#define CONFIG_SYS_FLASH_ERASE_TOUT	60000	/* Flash Erase Timeout (ms) */
#define CONFIG_SYS_FLASH_WRITE_TOUT	500	/* Flash Write Timeout (ms) */

#define CONFIG_SYS_FLASH_EMPTY_INFO
#define CONFIG_SYS_FLASH_BANKS_LIST	{CONFIG_SYS_FLASH_BASE_PHYS, \
					CONFIG_SYS_FLASH_BASE_PHYS + 0x8000000}

/*
 * NAND Flash Definitions
 */
#define CONFIG_NAND_FSL_IFC

#define CONFIG_SYS_NAND_BASE		0x7e800000
#define CONFIG_SYS_NAND_BASE_PHYS	CONFIG_SYS_NAND_BASE

#define CONFIG_SYS_NAND_CSPR_EXT	(0x0)

#define CONFIG_SYS_NAND_CSPR	(CSPR_PHYS_ADDR(CONFIG_SYS_NAND_BASE_PHYS) \
				| CSPR_PORT_SIZE_8	\
				| CSPR_MSEL_NAND	\
				| CSPR_V)
#define CONFIG_SYS_NAND_AMASK	IFC_AMASK(64*1024)
#define CONFIG_SYS_NAND_CSOR	(CSOR_NAND_ECC_ENC_EN	/* ECC on encode */ \
				| CSOR_NAND_ECC_DEC_EN	/* ECC on decode */ \
				| CSOR_NAND_ECC_MODE_4	/* 4-bit ECC */ \
				| CSOR_NAND_RAL_3	/* RAL = 3 Bytes */ \
				| CSOR_NAND_PGS_2K	/* Page Size = 2K */ \
				| CSOR_NAND_SPRZ_64	/* Spare size = 64 */ \
				| CSOR_NAND_PB(64))	/* 64 Pages Per Block */

#define CONFIG_SYS_NAND_ONFI_DETECTION

#define CONFIG_SYS_NAND_FTIM0		(FTIM0_NAND_TCCST(0x7) | \
					FTIM0_NAND_TWP(0x18)   | \
					FTIM0_NAND_TWCHT(0x7) | \
					FTIM0_NAND_TWH(0xa))
#define CONFIG_SYS_NAND_FTIM1		(FTIM1_NAND_TADLE(0x32) | \
					FTIM1_NAND_TWBE(0x39)  | \
					FTIM1_NAND_TRR(0xe)   | \
					FTIM1_NAND_TRP(0x18))
#define CONFIG_SYS_NAND_FTIM2		(FTIM2_NAND_TRAD(0xf) | \
					FTIM2_NAND_TREH(0xa) | \
					FTIM2_NAND_TWHRE(0x1e))
#define CONFIG_SYS_NAND_FTIM3           0x0

#define CONFIG_SYS_NAND_BASE_LIST	{ CONFIG_SYS_NAND_BASE }
#define CONFIG_SYS_MAX_NAND_DEVICE	1

#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 * 1024)
#endif

/*
 * QIXIS Definitions
 */
#define CONFIG_FSL_QIXIS

#ifdef CONFIG_FSL_QIXIS
#define QIXIS_BASE			0x7fb00000
#define QIXIS_BASE_PHYS			QIXIS_BASE
#define CONFIG_SYS_I2C_FPGA_ADDR	0x66
#define QIXIS_LBMAP_SWITCH		6
#define QIXIS_LBMAP_MASK		0x0f
#define QIXIS_LBMAP_SHIFT		0
#define QIXIS_LBMAP_DFLTBANK		0x00
#define QIXIS_LBMAP_ALTBANK		0x04
#define QIXIS_PWR_CTL			0x21
#define QIXIS_PWR_CTL_POWEROFF		0x80
#define QIXIS_RST_CTL_RESET		0x44
#define QIXIS_RCFG_CTL_RECONFIG_IDLE	0x20
#define QIXIS_RCFG_CTL_RECONFIG_START	0x21
#define QIXIS_RCFG_CTL_WATCHDOG_ENBLE	0x08
#define QIXIS_CTL_SYS			0x5
#define QIXIS_CTL_SYS_EVTSW_MASK	0x0c
#define QIXIS_CTL_SYS_EVTSW_IRQ		0x04
#define QIXIS_RST_FORCE_3		0x45
#define QIXIS_RST_FORCE_3_PCIESLOT1	0x80
#define QIXIS_PWR_CTL2			0x21
#define QIXIS_PWR_CTL2_PCTL		0x2

#define CONFIG_SYS_FPGA_CSPR_EXT	(0x0)
#define CONFIG_SYS_FPGA_CSPR		(CSPR_PHYS_ADDR(QIXIS_BASE_PHYS) | \
					CSPR_PORT_SIZE_8 | \
					CSPR_MSEL_GPCM | \
					CSPR_V)
#define CONFIG_SYS_FPGA_AMASK		IFC_AMASK(64 * 1024)
#define CONFIG_SYS_FPGA_CSOR		(CSOR_NOR_ADM_SHIFT(4) | \
					CSOR_NOR_NOR_MODE_AVD_NOR | \
					CSOR_NOR_TRHZ_80)

/*
 * QIXIS Timing parameters for IFC GPCM
 */
#define CONFIG_SYS_FPGA_FTIM0		(FTIM0_GPCM_TACSE(0xe) | \
					FTIM0_GPCM_TEADC(0xe) | \
					FTIM0_GPCM_TEAHC(0xe))
#define CONFIG_SYS_FPGA_FTIM1		(FTIM1_GPCM_TACO(0xe) | \
					FTIM1_GPCM_TRAD(0x1f))
#define CONFIG_SYS_FPGA_FTIM2		(FTIM2_GPCM_TCS(0xe) | \
					FTIM2_GPCM_TCH(0xe) | \
					FTIM2_GPCM_TWP(0xf0))
#define CONFIG_SYS_FPGA_FTIM3		0x0
#endif

#if defined(CONFIG_NAND_BOOT)
#define CONFIG_SYS_CSPR0_EXT		CONFIG_SYS_NAND_CSPR_EXT
#define CONFIG_SYS_CSPR0		CONFIG_SYS_NAND_CSPR
#define CONFIG_SYS_AMASK0		CONFIG_SYS_NAND_AMASK
#define CONFIG_SYS_CSOR0		CONFIG_SYS_NAND_CSOR
#define CONFIG_SYS_CS0_FTIM0		CONFIG_SYS_NAND_FTIM0
#define CONFIG_SYS_CS0_FTIM1		CONFIG_SYS_NAND_FTIM1
#define CONFIG_SYS_CS0_FTIM2		CONFIG_SYS_NAND_FTIM2
#define CONFIG_SYS_CS0_FTIM3		CONFIG_SYS_NAND_FTIM3
#define CONFIG_SYS_CSPR1_EXT		CONFIG_SYS_NOR0_CSPR_EXT
#define CONFIG_SYS_CSPR1		CONFIG_SYS_NOR0_CSPR
#define CONFIG_SYS_AMASK1		CONFIG_SYS_NOR_AMASK
#define CONFIG_SYS_CSOR1		CONFIG_SYS_NOR_CSOR
#define CONFIG_SYS_CS1_FTIM0		CONFIG_SYS_NOR_FTIM0
#define CONFIG_SYS_CS1_FTIM1		CONFIG_SYS_NOR_FTIM1
#define CONFIG_SYS_CS1_FTIM2		CONFIG_SYS_NOR_FTIM2
#define CONFIG_SYS_CS1_FTIM3		CONFIG_SYS_NOR_FTIM3
#define CONFIG_SYS_CSPR2_EXT		CONFIG_SYS_NOR1_CSPR_EXT
#define CONFIG_SYS_CSPR2		CONFIG_SYS_NOR1_CSPR
#define CONFIG_SYS_AMASK2		CONFIG_SYS_NOR_AMASK
#define CONFIG_SYS_CSOR2		CONFIG_SYS_NOR_CSOR
#define CONFIG_SYS_CS2_FTIM0		CONFIG_SYS_NOR_FTIM0
#define CONFIG_SYS_CS2_FTIM1		CONFIG_SYS_NOR_FTIM1
#define CONFIG_SYS_CS2_FTIM2		CONFIG_SYS_NOR_FTIM2
#define CONFIG_SYS_CS2_FTIM3		CONFIG_SYS_NOR_FTIM3
#define CONFIG_SYS_CSPR3_EXT		CONFIG_SYS_FPGA_CSPR_EXT
#define CONFIG_SYS_CSPR3		CONFIG_SYS_FPGA_CSPR
#define CONFIG_SYS_AMASK3		CONFIG_SYS_FPGA_AMASK
#define CONFIG_SYS_CSOR3		CONFIG_SYS_FPGA_CSOR
#define CONFIG_SYS_CS3_FTIM0		CONFIG_SYS_FPGA_FTIM0
#define CONFIG_SYS_CS3_FTIM1		CONFIG_SYS_FPGA_FTIM1
#define CONFIG_SYS_CS3_FTIM2		CONFIG_SYS_FPGA_FTIM2
#define CONFIG_SYS_CS3_FTIM3		CONFIG_SYS_FPGA_FTIM3
#else
#define CONFIG_SYS_CSPR0_EXT		CONFIG_SYS_NOR0_CSPR_EXT
#define CONFIG_SYS_CSPR0		CONFIG_SYS_NOR0_CSPR
#define CONFIG_SYS_AMASK0		CONFIG_SYS_NOR_AMASK
#define CONFIG_SYS_CSOR0		CONFIG_SYS_NOR_CSOR
#define CONFIG_SYS_CS0_FTIM0		CONFIG_SYS_NOR_FTIM0
#define CONFIG_SYS_CS0_FTIM1		CONFIG_SYS_NOR_FTIM1
#define CONFIG_SYS_CS0_FTIM2		CONFIG_SYS_NOR_FTIM2
#define CONFIG_SYS_CS0_FTIM3		CONFIG_SYS_NOR_FTIM3
#define CONFIG_SYS_CSPR1_EXT		CONFIG_SYS_NOR1_CSPR_EXT
#define CONFIG_SYS_CSPR1		CONFIG_SYS_NOR1_CSPR
#define CONFIG_SYS_AMASK1		CONFIG_SYS_NOR_AMASK
#define CONFIG_SYS_CSOR1		CONFIG_SYS_NOR_CSOR
#define CONFIG_SYS_CS1_FTIM0		CONFIG_SYS_NOR_FTIM0
#define CONFIG_SYS_CS1_FTIM1		CONFIG_SYS_NOR_FTIM1
#define CONFIG_SYS_CS1_FTIM2		CONFIG_SYS_NOR_FTIM2
#define CONFIG_SYS_CS1_FTIM3		CONFIG_SYS_NOR_FTIM3
#define CONFIG_SYS_CSPR2_EXT		CONFIG_SYS_NAND_CSPR_EXT
#define CONFIG_SYS_CSPR2		CONFIG_SYS_NAND_CSPR
#define CONFIG_SYS_AMASK2		CONFIG_SYS_NAND_AMASK
#define CONFIG_SYS_CSOR2		CONFIG_SYS_NAND_CSOR
#define CONFIG_SYS_CS2_FTIM0		CONFIG_SYS_NAND_FTIM0
#define CONFIG_SYS_CS2_FTIM1		CONFIG_SYS_NAND_FTIM1
#define CONFIG_SYS_CS2_FTIM2		CONFIG_SYS_NAND_FTIM2
#define CONFIG_SYS_CS2_FTIM3		CONFIG_SYS_NAND_FTIM3
#define CONFIG_SYS_CSPR3_EXT		CONFIG_SYS_FPGA_CSPR_EXT
#define CONFIG_SYS_CSPR3		CONFIG_SYS_FPGA_CSPR
#define CONFIG_SYS_AMASK3		CONFIG_SYS_FPGA_AMASK
#define CONFIG_SYS_CSOR3		CONFIG_SYS_FPGA_CSOR
#define CONFIG_SYS_CS3_FTIM0		CONFIG_SYS_FPGA_FTIM0
#define CONFIG_SYS_CS3_FTIM1		CONFIG_SYS_FPGA_FTIM1
#define CONFIG_SYS_CS3_FTIM2		CONFIG_SYS_FPGA_FTIM2
#define CONFIG_SYS_CS3_FTIM3		CONFIG_SYS_FPGA_FTIM3
#endif

/*
 * Serial Port
 */
#ifdef CONFIG_LPUART
#define CONFIG_LPUART_32B_REG
#else
#define CONFIG_SYS_NS16550_SERIAL
#ifndef CONFIG_DM_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	1
#endif
#define CONFIG_SYS_NS16550_CLK		get_serial_clock()
#endif

/*
 * I2C
 */
#if !CONFIG_IS_ENABLED(DM_I2C)
#define CONFIG_SYS_I2C
#else
#define CONFIG_I2C_SET_DEFAULT_BUS_NUM
#define CONFIG_I2C_DEFAULT_BUS_NUMBER 0
#endif
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_MXC_I2C1		/* enable I2C bus 1 */
#define CONFIG_SYS_I2C_MXC_I2C2		/* enable I2C bus 2 */
#define CONFIG_SYS_I2C_MXC_I2C3		/* enable I2C bus 3 */

/* GPIO */
#ifdef CONFIG_DM_GPIO
#ifndef CONFIG_MPC8XXX_GPIO
#define CONFIG_MPC8XXX_GPIO
#endif
#endif

/* EEPROM */
#define CONFIG_ID_EEPROM
#define CONFIG_SYS_I2C_EEPROM_NXID
#define CONFIG_SYS_EEPROM_BUS_NUM		0
#define CONFIG_SYS_I2C_EEPROM_ADDR		0x57
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN		1
#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	3
#define CONFIG_SYS_EEPROM_PAGE_WRITE_DELAY_MS	5

/*
 * I2C bus multiplexer
 */
#define I2C_MUX_PCA_ADDR_PRI		0x77
#define I2C_MUX_CH_DEFAULT		0x8
#define I2C_MUX_CH_CH7301		0xC

/*
 * MMC
 */

/*
 * Video
 */
#ifdef CONFIG_VIDEO_FSL_DCU_FB
#define CONFIG_VIDEO_LOGO
#define CONFIG_VIDEO_BMP_LOGO

#define CONFIG_FSL_DIU_CH7301
#define CONFIG_SYS_I2C_DVI_BUS_NUM	0
#define CONFIG_SYS_I2C_QIXIS_ADDR	0x66
#define CONFIG_SYS_I2C_DVI_ADDR		0x75
#endif

/*
 * eTSEC
 */

#ifdef CONFIG_TSEC_ENET
#define CONFIG_MII_DEFAULT_TSEC		3
#define CONFIG_TSEC1			1
#define CONFIG_TSEC1_NAME		"eTSEC1"
#define CONFIG_TSEC2			1
#define CONFIG_TSEC2_NAME		"eTSEC2"
#define CONFIG_TSEC3			1
#define CONFIG_TSEC3_NAME		"eTSEC3"

#define TSEC1_PHY_ADDR			1
#define TSEC2_PHY_ADDR			2
#define TSEC3_PHY_ADDR			3

#define TSEC1_FLAGS			(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC2_FLAGS			(TSEC_GIGABIT | TSEC_REDUCED)
#define TSEC3_FLAGS			(TSEC_GIGABIT | TSEC_REDUCED)

#define TSEC1_PHYIDX			0
#define TSEC2_PHYIDX			0
#define TSEC3_PHYIDX			0

#define CONFIG_ETHPRIME			"eTSEC1"

#define CONFIG_HAS_ETH0
#define CONFIG_HAS_ETH1
#define CONFIG_HAS_ETH2

#define CONFIG_FSL_SGMII_RISER		1
#define SGMII_RISER_PHY_OFFSET		0x1b

#ifdef CONFIG_FSL_SGMII_RISER
#define CONFIG_SYS_TBIPA_VALUE		8
#endif

#endif

/* PCIe */
#define CONFIG_PCIE1		/* PCIE controller 1 */
#define CONFIG_PCIE2		/* PCIE controller 2 */

#ifdef CONFIG_PCI
#define CONFIG_PCI_SCAN_SHOW
#endif

#define CONFIG_CMDLINE_TAG

#define CONFIG_PEN_ADDR_BIG_ENDIAN
#define CONFIG_LAYERSCAPE_NS_ACCESS
#define CONFIG_SMP_PEN_ADDR		0x01ee0200
#define COUNTER_FREQUENCY		12500000

#define CONFIG_HWCONFIG
#define HWCONFIG_BUFFER_SIZE		256

#define CONFIG_FSL_DEVICE_DISABLE


#define CONFIG_SYS_QE_FW_ADDR     0x60940000

#ifdef CONFIG_LPUART
#define CONFIG_EXTRA_ENV_SETTINGS       \
	"bootargs=root=/dev/ram0 rw console=ttyLP0,115200\0" \
	"initrd_high=0xffffffff\0"      \
	"hwconfig=fsl_ddr:ctlr_intlv=null,bank_intlv=null\0"
#else
#define CONFIG_EXTRA_ENV_SETTINGS	\
	"bootargs=root=/dev/ram0 rw console=ttyS0,115200\0" \
	"initrd_high=0xffffffff\0"      \
	"hwconfig=fsl_ddr:ctlr_intlv=null,bank_intlv=null\0"
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_BOOTMAPSZ		(256 << 20)

#define CONFIG_SYS_LOAD_ADDR		0x82000000

#define CONFIG_LS102XA_STREAM_ID

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SYS_MONITOR_BASE CONFIG_SPL_TEXT_BASE
#else
#define CONFIG_SYS_MONITOR_BASE CONFIG_SYS_TEXT_BASE    /* start of monitor */
#endif

/*
 * Environment
 */

#include <asm/fsl_secure_boot.h>
#define CONFIG_SYS_BOOTM_LEN	(64 << 20) /* Increase max gunzip size */

#endif
