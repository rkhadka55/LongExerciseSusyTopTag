import argparse
import os
import subprocess
from multiprocessing import Pool
from datetime import datetime
import tqdm

def runSimpleAnalyzer(optlist):
    command = ["./RunSimpleAnalyzer"]
    command.extend(optlist)
    if "-Q" not in optlist:
        print "running command", " ".join(command)
    subprocess.call(command)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-o", "--outdir", help="Output directory to store the files", default="myhistos")
    parser.add_argument("-n", "--npool", dest="npool", default=4, help="number of processes to run", type=int)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="Print more stdout messages")
    args = parser.parse_args()

    if not os.path.isdir(args.outdir):
        os.mkdir(args.outdir)

    samplelist = ["TTbarNoHad",
                  "ttbarW",
                  "Rare",
                  "WJetsToLNu",
                  "ZJetsToNuNu",
                  "QCD",
                  "ST",
                  "Diboson",
                  "TTZ",
                  "Data_MET",
                  "Signal_fastsim_T2tt",
                  "Signal_fastsim_T2tt_mStop-800",
                  "Signal_fastsim_T2tt_mStop-900",
                  "Signal_fastsim_T2tt_mStop-1000",
                  "Signal_fastsim_T2tt_mStop-1100",
                  "Signal_fastsim_T2tt_mStop-1200",
                  "Signal_fastsim_T1tttt",
                  "Signal_fastsim_T1tttt_mGluino-1700",
                  "Signal_fastsim_T1tttt_mGluino-1800",
                  "Signal_fastsim_T1tttt_mGluino-1900",
                  "Signal_fastsim_T1tttt_mGluino-2000",
                  "Signal_fastsim_T1tttt_mGluino-2100"
                  ]
    
    my_list = []
    for sample in samplelist:
        my_list.append(["-I",sample, "-D", args.outdir])
        if not args.verbose:
            my_list[-1].append("-Q")

    start_time = datetime.now()
    if args.verbose:
        print "Current time: " , str(start_time)
    
    p = Pool(args.npool)
    if args.verbose:
        p.map(runSimpleAnalyzer,my_list)
    else:
        for _ in tqdm.tqdm(p.imap_unordered(runSimpleAnalyzer, my_list), total=len(my_list), unit="sample"):
            pass

    if args.verbose: 
        print "Started on ", str(start_time)
        print "Ended on " , str(datetime.now())
