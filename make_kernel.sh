#!/bin/sh
cp -r linux_zturn/* linux-xlnx/
cd linux-xlnx/
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- xilinx_zynq_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- uImage LOADADDR=0x00008000 -j12
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- dtbs
cp arch/arm/boot/uImage ../BOOT
cp arch/arm/boot/dts/zynq-zturn.dtb ../BOOT/devicetree.dtb
