#!/bin/sh
corenum=$(grep -c ^processor /proc/cpuinfo)
echo "Running make with: " $corenum " threads"

# sdk_dir=$(vivado_prj/hdmi_prj/z-turn.sdk)
# bitstream_dir=$sdk_dir/hdmi_out_wrapper_hw_platform_0

cp vivado_prj/hdmi_prj/hdmi_prj.sdk/hdmi_out_wrapper_hw_platform_0/ps7_init_gpl.c u-boot_zturn/board/xilinx/zynq/zynq_zturn/
cp vivado_prj/hdmi_prj/hdmi_prj.sdk/device_tree_bsp_0/pcw.dtsi u-boot-xlnx/arch/arm/dts/
cp vivado_prj/hdmi_prj/hdmi_prj.sdk/device_tree_bsp_0/zynq-7000.dtsi u-boot-xlnx/arch/arm/dts/
cp vivado_prj/hdmi_prj/hdmi_prj.sdk/device_tree_bsp_0/pl.dtsi u-boot-xlnx/arch/arm/dts/
cp vivado_prj/hdmi_prj/hdmi_prj.sdk/device_tree_bsp_0/system-top.dts u-boot-xlnx/arch/arm/dts/zynq-zturn-myir.dts
cp -r u-boot_zturn/* u-boot-xlnx/

rm u-boot-xlnx/u-boot.elf
rm BOOT/BOOT.bin
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
