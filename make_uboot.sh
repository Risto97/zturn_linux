corenum=$(grep -c ^processor /proc/cpuinfo)
echo "Running make with: " $corenum " threads"

cp vivado_prj/vivado_prj/vivado_prj.sdk/design_1_wrapper_hw_platform_0/ps7_init_gpl.c u-boot_zturn/board/xilinx/zynq/zynq_zturn/
cp -r u-boot_zturn/* u-boot-xlnx/

rm u-boot.elf
rm BOOT.bin
cd u-boot-xlnx/
echo "Clean U-Boot repo"
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- distclean
echo "Config U-Boot for zturn"
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- zynq_z_turn_config
echo "Compiling U-Boot"
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -j$corenum
mv u-boot u-boot.elf
cd ..

bootgen -image fsblk.bif -o i BOOT/BOOT.bin
