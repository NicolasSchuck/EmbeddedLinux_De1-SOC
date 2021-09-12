#!/bin/bash
echo "--generate .rbf--"
cd ~/intelFPGA/18.1/embedded
./embedded_command_shell.sh 
cd ~/Documents/EmbeddedLinux_De1-SOC/DE1-SoC/output_files
quartus_cpf -c -o bitstream_compression=on DE1-SoC.sof DE1_SoC.rbf
echo "--done--"


echo "generate dtb"
export PATH=$PATH:~/intelFPGA/18.1/embedded/
./embedded_command_shell.sh
sopc2dts --input DE1_SoC.sopcinfo --output DE1_SOC.dtb --type dtb --board soc_system_board_info.xml --board hps_common_board_info.xml --clocks

"generate dts"
sopc2dts --input DE1_SoC.sopcinfo --output DE1_SOC.dts --type dts --board soc_system_board_info.xml --board hps_common_board_info.xml --clocks


# after compile blinker

# 0. sudo
sudo dir

# 1. rbf
cd ~/intelFPGA/18.1/embedded
./embedded_command_shell.sh 
cd ~/Documents/EmbeddedLinux_De1-SOC/DE1-SoC/output_files
quartus_cpf -c -o bitstream_compression=on DE1-SoC.sof DE1_SoC.rbf

# 2. bsp-editor -> handoff/hps_0 dir
# -> generate
cd ..
cd software/spl_bsp
bsp-editor
make

# 3. generate dts
cd ../..
sopc2dts --input DE1_SoC.sopcinfo --output DE1_SOC.dts --type dts --board soc_system_board_info.xml --board hps_common_board_info.xml --clocks

# 4. rename in dts at blinker_0
# unknown to blinker
# compatible = "blinker,driver-1.0";

# 5. copy dts to buildroot

# 6. make buildroot

# 7. copy files to sdcard dir

# 8. extract rootfs

mkdir rootfs
cd rootfs
sudo tar -xvf ../rootfs.tar
cd ..

# 9. make image
sudo python3 ./make_sdimage_p3.py \
-f \
-P preloader-mkpimage.bin,u-boot.img,num=3,format=raw,size=10M,type=A2 \
-P rootfs/*,num=2,format=ext3,size=1200M \
-P zImage,u-boot.scr,DE1_SoC.rbf,DE1_SoC.dtb,num=1,format=vfat,size=300M \
-s 1700M \
-n de1soc-sd-card.img
