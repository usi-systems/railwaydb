import CommonConf

import re
from collections import OrderedDict
import numpy as np
import matplotlib.pyplot as pp

def main(dirn, fname):
  blockSizes = []
  localitiesPerApproach = OrderedDict()

  with open(dirn+"/"+fname+".dat") as fin:
    lines = fin.readlines()

    for line in lines:
      if line.startswith("#"):
        continue
      (blockSize, approach, locality, nline) = re.split("[\t]", line)
      blockSize = float(blockSize)
      locality = float(locality)
      if approach in localitiesPerApproach:
        localitiesPerApproach[approach].append(locality)
      else:
        localitiesPerApproach[approach] = [locality]
      if len(blockSizes) == 0 or blockSize > blockSizes[-1]:
        blockSizes.append(blockSize)

  CommonConf.setupMPPDefaults()
  fmts = CommonConf.getLineFormats()
  fig = pp.figure()
  ax = fig.add_subplot(111)
  #ax.set_xscale("log", basex=2)
    
  index = 0
  for approach, localities in localitiesPerApproach.iteritems():
    ax.plot(blockSizes, localities, label=approach, 
            marker=fmts[index][0], linestyle=fmts[index][1])
    index = index + 1

  ax.set_xlabel('block size (bytes)');
  ax.set_ylabel('locality');
  # ax.set_xlim(0, 2100)
  ax.legend(loc='best', fancybox=True)

  pp.savefig(dirn+"/"+fname+".pdf")
  pp.show()

if __name__ == "__main__":
  main("expData", "SampleExperiment")

