import matplotlib.pyplot as plt
import matplotlib.lines as mlines
import pickle
import json
import numpy as np
import errno
import os
import optparse
from datetime import datetime

class Plotter:
    def __init__(self):
        self.outputDirectory = ""
        self.jsonFile = "rocPlots.json"
        self.colors = ["red", "blue", "green", "orange", "black", "purple", "yellow", "pink", "maroon", "xkcd:sky blue", "xkcd:violet", "xkcd:cerulean",
                       "xkcd:light red", "xkcd:sea blue", "xkcd:emerald", "xkcd:reddish purple", "xkcd:dark rose", "xkcd:aubergine", "xkcd:teal green", "xkcd:avocado"]


def main():
    parser = optparse.OptionParser("usage: %prog [options]\n")
    parser.add_option('-d', "--directory",   dest='directory',   action='store',      default="",               help="Directory to store outputs")
    parser.add_option('-f', "--files",       dest='files',       action='store',      default="",               help="ROC pkl files, semicolon seperated")
    parser.add_option('-l', "--labels",      dest='labels',      action='store',      default="",               help="Legend labels for each ROC pkl file, semicolon seperated")
    parser.add_option('-n', "--name",        dest='name',        action='store',      default="DAS",            help="String to add to plot name to identify this figure")
    
    options, args = parser.parse_args()

    p = Plotter()
    
    if len(options.directory):
      p.outputDirectory = options.directory
      if p.outputDirectory[-1] != "/":
          p.outputDirectory += "/"
      try:
          os.mkdir(p.outputDirectory)
      except OSError as exc:
          if exc.errno == errno.EEXIST and os.path.isdir(p.outputDirectory):
              pass
          else:
              raise
    
    p.name = options.name
    p.files  = options.files.split(";")
    if len(options.labels):
        p.labels = options.labels.split(";")
    else:
        p.labels = p.files

    makeCutPlots(p)


# make one plot per pt cut with multiple models per plot
def makeCutPlots(plotter):
    print "MAKING PT CUT PLOTS"
    p = plotter
    style = "solid"

    # get files
    name = p.name
    files = p.files
    labels = p.labels
    
    PtCutList = []
    # get p_t cut maps from files
    for file1 in files:
        print "OPENING PICKLE; NAME: {0} FILE: {1}".format(name, file1)
        f1 = open(file1, "rb")
        PtCutMap = pickle.load(f1) 
        PtCutList.append(PtCutMap)
    
    # plot p_t cuts per file
    for cut in sorted(PtCutList[0].iteritems()):
        plotRoc  = plt.figure()
        plotRocAx  = plotRoc.add_subplot(111)
        rocs  = []
        ifile = 0
        for file1, label in zip(files, labels):
            PtCutMap = PtCutList[ifile]
            color = p.colors[ifile]
            print "File: {0} Label: {1} Color: {2} Cut: {3}".format(file1, label, color, cut[0])
            PtCutData = PtCutMap[cut[0]]
            TPRPtCut  = PtCutData["TPR"]  
            FPRPtCut  = PtCutData["FPR"] 
            pt_min = PtCutData["PtMin"]
            pt_max = PtCutData["PtMax"]
            
            rocs.append(plotRocAx.plot(FPRPtCut,      TPRPtCut, label=label, linestyle=style, color=color, alpha=1.0)[0])
            ifile += 1
        
        fileLabel = cut[0]
        plotLabel = ""
        if pt_max > 0:
            plotLabel = r'${0}$ GeV $< p_T < {1}$ GeV'.format(pt_min, pt_max)
        else:
            plotLabel = r'$p_T > {0}$ GeV'.format(pt_min)
        
        # crate plot for each cut
        first_legend = plotRocAx.legend(handles=rocs, loc="lower right")
        plotRoc.gca().add_artist(first_legend)
        
        plotRocAx.set_xlabel("FPR (ttbar)")
        plotRocAx.set_ylabel("TPR (ttbar)")
        plotRocAx.set_title("ROC Plot for TPR (ttbar) vs FPR (ttbar) "+plotLabel)
        plotRocAx.set_xlim(0.0, 1.0)
        plotRocAx.set_ylim(0.0, 1.0)
        plotRoc.savefig("{0}roc_{1}_{2}.png".format(p.outputDirectory, name, fileLabel))
        plotRoc.savefig("{0}roc_{1}_{2}.pdf".format(p.outputDirectory, name, fileLabel))
        plt.close(plotRoc)
    
if __name__ == "__main__":
    main()
