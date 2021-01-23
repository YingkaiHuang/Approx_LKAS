import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib.image as mpimg
import matplotlib.gridspec as gridspec
import time
import cv2
from collections import deque
import numpy as np
import argparse

# parser = argparse.ArgumentParser(description='Live Plot Graphs.')
# parser.add_argument('-p', "--pipe_ver",
#                    help='pipeline version')
				   
# parser.add_argument('-v', "--velocity",
#                    help='velocity')
				   
# parser.add_argument('-m', "--mode",
#                    help='soc config')
				 
# parser.add_argument('-s', "--scene",
#                   help='scene')
				  
# parser.add_argument('--wt', help='Plot',
#     action='store_true')

# parser.add_argument('--comp', help='Plot',
#     action='store_true')
				   
# args = parser.parse_args()

fig = plt.figure(dpi = 100, figsize=(12, 8))
gs = gridspec.GridSpec(ncols=4, nrows=6)
ax1 = fig.add_subplot(gs[0:4, 0:2])
ax2 = fig.add_subplot(gs[0:3, 2:4])
ax3 = fig.add_subplot(gs[3:6, 2:4])
#ax4 = fig.add_subplot(gs[4:6, 0:2])


left  = 0.125  # the left side of the subplots of the figure
right = 0.9    # the right side of the subplots of the figure
bottom = 0.1   # the bottom of the subplots of the figure
top = 0.9      # the top of the subplots of the figure
wspace = 0.6   # the amount of width reserved for blank space between subplots
hspace = 0.8   # the amount of height reserved for white space between subplots
fig.subplots_adjust(left=None, bottom=None, right=None, top=top, wspace=wspace, hspace=hspace)

# if (int(args.pipe_ver) != 0):
# 	if (not args.comp):
# 		pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe0_m"+args.mode+"_v"+args.velocity+"kmph.csv","r").read()
# 	if (args.comp):
# 		#pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe0_m"+args.mode+"_v"+args.velocity+"kmph_comp90.csv","r").read()
# 		pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe0_m"+args.mode+"_v"+args.velocity+"kmph_comp70.csv","r").read()
# 	dataArray = pullData.split('\n')
# 	xaref1 = []
# 	yaref1 = []
# 	saref1 = []
# 	for eachLine in dataArray:
# 		if len(eachLine)>1:
# 			x,y,m = eachLine.split(',')
# 			xaref1.append(float(x))
# 			yaref1.append(float(y)*100)
# 			saref1.append(float(m))

