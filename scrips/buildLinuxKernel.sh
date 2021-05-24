#!/bin/bash
echo "--build_linux_kernel.sh--"
cd ..
export set LINUX_TOP=`pwd`/linux-socfga.a9
rm -rf linux-kernel && mkdir linux-bin
echo "removed and reinitialized folder..."
export set LINUX_BIN=`pwd`/linux-bin
mkdir -p $LINUX_BIN/a9
mkdir -p $LINUX_BIN/a53

echo "config toolchain..."
cd $LINUX_TOP
export ARCH=arm
export CROSS_COMPILE=arm-none-linux-gnueabihf-

echo "config .dts"
sed -i 's/120MB for jffs2 data/56MB for jffs2 data/g' arch/arm/boot/dts/socfpga_cyclone5_socdk.dts
sed -i 's/<0x800000 0x7800000>;/<0x800000 0x3800000>;/g' arch/arm/boot/dts/socfpga_cyclone5_socdk.dts

echo "make kernel"
make socfpga_defconfig
make -j 48 zImage Image dtbs modulesec
make -j 48 modules_install INSTALL_MOD_PATH=modules_install
rm -rf modules_install/lib/modules/*/build
rm -rf modules_install/lib/modules/*/source

echo "linking all relevant files to LINUX_BIN"
ln -s $LINUX_TOP/linux-socfpga.a9/arch/arm/boot/zImage $LINUX_BIN/a9/
ln -s $LINUX_TOP/linux-socfpga.a9/arch/arm/boot/Image $LINUX_BIN/a9/
ln -s $LINUX_TOP/linux-socfpga.a9/arch/arm/boot/dts/socfpga_cyclone5_socdk.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/linux-socfpga.a9/arch/arm/boot/dts/socfpga_arria10_socdk_sdmmc.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/linux-socfpga.a9/arch/arm/boot/dts/socfpga_arria10_socdk_qspi.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/linux-socfpga.a9/arch/arm/boot/dts/socfpga_arria10_socdk_nand.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/linux-socfpga.a9/modules_install/lib/modules $LINUX_BIN/a9/

echo "include hardware design"
cd ~/EmbeddedLinux_De1-SOC/
export TOP_FOLDER=`pwd`

echo "reinit software folder"
rm -rf software
mkdir -p Quartus/software/bootloader

echo "copy u-boot"
cp -rv u-boot-socfpga Quartus/software/bootloader

echo "run BSP"
~/intelFPGA/20.1/embedded/embedded_command_shell.sh \
bsp-create-settings \
   --type spl \
   --bsp-dir software/bootloader \
   --preloader-settings-dir "hps_isw_handoff/soc_system_hps_0" \
   --settings software/bootloader/settings.bsp

echo "run qts-Filter"
cd $TOP_FOLDER/Quartus/software/bootloader/u-boot-socfpga
./arch/arm/mach-socfpga/qts-filter.sh cyclone5 ../../../ ../ ./board/altera/cyclone5-socdk/qts/

echo "configure and build u-boot"
cd $TOP_FOLDER/Quartus/software/bootloader/u-boot-socfpga
export CROSS_COMPILE=arm-none-linux-gnueabihf-
make socfpga_cyclone5_defconfig
make -j 48

echo "prepare SD-Card"
