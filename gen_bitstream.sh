cd vivado_prj/
vivado -mode tcl -source prj.tcl
cd vivado_prj/vivado_prj.runs/design_1_processing_system7_0_0_synth_1/
./runme.sh
cd ../synth_1/
./runme.sh
cd ../impl_1
./runme.sh
