#!/bin/bash
echo u-boot

wget https://developer.arm.com/-/media/Files/downloads/gnu-a/10.2-2020.11/binrel/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz
tar xf gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf.tar.xz 


export set QUARTUS=/home/nicolas/Documents/EmbeddedLinux_De1-SOC/Quartus
export PATH=~/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin:$PATH
export CROSS_COMPILE=arm-none-linux-gnueabihf-
echo config u-boot -> dir EmbeddedLinux_De1-SOC/u-boot-socfpga

mkdir -p $QUARTUS/software/bootloader

cp -rv $QUARTUS/../u-boot-socfpga $QUARTUS/software/bootloader

echo "run BSP"
~/intelFPGA/20.1/embedded/embedded_command_shell.sh \
bsp-create-settings \
   --type spl \
   --bsp-dir $QUARTUS/software/bootloader \
   --preloader-settings-dir "$QUARTUS/hps_isw_handoff/soc_system_hps_0" \
   --settings $QUARTUS/software/bootloader/settings.bsp

echo "run qts-Filter"
cd $QUARTUS/software/bootloader/u-boot-socfpga
./arch/arm/mach-socfpga/qts-filter.sh cyclone5 ../../../ ../ ./board/terasic/de1-soc/qts/


make socfpga_de1_soc_defconfig

make





