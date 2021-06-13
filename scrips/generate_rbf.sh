#!/bin/bash
echo "--generate .rbf--"
cd ~/intelFPGA/20.1/embedded
./embedded_command_shell.sh 
cd ~/Documents/EmbeddedLinux_De1-SOC/Quartus/output_files
quartus_cpf -c -o bitstream_compression=on De1-SOC.sof De1-SOC.rbf
echo "--done--"


echo "generate dts"
export PATH=$PATH:~/intelFPGA/20.1/embedded/
./embedded_command_shell.sh
sopc2dts --input soc_system.sopcinfo\
--output De1-SOC.dtb --type dtb\
--board soc_system_board_info.xml\
--board hps_common_board_info.xml\
--clocks
