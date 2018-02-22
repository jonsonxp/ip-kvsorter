#ip-mergesorter
ip-mergesorter is an hardware merge sorter IP. It use Vivado_HLS for the high-level synthesis. It can generate verilog codes of a merge sorter tree of any given size. 

This project is a demo project of [hCODE](https://github.com/hCODE-FPGA/hCODE), which is an repository platform open-sourced hardware.

## Installation
[Standalone]

git clone https://github.com/jonsonxp/ip-mergesorter.git

[hCODE]

hcode ip get ip-mergesorter

## Usage
[standalone]

1) ./configuration.sh board clock_period sorter_size [fifo_depth]

  - board        : vc707|zybo
  - clock_period : peroid in ms
  - sorter_size  : the input size of a merge tree
  - fifo_depth   : the fifo_depth of the input channel

2) ./make.sh --no-driver

Then synthesized verilog codes can be found in output folder.

[hCODE]

1) Setup environment parameters:

  - $HOME  : Your home folder, like /home/yourname
  - $JAVA_HOME   : Your JDK folder, like /usr/lib/jvm/java-8-openjdk-amd64

2) hcode ip get ip-mergesorter

Please note the driver/rt_mergetree/java/util/make.sh copy the generated driver library libjava_util_Arrays.so into /usr/lib/jvm/java-8-openjdk-amd64/jre/lib/amd64 by default. You can change the JDK folder if you are using other versions.
 

The hCODE can automaticlly synthesis the IP according to your hardware environment.

## License
This project is licenced under the MIT license.
