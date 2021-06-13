# out of script 

fatload mmc 0:1 $fpgadata soc_system.rbf;  	#-> name.rbf
fpga load 0 $fpgadata $filesize;
setenv fdtimage soc_system.dtb;		#-> u-boot.dtb
run bridge_enable_handoff;
run mmcload;
run mmcboot;


#before that

./software/bootloader/u-boot-socfpga/tools/mkimage -A arm -O linux -T script -C none -a 0 -e 0 -n "My script" -d boot.script u-boot.scr


