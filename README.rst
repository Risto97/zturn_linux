=====================================================
Linux on Z-Turn board with free HDMI video controller
=====================================================

MYIR released video core for HDMI without time limitation.
This guide will provide explanations on how to compile U-Boot, Linux Kernel and add your own peripheral to your Linux design.

**Clone git repos**

.. code-block:: bash

   ./clone_gits.sh

**Add devicetree_generator to Xilinx SDK**

In SDK:
SDK Menu: Xilinx > Repositories > New... (< devicetree_git path>) > OK

**Generate devicetree using Xilinx SDK**

In SDK:
SDK Menu: File > New > Board Support Package > Under Board Support Package OS, select device_tree > finish

**Modify devicetree files for framebuffer**

In file system-top.dtsi

in section chosen{} after stdout-path = "serial0:115200n8"; append

.. code-block:: bash


   #address-cells = <0x1>;
   #size-cells = <0x1>;
   ranges;
   framebuffer0: framebuffer@3F000000 {
                compatible = "simple-framebuffer";
                reg = <0x3F000000 (1920*1080*4)>;
                width = <1920>;
                height = <1080>;
                stride = <(1920*4)>;
                format = "a8r8g8b8";
                status = "okay";
   };

In section memory{}, change from reg = <0x0 0x40000000>; to reg = <0x0 0x3F000000>;
