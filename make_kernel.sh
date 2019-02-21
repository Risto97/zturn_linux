#!/bin/sh
corenum=$(grep -c ^processor /proc/cpuinfo)
echo "Running make with: " $corenum " threads"

sdk_dir=/tools/work/vivado/projects/cascade_classifier_pygears_linux/cascade_classifier_pygears_linux.sdk
bitstream_dir=$sdk_dir/hdmi_out_wrapper_hw_platform_0

cp -r linux_zturn/* linux-xlnx/
cp $sdk_dir/device_tree_bsp_0/pcw.dtsi linux-xlnx/arch/arm/boot/dts/
cp $sdk_dir/device_tree_bsp_0/zynq-7000.dtsi linux-xlnx/arch/arm/boot/dts/
cp $sdk_dir/device_tree_bsp_0/pl.dtsi linux-xlnx/arch/arm/boot/dts/
cp $sdk_dir/device_tree_bsp_0/system-top.dts linux-xlnx/arch/arm/boot/dts/zynq-zturn.dts
cd linux-xlnx/
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- zynq_z_turn_sbf_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- uImage LOADADDR=0x00008000 -j$corenum
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- dtbs
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- modules
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- modules_install INSTALL_MOD_PATH=../modules
cp arch/arm/boot/uImage ../BOOT
cp arch/arm/boot/dts/zynq-zturn.dtb ../BOOT/devicetree.dtb
