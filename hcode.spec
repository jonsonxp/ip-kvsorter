{
  "name": "ip-kvsorter",
  "type": "ip",
  "version": "0.3.0",
  "summary": "A key-value mergesorter IP.",
  "description": "A merge sorter IP based on hCODE 2.0 platform, which is implemented with Vivado HLS.",
  "homepage": "https://github.com/jonsonxp/ip-kvsorter/",
  "license": "MIT",
  "authors": {
    "jonsonxp": "ofmsmile@msn.com"
  },
  "source": {
    "git": "https://github.com/jonsonxp/ip-kvsorter.git",
    "tag": "0.2.0"
  },
  "interface": {
    "host": {
       "ports":{
         "ap_fifo": {
           "type": "ap_fifo",
           "data_width": 128
         }
       }
    }
  },
  "shell": {
    "shell-vc707-xillybus-ap_fifo128": {
      "property": {
        "max_number": 3,
        "acceleration": 2,
        "throughput": 1000,
        "resource": {
	    "LUT": 20778,
	    "LUTRAM": 3968,
	    "FF": 40414,
	    "BRAM": 128
	}
      },
      "ip_conf": {
        "clk": 150,
        "fifo_depth": 500,
        "size": 64
      },
      "shell_conf": {
	"clk": 150,
        "reference": " $ip_name $instance_name (.ap_clk(ip_clk_$ch_num), .ap_rst(~ip_rst_n_$ch_num), .in_V_V_dout(in_r_dout_$ch_num), .in_V_V_empty_n(in_r_empty_n_$ch_num), .in_V_V_read(in_r_read_$ch_num), .out_V_V_din(out_r_din_$ch_num), .out_V_V_full_n(!out_r_full_$ch_num), .out_V_V_write(out_r_write_$ch_num));"
      }
    },
    "shell-kc705-xillybus-ap_fifo128": {
      "property": {
        "max_number": 3,
        "acceleration": 2,
        "throughput": 1000,
        "resource": {
	    "LUT": 20778,
	    "LUTRAM": 3968,
	    "FF": 40414,
	    "BRAM": 128
	}
      },
      "ip_conf": {
        "clk": 150,
        "fifo_depth": 500,
        "size": 64
      },
      "shell_conf": {
	"clk": 150,
        "reference": " $ip_name $instance_name (.ap_clk(ip_clk_$ch_num), .ap_rst(~ip_rst_n_$ch_num), .in_V_V_dout(in_r_dout_$ch_num), .in_V_V_empty_n(in_r_empty_n_$ch_num), .in_V_V_read(in_r_read_$ch_num), .out_V_V_din(out_r_din_$ch_num), .out_V_V_full_n(!out_r_full_$ch_num), .out_V_V_write(out_r_write_$ch_num));"
      }
    }
  }
}

