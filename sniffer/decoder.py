##################################################
# second part of the backscatter network sniffer
# This part decodes the trace collected by GNURadio
#
# @Author: Amjad Yousef Amjid
# @data  : 3/July/2018
##################################################

import matplotlib.pyplot as plt
import scipy as sp
import numpy as np
from itertools import groupby

# for printing large arrays on the console 
np.set_printoptions(threshold=np.nan)

##################################################
#			   Variable definitions
##################################################
sig_start =  0e6
sig_len	  =  10e6
backscatter_len =  100
backatter_1_freq = 10e3
backatter_0_freq = backatter_1_freq * 2
lpf_decimation = 4
bit_len_jitter = 1  # +1 signal sample
samples_rate = 200e3
samples_per_1 = samples_rate//backatter_1_freq//lpf_decimation
samples_per_0 = samples_rate//backatter_0_freq//lpf_decimation
frame_num_bytes =  11
byte_len = 8
sample_per_frame = frame_num_bytes * byte_len * (samples_per_1 - bit_len_jitter)

bin_sig_amp = 0.56

##################################################
#			  Helper functions
##################################################

# def moving_average(a, n=3):
# 	ret = np.cumsum(a, dtype=float)
# 	ret[n:] = ret[n:] - ret[:-n]
# 	return ret[n-1:] / n

def moving_average(a, n=3):
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    r = ret[n-1:] / n
    r = np.append(np.array(r), np.mean(np.array(a[-n:])) * np.ones(n-1) )
    return r

def signal_slicer(signal, start=0, end=1e6):
	assert len(signal) > 1
	return signal[start: end]

def signal_transitions(signal):
	# a transition happens in between two bits. I chose
	# to shift it to the next bit by adding a 0 at the
	# bigenning 
	arr =  np.abs(signal[1:] - signal[:-1]) 
	return [0] + arr

def raw_bits_len(signal):
	return signal_transitions(signal)

def bin_formater(arr):
    return [x[1] for x in arr]

def sig_average(signal, backscatter_len):
	# if the signal is shorter than approximately 1.5 of 
	# the backatter signal length use normal mean 
	if len(signal) <= backscatter_len*1.5:
		print("normal mean")
		sig_len = len(signal)
		backscatter_len = sig_len//2
		signal_average =  np.mean(signal) * np.ones(sig_len)
		signal_average_shifted = signal_average.copy()
	else:
	# if the signal is longer than 1.5 of the backscattered
	# signal then use moving average
		print("moving average")
		signal_average = moving_average(signal, backscatter_len)
		signal = signal[:len(signal_average)]
		signal_average_shifted = signal_average
		signal_average_shifted = np.roll(signal_average, backscatter_len//2)

	return signal_average_shifted

def file_write(file, frames):
	if len(frames) > frame_num_bytes * byte_len:
		sig_str = "".join(str(e) for e in frames)
		sig_str = sig_str + "\n"
		file.write(sig_str)

##################################################
#			 Main code
##################################################

# print("samples_per_0, samples_per_1", samples_per_0, samples_per_1)

# reading from USRP file
signal = sp.fromfile(open("data.dat"), dtype=sp.float32)
print("length of signal from USRP",len(signal))

# slice the signal
signal = signal_slicer(signal, sig_start, sig_start+sig_len )
print("length of signal         ", signal.size)

signal_average = sig_average(signal, backscatter_len)
print("length of signal_average ", signal_average.size)

# exit()

# Produce a binary signal ( of 1's and 0's )
# This should not be confused with logical 1's and 0's
# this binary signal does not directly represent the data
signal_binary = np.zeros(len(signal))
signal_binary[np.greater(signal, signal_average) ] = bin_sig_amp
signal_binary[np.less(signal, signal_average) ] = -bin_sig_amp
# print("binary representation of the signal", signal_binary)
sig_len = len(signal_binary)
print("binary signal length: ", sig_len )

##################################################
#			Decoding first stage
#  produce the binary represention of the signal
##################################################

frames=[]
bit = 0
cntr = 0
potential_0 = False
f = open("data.txt", "w")
frame_delim =  False

for sample in range(1,sig_len):
		frame_delim =  False
		if( (signal_binary[sample] - signal_binary[sample-1]) == 0): # not edge
			cntr+=1
		else:
			if (cntr >= (samples_per_1 - bit_len_jitter)) and (cntr <= (samples_per_1 + bit_len_jitter)) :  # 1
				frames.append(1)
				potential_0 = False
			elif cntr >= (samples_per_0 - bit_len_jitter) and cntr <= (samples_per_0 + bit_len_jitter):  # 0
				if potential_0:
					frames.append(0)
					potential_0 = False
				else:
					potential_0 = True
			else:	# frame terminator (invalid bit)
				frame_delim =  True
				# print("Frame delimitor")	 
				file_write(f, frames)

				potential_0 = False
				frames = []
				sig_str = ""
			# print(cntr)
			cntr = 0

# if the signal finished on a valid bit, 
# save what is in the frames buffer 
if 	not frame_delim:
	file_write(f, frames)


##################################################
#			Decoding second stage
#  match against the delimeter and convert to hex
##################################################

f = open("data.txt","r")
msg = open("msgs.txt","w")

# lineLen = (1+1+10) * 8
cntr=0
for l in f.readlines():
    # if len(l) > lineLen:
    s = str(l) # [:lineLen])
    hexS = hex(int(s,2))

    if  "bbbbbbaa" in hexS:
        cntr+=1
        print( cntr,"+ ", hexS )
        msg.write(hexS+"\n")
    else:
    	pass
    	# print("-", hexS)
    	# print()



# plt.plot(signal[0:len(signal):4], '-')
# plt.plot(signal_average[0:len(signal_average):4])
plt.plot(signal)
plt.plot(signal_average)
# plt.plot(signal_binary)

plt.show()
