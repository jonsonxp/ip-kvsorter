#!/bin/bash

for i in "$@"
do
case $i in
        -driver)
        DRIVER=1
        shift
        ;;
        *)
        ;;
esac
done

IPNAME=ip_kvsorter_a274271d12

#compile and install the ip driver
if [ "$DRIVER" ]; then
	if [ -z "$JAVA_HOME" ]; then
        	echo "Please setup the JAVA_HOME environment parameter. For example,"
        	echo "export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/"
        	exit
	fi

        cd driver/rt_mergetree/java/util
        sh make.sh
        cd ../../../..
        mkdir ~/.hcode/ip-drivers
        mkdir ~/.hcode/ip-drivers/$IPNAME
        cp -r driver/rt_mergetree ~/.hcode/ip-drivers/$IPNAME
        exit
fi

#cleaning
rm -rf $IPNAME
rm -rf output

#execute the vivado_hls compiling
vivado_hls -f ./scripts/hls.tcl

#copy the dcp and verilog to output folder
mkdir output
mkdir output/verilog
mkdir output/dcp
cp $IPNAME/solution1/impl/verilog/*.v ./output/verilog/

vivado -nolog -nojournal -mode batch -source ./scripts/synth.tcl
ruby ./scripts/extract_utilization.rb ./hcode.log ./utilization.rpt

