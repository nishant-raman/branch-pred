import re, os, glob

path = "output"
hash_loc = 0
hash_shift = 1

csv = open("re.csv", "a")

for filename in glob.glob(os.path.join(path, "*.txt")):
	with open(filename, "r") as f:
		for line in f:
			s = re.search(r"sim\s(.*)\straces", line)
			if s:
				cmd = (s.group(1)).split()
			s = re.search(r"traces\/(.*)\.txt", line)
			if s:
				trace = s.group(1)
				continue
			s = re.search(r"misprediction rate:\s+(\d+\.\d+)%", line)
			if s:
				mpr = s.group(1)
				break

		if len(cmd) == 2:
			data = str(cmd[0])+","+str(trace)+",,"+str(cmd[1])+",,,"+str(hash_shift)+","+str(hash_loc)+","+str(mpr)+"\n"
		elif len(cmd) == 3:
			data = str(cmd[0])+","+str(trace)+","+str(cmd[1])+",,"+str(cmd[2])+",,"+str(hash_shift)+","+str(hash_loc)+","+str(mpr)+"\n"
		else:
			data = str(cmd[0])+","+str(trace)+","+str(cmd[2])+","+str(cmd[3])+","+str(cmd[4])+","+str(cmd[1])+","+str(hash_shift)+","+str(hash_loc)+","+str(mpr)+"\n"
		csv.write(data)

csv.close()
			
