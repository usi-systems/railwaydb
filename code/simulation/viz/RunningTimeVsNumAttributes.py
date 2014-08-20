import CommonConf

import re
from collections import OrderedDict
import numpy as np
import matplotlib.pyplot as pp

def main(dirn, fname):
  attributeSizes = []
  timePerSolver = OrderedDict()

  with open(dirn+"/"+fname+".dat") as fin:
    lines = fin.readlines()

    for line in lines:
      if line.startswith("#"):
        continue
      (solver, attributes, time, nline) = re.split("[\t]", line)
      attributes = int(attributes)
      time = float(time)
      if solver in timePerSolver:
        timePerSolver[solver].append(time)
      else:
        timePerSolver[solver] = [time]
      if len(attributeSizes) == 0 or attributes > attributeSizes[-1]:
        attributeSizes.append(attributes)

  CommonConf.setupMPPDefaults()
  fmts = CommonConf.getLineFormats()
  fig = pp.figure()
  ax = fig.add_subplot(111)
  ax.set_xscale("log", basex=2)
    
  index = 0
  for solver, times in timePerSolver.iteritems():
    ax.plot(attributeSizes, times, label=solver, 
            marker=fmts[index][0], linestyle=fmts[index][1])
    index = index + 1

  ax.set_xlabel('Number of Attributes');
  ax.set_ylabel('Time (sec.)');
  # ax.set_xlim(0, 2100)
  ax.legend(loc='best', fancybox=True)

  pp.savefig(dirn+"/"+fname+".pdf")
  pp.show()

if __name__ == "__main__":
  main("expData", "RunningTimeVsNumAttributes")

