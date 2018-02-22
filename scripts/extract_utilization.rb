#Ruby script to extract resource utilization information
#from a Vivado "report_utilization -hierarchical" report.
#And then write into a hcode.log file.
require 'json'

file_util = ARGV[1]
file_hcode = ARGV[0]

if !File.exist?(file_util)
  puts "File #{file_util} does not exist."
  exit
end
if !File.exist?(file_hcode)
  puts "File #{file_hcode} does not exist."
  exit
end

design_name = ""
resource = Hash.new
util = File.read(file_util)
util.each_line{|l|
	l = l.gsub("\t","").gsub(" ","").strip
	if(l.include?"|Design:")
		design_name = l.split(':')[1]
	end
	if((design_name != "") && (l.include?"|#{design_name}|"))
		items = l.split("|")
		resource["LUT"] = items[4].to_i
		resource["LUTRAM"] = items[5].to_i
		resource["FF"] = items[7].to_i
		resource["BRAM"] = (items[8].to_f + (items[9].to_f / 2) + 0.5).to_i
	end
}

tmp = File.read(file_hcode)
hcode = JSON.parse(tmp)
hcode["resource"] = resource
File.open(file_hcode, 'w') do |file|
	json_str = JSON.pretty_generate(hcode)
        str = file.write(json_str)
end

