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
