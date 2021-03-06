#!/bin/bash
echo "--build_linux_kernel.sh--"
echo "config compiler"
export PATH=~/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin:$PATH
cd ..
export set TOP_FOLDER=`pwd`
export set LINUX_TOP=`pwd`/linux-socfpga.a9
rm -rf linux-bin && mkdir linux-bin
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
make -j 48 zImage Image dtbs modules
make -j 48 modules_install INSTALL_MOD_PATH=modules_install
rm -rf modules_install/lib/modules/*/build
rm -rf modules_install/lib/modules/*/source

echo "linking all relevant files to LINUX_BIN"
ln -s $LINUX_TOP/arch/arm/boot/zImage $LINUX_BIN/a9/
ln -s $LINUX_TOP/arch/arm/boot/Image $LINUX_BIN/a9/
ln -s $LINUX_TOP/arch/arm/boot/dts/socfpga_cyclone5_socdk.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/arch/arm/boot/dts/socfpga_arria10_socdk_sdmmc.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/arch/arm/boot/dts/socfpga_arria10_socdk_qspi.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/arch/arm/boot/dts/socfpga_arria10_socdk_nand.dtb $LINUX_BIN/a9/
ln -s $LINUX_TOP/modules_install/lib/modules $LINUX_BIN/a9/

echo "Build rootfs"
cd $TOP_FOLDER
mkdir rootfs 
cd rootfs
export set ROOTFS_TOP=`pwd`
echo $ROOTFS_TOP

mkdir cv
cd cv
git clone -b gatesgarth git://git.yoctoproject.org/poky.git
git clone -b master git://git.yoctoproject.org/meta-intel-fpga.git
source poky/oe-init-build-env ./build
echo 'MACHINE = "cyclone5"' >> conf/local.conf
echo 'BBLAYERS += " ${TOPDIR}/../meta-intel-fpga "' >> conf/bblayers.conf
# echo 'CORE_IMAGE_EXTRA_INSTALL += "openssh gdbserver"' >> conf/local.conf
bitbake core-image-minimal
ln -s $ROOTFS_TOP/cv/build/tmp/deploy/images/cyclone5/core-image-minimal-cyclone5.tar.gz $LINUX_BIN/a9/

echo "include hardware design"
cd $TOP_FOLDER
mkdir -p rootfs/cv/build/soc_ghrd
cp -rf Quartus rootfs/cv/build/soc_ghrd
export set QUARTUS=$TOP_FOLDER/rootfs/cv/build/soc_ghrd/Quartus

echo "reinit software folder"
rm -rf $QUARTUS/software
mkdir -p $QUARTUS/software/bootloader

echo "copy u-boot"
cp -rv $TOP_FOLDER/u-boot-socfpga $QUARTUS/software/bootloader

echo "run BSP"
~/intelFPGA/20.1/embedded/embedded_command_shell.sh \
bsp-create-settings \
   --type spl \
   --bsp-dir $QUARTUS/software/bootloader \
   --preloader-settings-dir "$QUARTUS/hps_isw_handoff/soc_system_hps_0" \
   --settings $QUARTUS/software/bootloader/settings.bsp

echo "run qts-Filter"
cd $QUARTUS/software/bootloader/u-boot-socfpga
./arch/arm/mach-socfpga/qts-filter.sh cyclone5 ../../../ ../ ./board/altera/cyclone5-socdk/qts/

echo "configure and build u-boot"
cd $QUARTUS/software/bootloader/u-boot-socfpga
export CROSS_COMPILE=arm-none-linux-gnueabihf-
make socfpga_cyclone5_defconfig
make -j 48

echo "prepare SD-Card"
cd $TOP_FOLDER/rootfs/cv/build/soc_ghrd
sudo rm -rf sd_card && mkdir sd_card && cd sd_card
export set SD_CARD=`pwd`
wget https://releases.rocketboards.org/release/2020.05/gsrd/tools/make_sdimage_p3.py
chmod +x make_sdimage_p3.py

echo "prepare FAT partition"
mkdir sdfs &&  cd sdfs
cp $LINUX_BIN/a9/zImage .
cp $LINUX_BIN/a9/socfpga_cyclone5_socdk.dtb .
mkdir extlinux
echo "LABEL Linux Default" > extlinux/extlinux.conf
echo "    KERNEL ../zImage" >> extlinux/extlinux.conf
echo "    FDT ../socfpga_cyclone5_socdk.dtb" >> extlinux/extlinux.conf
echo "    APPEND root=/dev/mmcblk0p2 rw rootwait earlyprintk console=ttyS0,115200n8" >> extlinux/extlinux.conf

echo "prepare rootfs partition"
cd $SD_CARD
mkdir rootfs && cd rootfs
sudo tar xf $LINUX_BIN/a9/core-image-minimal-cyclone5.tar.gz
sudo rm -rf lib/modules/*
sudo cp -r $LINUX_BIN/a9/modules/* lib/modules

echo "copy over u-boot bootable bin file"
cd $SD_CARD
cp $QUARTUS/software/bootloader/u-boot-socfpga/u-boot-with-spl.sfp .

echo "prepare SD card image"
sudo python3 ./make_sdimage_p3.py -f \
-P u-boot-with-spl.sfp,num=3,format=raw,size=10M,type=A2  \
-P sdfs/*,num=1,format=fat32,size=100M \
-P rootfs/*,num=2,format=ext3,size=300M \
-s 512M \
-n sdcard_cv.img
