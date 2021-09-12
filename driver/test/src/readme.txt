--------------------------------------------------------------------------------
README - blinker_devmem_test
--------------------------------------------------------------------------------
Author: Nicoals Schuck
Date: 11.09.21

This is a driver test software for an embedded Linux system on a DE1-SoC 
Development board.
This file describes the process to setup your development environment.

--------------------------------------------------------------------------------
SoC EDS
--------------------------------------------------------------------------------
Start embedded command shell and create your hps_0.h header-file.

cd ~/intelFPGA/18.1/embedded
./embedded_command_shell.sh 
PATH=/home/nicolas/intelFPGA_lite/18.1/quartus/sopc_builder/bin:$PATH

cd ~/Documents/EmbeddedLinux_De1-SOC/DE1-SoC/
sopc-create-header-files --single hps_0.h --module hps_0

copy hps_0.h to lib directory of this project
copy <altera>/<version>/embedded/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av/socal