def animate(i):
	# subplot 1
	# if (args.wt):
	# 	if (not args.comp):
	# 		pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_wt_m"+args.mode+"_v"+args.velocity+"kmph.csv","r").read()
	# 	if (args.comp):
	# 		#pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_wt_m"+args.mode+"_v"+args.velocity+"kmph_comp90.csv","r").read()
	# 		pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_wt_m"+args.mode+"_v"+args.velocity+"kmph_comp70.csv","r").read()
	# else:
	# 	if (not args.comp):
	# 		pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_m"+args.mode+"_v"+args.velocity+"kmph.csv","r").read()
	# 	if (args.comp):
	# 		#pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_m"+args.mode+"_v"+args.velocity+"kmph_comp100.csv","r").read()
	# 		#pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_m"+args.mode+"_v"+args.velocity+"kmph_comp90.csv","r").read()
	# 		pullData = open("/home/ocps/Approx_IBC/hil/server/results_qoc/"+args.scene+"/yL_pipe"+args.pipe_ver+"_m"+args.mode+"_v"+args.velocity+"kmph_comp70.csv","r").read()
	#dataArray = pullData.split('\n')
	#xar = []
	#yar = []
	#sar = []
	
	# subplot 1
	img = cv2.imread("/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/img_thresholding.pnget_bev_pointsg.png")
	if img is not None:
		ax1.clear()
		#ax3.set_xlabel('time (sec)', fontsize=15)
		ax1.set_title("Sliding Window Based Lane Tracking", fontsize=18)
		#ax3.set_ylabel('lateral deviation (cm)', fontsize=15)
		ax1.grid(alpha=0.3, linestyle='-.', zorder=-3)
		#ax[i].legend(["VER:{}".format(0), "VER:{}".format(i)], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
		#ax1.imshow(img)
		ax1.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
	else:
		print("img_binary is None")
	
	# subplot 2
	img = cv2.imread("/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/img_output.png")
	if img is not None:
		ax2.clear()
		#ax4.set_xlabel('time (sec)', fontsize=15)
		#ax4.set_ylabel('lateral deviation (cm)', fontsize=15)
		ax2.grid(alpha=0.3, linestyle='-.', zorder=-3)
		ax2.set_title("Final Output Image", fontsize=18)
		#ax[i].legend(["VER:{}".format(0), "VER:{}".format(i)], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
		#ax2.imshow(img)
		ax2.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
	else:
		print("img_output is None")
		# subplot 3
	img = cv2.imread("/home/yingkai/yingkai-huang3/Approx_IBC/final_app/cpp/cpp_webots_api/out_imgs/img_rev_warped.png")
	if img is not None:
		ax3.clear()
		#ax3.set_xlabel('time (sec)', fontsize=15)
		ax3.set_title("bev", fontsize=18)
		#ax3.set_ylabel('lateral deviation (cm)', fontsize=15)
		ax3.grid(alpha=0.3, linestyle='-.', zorder=-3)
		#ax[i].legend(["VER:{}".format(0), "VER:{}".format(i)], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
		#ax1.imshow(img)
		ax3.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
	else:
		print("img_binary is None")
	# subplot 3	
	# for eachLine in dataArray:
	# 	if len(eachLine)>1:
	# 		x,y,m = eachLine.split(',')
	# 		xar.append(float(x))
	# 		yar.append(float(y)*100)
	# 		sar.append(float(m))
	# ax3.clear()
	# ax3.set_xlabel('time (sec)', fontsize=15)
	# ax3.set_ylabel('lateral deviation (cm)', fontsize=15)
	# ax3.grid(alpha=0.3, linestyle='-.', zorder=-3)
	# ax3.plot(xar, yar, marker='.', markersize=4, markerfacecolor='red', markeredgecolor='red', alpha=0.6, color='red', zorder = 1, linestyle='-', linewidth = 0.5)
	# #ax3.plot(xar,sar, marker='', markersize=4, markerfacecolor='k', alpha=1, color='red', zorder = 1, linestyle='')
	# if (int(args.pipe_ver) != 0):
	# 	ax3.plot(xaref1,yaref1, marker='.',  markersize=4, alpha=0.6, zorder=-1, linestyle='-', linewidth = 0.5)
	# 	#ax3.plot(xaref1,saref1, marker='',  markersize=4, markerfacecolor='k', alpha=0.8, zorder=-1, linestyle='')
	# 	#ax3.legend([rearange[int(args.pipe_ver)]+" ("+args.mode+")", "VER:0 "+"("+args.mode+")"], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
	# 	ax3.legend(["VER:" +args.pipe_ver+" ("+args.mode+")", "VER:0 "+"("+args.mode+")"], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
	# else:
	# 	ax3.legend(["VER:" +args.pipe_ver+" ("+args.mode+")"], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
	# #ax3.plot(xaref2,yaref2, marker='', alpha=0.8, color="green", zorder=-2)
	# #ax3.plot(xaref3,yaref3, marker='', alpha=0.5, color="black", zorder=-3)
	# ax3.set_ylim([-0.4,0.9]) ## CPU_ALL_GPU
	# #ax3.set_ylim([-2,5]) ## CPU_ALL_GPU
	# #ax3.set_ylim([-2,2]) ## CPU_ALL
	# #ax3.set_ylim([-4,4]) ## CPU_4CORE
	# #ax3.set_xlim([6,10])
	
	# # subplot 4
	# ax4.clear()
	# ax4.set_xlabel('time (sec)', fontsize=15)
	# ax4.set_ylabel('steering angles', fontsize=15)
	# ax4.grid(alpha=0.3, linestyle='-.', zorder=-3)
	# ax4.plot(xar,sar, marker='.', markersize=1, markerfacecolor='k', alpha=1, color='red', zorder = 1, linestyle='')
	# if (int(args.pipe_ver) != 0):
	# 	ax4.legend(["VER:" +args.pipe_ver+" ("+args.mode+")", "VER:0 "+"("+args.mode+")"], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
	# 	ax4.plot(xaref1,saref1, marker='.',  markersize=1, markerfacecolor='k', alpha=0.8, zorder=-1, linestyle='')
	# else:
	# 	#ax4.legend([rearange[int(args.pipe_ver)]+" ("+args.mode+")"], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
	# 	ax4.legend(["VER:" +args.pipe_ver+" ("+args.mode+")"], loc='upper right', frameon=False, prop={'size': 12}, bbox_to_anchor=(1.01, 1.01))
	# ax4.set_ylim([-0.03,0.01])

	
# #LoadFile="/sys/bus/i2c/drivers/ina3221x/1-0040/iio_device/in_power1_input"
# LoadFile="/sys/bus/i2c/drivers/ina3221x/1-0041/iio_device/in_power1_input"
# # For the comparison
# gpuLine, = ax4.plot([],[])

# # The line points in x,y list form
# gpuy_list = deque([0]*240)
# gpux_list = deque(np.linspace(60,0,num=240))

# fill_lines=0

# def initGraph():
    # global ax4
    # global gpuLine
    # global fill_lines


    # ax4.set_xlim(60, 0)
    # #ax4.set_ylim(-5, 105)
    # ax4.set_title('Instantaneous Power', fontsize=15)
    # ax4.set_ylabel('Power (mW)', fontsize=15)
    # ax4.set_xlabel('Seconds', fontsize=15);
    # ax4.grid(color='gray', linestyle='dotted', linewidth=1)

    # gpuLine.set_data([],[])
    # fill_lines=ax4.fill_between(gpuLine.get_xdata(),50,0)

    # return [gpuLine] + [fill_lines]

# def updateGraph(frame):
    # global fill_lines
    # global gpuy_list
    # global gpux_list
    # global gpuLine
    # global ax4

 
    # # Now draw the GPU usage
    # gpuy_list.popleft()
    # with open(LoadFile, 'r') as gpuFile:
      # fileData = gpuFile.read()
    # # The GPU load is stored as a percentage * 10, e.g 256 = 25.6%
    # gpuy_list.append(int(fileData))
    # gpuLine.set_data(gpux_list,gpuy_list)
    # fill_lines.remove()
    # fill_lines=ax4.fill_between(gpux_list,0,gpuy_list, facecolor='cyan', alpha=0.50)

    # return [gpuLine] + [fill_lines]


ani = animation.FuncAnimation(fig, animate, interval=250)
# Keep a reference to the FuncAnimation, so it does not get garbage collected
# animation = animation.FuncAnimation(fig, updateGraph, frames=200,
                    # init_func=initGraph,  interval=250, blit=True)

plt.show()

