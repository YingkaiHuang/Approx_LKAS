import cv2
from turbojpeg import TurboJPEG, TJPF_GRAY, TJSAMP_GRAY, TJFLAG_PROGRESSIVE
import time
import argparse

# available_clocks = [
    # ('monotonic', time.monotonic),
    # ('perf_counter', time.perf_counter),
    # ('process_time', time.process_time),
    # ('time', time.time),
# ]

# for clock_name, func in available_clocks:
    # print(textwrap.dedent('''\
    # {name}:
        # adjustable    : {info.adjustable}
        # implementation: {info.implementation}
        # monotonic     : {info.monotonic}
        # resolution    : {info.resolution}
        # current       : {current}
    # ''').format(
        # name=clock_name,
        # info=time.get_clock_info(clock_name),
        # current=func())
    # )

# specifying library path explicitly
# jpeg = TurboJPEG(r'D:\turbojpeg.dll')
# jpeg = TurboJPEG('/usr/lib64/libturbojpeg.so')
# jpeg = TurboJPEG('/usr/local/lib/libturbojpeg.dylib')

# using default library installation

def parse_options():
	parser = argparse.ArgumentParser(
		description = 'Run compression')

	# # Input/Output options
	parser.add_argument("--time_compress_decompress", action="store_true",
		help="Run in timing analysis mode")

	parser.add_argument("--memprof_compress", action="store_true",
		help="Run in mem prof compress mode")

	parser.add_argument("--memprof_decompress", action="store_true", 
		help="Run in memprof decompress mode")
		
	parser.add_argument("--timeprof_compress", action="store_true",
		help="Run in time prof compress mode")

	parser.add_argument("--timeprof_decompress", action="store_true", 
		help="Run in timeprof decompress mode")
	
	return parser.parse_args()

def do_benchmarking_compress(bgr_array, quality, out_img):
	timeval = []
	jpeg = TurboJPEG()
	for j in range(100):
		for i in range(100):
			out_file = open(out_img, 'wb')
			start = time.process_time()
			out_file.write(jpeg.encode(bgr_array, quality=quality))
			end = time.process_time()
			val = (end - start)*1000 #msec
			timeval.append(val)
			out_file.close()
	return timeval
	
def do_benchmarking_decompress(in_img):
	timeval = []
	jpeg = TurboJPEG()
	for j in range(100):
		for i in range(100):
			in_file = open(in_img, 'rb')
			start = time.process_time()
			bgr_array = jpeg.decode(in_file.read())
			end = time.process_time()
			val = (end - start)*1000 #msec
			timeval.append(val)
			in_file.close()
	return timeval

#@profile (activate only for mem profile)
def do_mem_profile_compress():
	jpeg = TurboJPEG()
	quality = 100
	out_img = 'jpegs/single_v0.jpg'
	in_img = 'imgs/output_fwd_v0.png'
	bgr_array = cv2.imread(in_img)
	out_file = open(out_img, 'wb')
	out_file.write(jpeg.encode(bgr_array, quality=quality))
	out_file.close()
	return None
	
#@profile
def do_mem_profile_decompress():
	jpeg = TurboJPEG()
	out_img = 'jpegs/single_v0.jpg'
	in_file = open(out_img, 'rb')
	bgr_array = jpeg.decode(in_file.read())
	in_file.close()
	return None
	
def do_time_profile_compress():
	jpeg = TurboJPEG()
	quality = 100
	out_img = 'jpegs/single_v0.jpg'
	in_img = 'imgs/output_fwd_v0.png'
	bgr_array = cv2.imread(in_img)
	out_file = open(out_img, 'wb')
	out_file.write(jpeg.encode(bgr_array, quality=quality))
	out_file.close()
	return None

def do_time_profile_decompress():
	jpeg = TurboJPEG()
	out_img = 'jpegs/single_v0.jpg'
	in_file = open(out_img, 'rb')
	bgr_array = jpeg.decode(in_file.read())
	in_file.close()
	return None
	
if __name__ == "__main__":
	args = parse_options()
	if args.time_compress_decompress:
		jpeg = TurboJPEG()
		quality = 100
		out_img = 'jpegs/single_v0.jpg'
		in_img = 'imgs/output_fwd_v0.png'
		bgr_array = cv2.imread(in_img)
		out_file = open(out_img, 'wb')
		out_file.write(jpeg.encode(bgr_array, quality=quality))
		out_file.close()
		####### decompress #########
		print('Decompress:\n')
		timeval = do_benchmarking_decompress(out_img)
		out_csv = 'chrono/runtime_decompress_v0_single_py.csv'
		f = open(out_csv, 'w')
		f.write('v0\n')
		for value in timeval:
			f.write(str(value))
			f.write('\n')
		f.close()
		####### compress #########
		print('Compress:\n')
		timeval = do_benchmarking_compress(bgr_array, quality, out_img)
		out_csv = 'chrono/runtime_compress_v0_single_py.csv'
		f = open(out_csv, 'w')
		f.write('v0\n')
		for value in timeval:
			f.write(str(value))
			f.write('\n')
		f.close()
	
	if args.memprof_compress:
		do_mem_profile_compress()
	
	if args.memprof_decompress:
		do_mem_profile_decompress()
		
	if args.timeprof_compress:
		do_time_profile_compress()
	
	if args.timeprof_decompress:
		do_time_profile_decompress()

