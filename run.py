import argparse
import os
import subprocess
from multiprocessing import Pool

def runSimpleAnalyzer(optlist):
    command = ["./RunSimpleAnalyzer"]
    command.extend(optlist)
    print "running command", " ".join(command)
    subprocess.call(command)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-o", "--outdir", help="Output directory to store the files", default="myhistos")
    parser.add_argument("-n", "--npool", dest="npool", default=4, help="number of processes to run", type=int)
    args = parser.parse_args()

    if not os.path.isdir(args.outdir):
        os.mkdir(args.outdir)

    samplelist = ["TTbarNoHad",
                  "WJetsToLNu",
                  "ZJetsToNuNu",
                  "QCD",
                  "ST",
                  "Diboson",
                  "Rare",
                  "Data_MET"
                  ]
    
    my_list = []
    for sample in samplelist:
        my_list.append([sample, args.outdir])
    
    p = Pool(args.npool)
    p.map(runSimpleAnalyzer,my_list)
