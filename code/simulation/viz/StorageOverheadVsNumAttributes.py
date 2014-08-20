import CommonConf

import re
from collections import OrderedDict
import numpy as np
import matplotlib.pyplot as pp

def main(dirn, fname):
  attributeSizes = []
  storagePerSolver = OrderedDict()

  with open(dirn+"/"+fname+".dat") as fin:
    lines = fin.readlines()

    for line in lines:
      if line.startswith("#"):
        continue
      (solver, attributes, storage, nline) = re.split("[\t]", line)
      attributes = int(attributes)
      storage = float(storage)
      if solver in storagePerSolver:
        storagePerSolver[solver].append(storage)
      else:
        storagePerSolver[solver] = [storage]
      if len(attributeSizes) == 0 or attributes > attributeSizes[-1]:
        attributeSizes.append(attributes)

  CommonConf.setupMPPDefaults()
  fmts = CommonConf.getLineFormats()
  fig = pp.figure()
  ax = fig.add_subplot(111)
  #ax.set_xscale("log", basex=2)
    
  index = 0
  for solver, storages in storagePerSolver.iteritems():
    ax.plot(attributeSizes, storages, label=solver, 
            marker=fmts[index][0], linestyle=fmts[index][1])
    index = index + 1

  ax.set_xlabel('Number of Attributes');
  ax.set_ylabel('Storage Cost (bytes)');
  # ax.set_xlim(0, 2100)
  ax.legend(loc='best', fancybox=True)

  pp.savefig(dirn+"/"+fname+".pdf")
  pp.show()

if __name__ == "__main__":
  main("expData", "StorageOverheadVsNumAttributes")

