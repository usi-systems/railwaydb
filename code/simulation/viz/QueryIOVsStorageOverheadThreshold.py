import CommonConf

import re
from collections import OrderedDict
import numpy as np
import matplotlib.pyplot as pp

def main(dirn, fname):
  attributeSizes = []
  ioPerSolver = OrderedDict()

  with open(dirn+"/"+fname+".dat") as fin:
    lines = fin.readlines()

    for line in lines:
      if line.startswith("#"):
        continue
        
      (solver, attributes, io, deviation, nline) = re.split("[\t]", line)

      if 'n/a' in io:
        io = 0
        deviation = 0
      else:
        io = float(io)
        deviation = float(deviation)

      attributes = float(attributes)

      if solver in ioPerSolver:
        ioPerSolver[solver].append((io,deviation))
      else:
        ioPerSolver[solver] = [(io,deviation)]
      if len(attributeSizes) == 0 or attributes > attributeSizes[-1]:
        attributeSizes.append(attributes)

  CommonConf.setupMPPDefaults()
  fmts = CommonConf.getLineFormats()
  fig = pp.figure()
  ax = fig.add_subplot(111)
      
  index = 0

  for solver, ioAndDeviation in ioPerSolver.iteritems():
    ioAndDeviationLists = map(list, zip(*ioAndDeviation))
    ax.errorbar(attributeSizes, ioAndDeviationLists[0],
                yerr=ioAndDeviationLists[1], label=solver,
                marker=fmts[index][0], linestyle=fmts[index][1])
    index = index + 1  

  ax.set_xlabel('Storage Overhead Threshold');
  ax.set_ylabel('I/O Cost (bytes)');
  # ax.set_xlim(0, 2100)
  ax.legend(loc='best', fancybox=True)

  pp.savefig(dirn+"/"+fname+".pdf")
  pp.show()

if __name__ == "__main__":
  main("expData", "QueryIOVsStorageOverheadThreshold")

