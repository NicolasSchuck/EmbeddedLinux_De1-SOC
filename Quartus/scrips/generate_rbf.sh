#!/bin/bash
echo "--generate .rbf--"
cd ~/intelFPGA/20.1/embedded
./embedded_command_shell.sh 
cd ~/Documents/EmbeddedLinux_SoC/Quartus/output_files
quartus_cpf -c -o bitstream_compression=on EmbeddedLinux_SoC.sof EmbeddedLinux_SoC.rbf
echo "--done--"
