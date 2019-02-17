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

**Fix bug in devicetree generated files**

It seems that Xilinx devicetree generator has a bug, it happens where there are multiple clock-names in a cell.

In file pl.dtsi watch for clock-names

.. code-block:: bash

      incorrect
			clock-names = "s_axi_lite_aclk", "m_axi_mm2s_aclk", "m_axis_mm2s_aclk";

      correct
			clock-names = "s_axi_lite_aclk m_axi_mm2s_aclk m_axis_mm2s_aclk";

**Add usb-otg support in devicetree**

In zynq-7000.dtsi replace usb0 entry without

.. code-block:: bash

		usb0: usb@e0002000 {
			compatible = "xlnx,zynq-usb-2.20a", "chipidea,usb2";
			status = "okay";
			clocks = <&clkc 28>;
			interrupt-parent = <&intc>;
			interrupts = <0 21 4>;
			reg = <0xe0002000 0x1000>;
			phy_type = "ulpi";
			usb-reset = <0x7 0x7 0x0>;
			dr_mode = "host";
			usb-phy = <0x6>;
		};

In system-top.dts add phy0 after memory{}

.. code-block:: bash

	phy0 {
		compatible = "ulpi-phy";
		#phy-cells = <0x0>;
		reg = <0xe0002000 0x1000>;
		view-port = <0x170>;
		drv-vbus;
		linux,phandle = <0x6>;
		phandle = <0x6>;
	};
