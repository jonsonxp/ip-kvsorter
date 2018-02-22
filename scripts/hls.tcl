############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 2015 Xilinx Inc. All rights reserved.
############################################################
set ip_name ip_kvsorter_a274271d12
open_project $ip_name
set_top $ip_name
add_files ./src/main.cpp
add_files ./src/main.hpp
add_files -tb ./src/test.cpp
open_solution "solution1"
set_part {xc7vx485tffg1761-2}
create_clock -period 6.667 -name default
set_clock_uncertainty 5%
csynth_design
export_design -format syn_dcp
quit
