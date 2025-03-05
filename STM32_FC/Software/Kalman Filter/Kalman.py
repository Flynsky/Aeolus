import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np

from matplotlib import patheffects

##read in rawdata
rawdata = []
with open('Test Data\mpu6050_millis.txt', 'r') as file:
    # Read the entire content of the file
    for el in file:
        rawdata.append(el.split()[3:])
rawdata = rawdata[7:]

##format data
data = {"x":[],"y":[],"z":[],"p":[],"q":[],"r":[],"t":[],"millis":[]}
try:
    for x,y,z,p,q,r,t,m in rawdata:
        data["x"].append(float(x))
        data["y"].append(float(y))
        data["z"].append(float(z))
        data["q"].append(float(q))
        data["p"].append(float(p))
        data["r"].append(float(r))
        data["t"].append(float(t))
        data["millis"].append(float(m))
        
except Exception as e:
    print(e)

# print(len(data["millis"][::100]))

##plot data

N_Points = 1

N_X_LABELS = 10

size_data = len(data["millis"])
print(size_data)

fig, ax = plt.subplots(figsize=(6, 6))
ax.plot(data["millis"], data["x"], label="x-Acelleration")

# print(max(data["x"])) [::int(size_data/N_Points)]

ax.set_xticks(data["millis"][::int(size_data/N_X_LABELS)])  # Show tick every nPoints data point
ax.yaxis.set_major_locator(ticker.MaxNLocator(integer=True, prune='lower'))  # Can adjust 'integer' or 'prune'
ax.legend()

## integrate X
integrated_x = []
for nr,el in enumerate(data["x"]):    
    if len(integrated_x) == 0:
        integrated_x.append(el)
    else:
        delta_time = data["millis"][nr] - data["millis"][nr-1]
        delta_x = data["x"][nr] - data["x"][nr-1]
        integrated_x.append(integrated_x[nr-1] + delta_x*delta_time)

# ax.plot(integrated_x, integrated_x, label="x-integration")

plt.show()