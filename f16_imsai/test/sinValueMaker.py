import math

inc = 18
deg = -90.0
prev = math.sin(math.radians(deg))
results = {deg: prev}
#offset = 0 - prev
offset = 0
deg += inc

while deg <= 270:
	results[deg] = math.sin(math.radians(deg))
	deg += inc
	
for k in sorted(results.iterkeys()):
	print str(k) + ' : ' + str(results[k])