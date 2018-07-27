from collections import Counter

f = open("msgs.txt","r")
msgs=[]
for msg in f.readlines():
	msgs.append(msg[-5:-1])

cntrs = Counter(msgs)
freqs = Counter(cntrs.values())

for cntr in cntrs:
	print(cntr, cntrs[cntr])
print(freqs, "sum:", sum(cntrs.values()))
