import CommonConf

import re
from collections import OrderedDict
import numpy as np
import matplotlib.pyplot as pp

def main(dirn, fname):
  storageOverheadThresholdSizes = []
  timePerSolver = OrderedDict()

  with open(dirn+"/"+fname+".dat") as fin:
    lines = fin.readlines()

    for line in lines:
      if line.startswith("#"):
        continue
      (solver, storageOverheadThresholds, time, deviation, nline) = re.split("[\t]", line)

      storageOverheadThresholds = float(storageOverheadThresholds)
      if 'n/a' in time:
        time = 0
        deviation = 0
      else:
        time = float(time)
        deviation = float(deviation)

      if solver in timePerSolver:
        timePerSolver[solver].append((time, deviation))
      else:
        timePerSolver[solver] = [(time, deviation)]
      if len(storageOverheadThresholdSizes) == 0 or storageOverheadThresholds > storageOverheadThresholdSizes[-1]:
        storageOverheadThresholdSizes.append(storageOverheadThresholds)

  CommonConf.setupMPPDefaults()
  fmts = CommonConf.getLineFormats()
  fig = pp.figure()
  ax = fig.add_subplot(111)
  # ax.set_xscale("log", basex=2)
    
  index = 0
  for solver, timesAndDeviation in timePerSolver.iteritems():
    timesAndDeviationLists = map(list, zip(*timesAndDeviation))
    ax.errorbar(storageOverheadThresholdSizes, timesAndDeviationLists[0],
                yerr=timesAndDeviationLists[1], label=solver,
                marker=fmts[index][0], linestyle=fmts[index][1])
    index = index + 1

  ax.set_xlabel('Storage Overhead Threashold');
  ax.set_ylabel('Time (sec.)');
  # ax.set_xlim(0, 2100)
  ax.legend(loc='best', fancybox=True)

  pp.savefig(dirn+"/"+fname+".pdf")
  pp.show()

if __name__ == "__main__":
  main("expData", "RunningTimeVsStorageOverheadThreshold")

