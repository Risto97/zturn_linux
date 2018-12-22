#!/bin/sh
cp -r linux_zturn/* linux-xlnx/
cp vivado_prj/vivado_prj/vivado_prj.sdk/device_tree_bsp_0/pcw.dtsi linux-xlnx/arch/arm/boot/dts/
cp vivado_prj/vivado_prj/vivado_prj.sdk/device_tree_bsp_0/zynq-7000.dtsi linux-xlnx/arch/arm/boot/dts/
cp vivado_prj/vivado_prj/vivado_prj.sdk/device_tree_bsp_0/pl.dtsi linux-xlnx/arch/arm/boot/dts/
cp vivado_prj/vivado_prj/vivado_prj.sdk/device_tree_bsp_0/system-top.dts linux-xlnx/arch/arm/boot/dts/zynq-zturn.dts
cd linux-xlnx/
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- zynq_z_turn_sbf_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- uImage LOADADDR=0x00008000 -j12
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- dtbs
cp arch/arm/boot/uImage ../BOOT
cp arch/arm/boot/dts/zynq-zturn.dtb ../BOOT/devicetree.dtb
