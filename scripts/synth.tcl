set top_module ip_kvsorter_a274271d12
set device_type xc7vx485tffg1761-2

read_verilog [glob ./output/verilog/*.v]
synth_design -top $top_module -part $device_type -mode out_of_context
report_utilization -hierarchical -file utilization.rpt
report_timing -file timing.rpt
write_checkpoint ./output/dcp/$top_module.dcp -force

