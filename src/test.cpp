#include "main.hpp"
#include <stdio.h>
#include <stdint.h>
#define N 128

using namespace hls;
using namespace std;

int main(){
	stream<MYINT> in;
	stream<MYINT> out;
	MYINT data[N];
	MYINT i;
	MYINT v;
	int numO=0;
	int pass=0;

	//send sortlength to IP
	in.write(1);
	ip_kvsorter(in, out);//pragma_ipcall


	//init data
	for(i = 0 ; i < N ; i++){
		data[i] = N-i;
	}
	i=0;

	//send data to IP, and get results from IP
	while(numO < N)
	{
		if(i<N){
			in.write(data[i]);
			i++;
		}
	ip_kvsorter(in, out);//pragma_ipcall
		if(!out.empty()){
			out.read(v);
			std::cout << v << std::endl;
			numO++;
		}
	}

	//send reset signal to IP
	in.write(0xFFFFFFFF);
	ip_kvsorter(in, out);//pragma_ipcall
}
