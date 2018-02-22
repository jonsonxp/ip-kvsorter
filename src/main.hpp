#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <hls_stream.h>
#include "ap_int.h"

using namespace hls;

#define MYINT ap_int<128>

void ip_kvsorter(
		stream<MYINT> &in,
		stream<MYINT> &out);
