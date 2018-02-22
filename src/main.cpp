#include "main.hpp"
using namespace hls;

void merge1(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 1;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void merge2(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 1;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void merge3(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 1;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void merge4(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 1;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void merge5(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 2;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void merge6(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 2;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void merge7(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = 4;
static ap_int<9> cnt0;
static ap_int<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_int<32> v0;
static ap_int<32> v1;
static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, SORT_FLUSH_READ_CH0, SORT_FLUSH_READ_CH1, SORT_FLUSH_CH1, SORT_FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0 = (value0 & 0x01);
				cnt1 = (value1 & 0x01);
				v0 = value0.range(63,32);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_COMPARE:{
			if(v0 < v1){
				if(out.write_nb(value0)){
					if(cnt0 < END_CNT){
						if(!channel0.empty()){
							channel0.read(value0);
							cnt0 += (value0 & 0x01);
							v0 = value0.range(63,32);
						}else{
							mState = SORT_READ_CH0;
						}
					}
					else
						mState = SORT_FLUSH_CH1;
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1 < END_CNT){
						if(!channel1.empty()){
							channel1.read(value1);
							cnt1 += (value1 & 0x01);
							v1 = value1.range(63,32);
						}else{
							mState = SORT_READ_CH1;
						}
					}
					else
						mState = SORT_FLUSH_CH0;
				}
			}
			break;
		}
		case SORT_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_COMPARE;
				}
				break;
			}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < END_CNT){
					if(!channel0.empty()){
						channel0.read(value0);
						cnt0 += (value0 & 0x01);
						v0 = value0.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH0;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < END_CNT){
					if(!channel1.empty()){
						channel1.read(value1);
						cnt1 += (value1 & 0x01);
						v1 = value1.range(63,32);
					}else{
						mState = SORT_FLUSH_READ_CH1;
					}
				}
				else{
					mState = SORT_INIT;
				}
			}
			break;
		}
		case SORT_FLUSH_READ_CH0:{
				if(channel0.read_nb(value0)){
					cnt0 += (value0 & 0x01);
					v0 = value0.range(63,32);
					mState = SORT_FLUSH_CH0;
				}
				break;
			}
		case SORT_FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				cnt1 += (value1 & 0x01);
				v1 = value1.range(63,32);
				mState = SORT_FLUSH_CH1;
			}
			break;
		}
	}
}
void split(stream<MYINT> &in,
    stream<MYINT> &channel0,
    stream<MYINT> &channel1,
    stream<MYINT> &channel2,
    stream<MYINT> &channel3,
    stream<MYINT> &channel4,
    stream<MYINT> &channel5,
    stream<MYINT> &channel6,
    stream<MYINT> &channel7 
    ){
    #pragma HLS PIPELINE II=1 enable_flush
    static MYINT value;
    if(!in.empty())
        if(in.read_nb(value)) {
            switch(value.range(15,8)) {
                case 0 : {
                    channel0.write_nb(value);
                    break;
                }
                case 1 : {
                    channel1.write_nb(value);
                    break;
                }
                case 2 : {
                    channel2.write_nb(value);
                    break;
                }
                case 3 : {
                    channel3.write_nb(value);
                    break;
                }
                case 4 : {
                    channel4.write_nb(value);
                    break;
                }
                case 5 : {
                    channel5.write_nb(value);
                    break;
                }
                case 6 : {
                    channel6.write_nb(value);
                    break;
                }
                case 7 : {
                    channel7.write_nb(value);
                    break;
                }
            }
         }
}
void ip_kvsorter(
		stream<MYINT> &in,
		stream<MYINT> &out
	){
	#pragma HLS DATAFLOW
	#pragma HLS LATENCY max=0
	#pragma HLS INTERFACE ap_fifo port=in
	#pragma HLS INTERFACE ap_fifo port=out
	#pragma AP interface ap_ctrl_none port=return
	static MYINT value;
	static stream<MYINT> channel0;
	static stream<MYINT> channel1;
	static stream<MYINT> channel2;
	static stream<MYINT> channel3;
	static stream<MYINT> channel4;
	static stream<MYINT> channel5;
	static stream<MYINT> channel6;
	static stream<MYINT> channel7;
	static stream<MYINT> channel8;
	static stream<MYINT> channel9;
	static stream<MYINT> channel10;
	static stream<MYINT> channel11;
	static stream<MYINT> channel12;
	static stream<MYINT> channel13;
	#pragma HLS STREAM variable=channel0 depth=500 dim=1
	#pragma HLS STREAM variable=channel1 depth=500 dim=1
	#pragma HLS STREAM variable=channel2 depth=500 dim=1
	#pragma HLS STREAM variable=channel3 depth=500 dim=1
	#pragma HLS STREAM variable=channel4 depth=500 dim=1
	#pragma HLS STREAM variable=channel5 depth=500 dim=1
	#pragma HLS STREAM variable=channel6 depth=500 dim=1
	#pragma HLS STREAM variable=channel7 depth=500 dim=1
	#pragma HLS STREAM variable=channel8 depth=2 dim=1
	#pragma HLS STREAM variable=channel9 depth=2 dim=1
	#pragma HLS STREAM variable=channel10 depth=2 dim=1
	#pragma HLS STREAM variable=channel11 depth=2 dim=1
	#pragma HLS STREAM variable=channel12 depth=2 dim=1
	#pragma HLS STREAM variable=channel13 depth=2 dim=1
    split(in,channel0,channel1,channel2,channel3,channel4,channel5,channel6,channel7);
	merge1(channel0, channel1, channel8);
	merge2(channel2, channel3, channel9);
	merge3(channel4, channel5, channel10);
	merge4(channel6, channel7, channel11);
	merge5(channel8, channel9, channel12);
	merge6(channel10, channel11, channel13);
	merge7(channel12, channel13, out);
}
