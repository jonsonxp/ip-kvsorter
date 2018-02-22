@ways = ARGV[0].to_i
@fifosize = ARGV[1].to_i
@fifosize = 500 if @fifosize == 0
@ip_name = ARGV[2]
@ip_name = "ip_kvsorter" if @ip_name == nil

string_merge = <<eos
void merge_NUMBER1(stream<MYINT> &channel0,
		stream<MYINT> &channel1,
		stream<MYINT> &out
		){
#pragma HLS PIPELINE II=1 enable_flush
#pragma HLS LATENCY max=0
const int END_CNT = _NUMBER_CNT;
static ap_uint<9> cnt0;
static ap_uint<9> cnt1;
static MYINT value0;
static MYINT value1;
static ap_uint<32> v0;
static ap_uint<32> v1;
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
eos

puts "#include \"main.hpp\""
puts "using namespace hls;"
puts

operator1 = 1
operator2 = @ways / 2
operator3 = @ways / 2
1.upto((@ways-1)){|i|
	
	string_merge_tmp=string_merge.gsub('_NUMBER_CNT',"#{operator1}")
	string_merge_tmp=string_merge_tmp.gsub('_NUMBER1', "#{i}")
	puts string_merge_tmp
	if(i==operator3)
		operator1*=2
		operator2/=2
		operator3+=operator2
	end
}

string_split_1 = <<eos
void split(stream<MYINT> &in,
eos
puts string_split_1

string_split_2 ="    stream<MYINT> &channel_NUMBER1,\n"

0.upto((@ways-1)) {|i|
    string_split_tmp=string_split_2.gsub('_NUMBER1',"#{i}")
    if i<(@ways-1)
    	puts string_split_tmp
    else
        puts string_split_tmp.gsub(",\n"," ")
    end
}

string_split_3 = <<eos
    ){
    #pragma HLS PIPELINE II=1 enable_flush
    static MYINT value;
    if(!in.empty())
        if(in.read_nb(value)) {
            switch(value.range(15,8)) {
eos
puts string_split_3

string_split_4 = <<eos
                case _NUMBER1 : {
                    channel_NUMBER1.write_nb(value);
                    break;
                }
eos
0.upto((@ways-1)) {|i|
    string_split_tmp=string_split_4.gsub('_NUMBER1',"#{i}")
    puts string_split_tmp
}

string_split_5 = <<eos
            }
         }
}
eos
puts string_split_5

string_ip_mergesorter = <<eos
void #{@ip_name}(
		stream<MYINT> &in,
		stream<MYINT> &out
	){
	#pragma HLS DATAFLOW
	#pragma HLS LATENCY max=0
	#pragma HLS INTERFACE ap_fifo port=in
	#pragma HLS INTERFACE ap_fifo port=out
	#pragma AP interface ap_ctrl_none port=return
	static MYINT value;
eos

puts string_ip_mergesorter

0.upto((2*@ways-3)) { |i|
    puts "	static stream<MYINT> channel#{i};"
}

operator_fifosize = @fifosize


0.upto((2*@ways-3)) { |i|
	if(i==@ways) 
		operator_fifosize=2
	end
	puts "\t#pragma HLS STREAM variable=channel#{i} depth=#{operator_fifosize} dim=1"
	
}

string_split_func_1 = "    split(in,"
string_split_func_2 = "channel_NUMBER1,"
0.upto((@ways-1)) {|i|
    string_split_func_1.concat(string_split_func_2.gsub('_NUMBER1',"#{i}"))
}
string_split_func_1 = string_split_func_1.chop
string_split_func_1.concat(');')
puts string_split_func_1


cnt_ch_0 = 0
cnt_ch_1 = 0
target = @ways
operator = 1
length = ""
1.upto(@ways-1){|i|
	from0 = cnt_ch_0
	cnt_ch_0 += 1
	from1 = cnt_ch_0
	cnt_ch_0 += 1
	to = @ways + cnt_ch_1
	cnt_ch_1 += 1
	
	

	if(to == (@ways-1)*2)
		puts "\tmerge#{i}(channel#{from0}, channel#{from1}, out);"
	else
		puts "\tmerge#{i}(channel#{from0}, channel#{from1}, channel#{to});"
	end
}

puts "}"
