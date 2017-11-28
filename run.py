import argparse
import os
import subprocess

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--outdir", help="Output directory to store the files", default="myhistos")
    args = parser.parse_args()
    if not os.path.isdir(args.outdir):
        os.mkdir(args.outdir)

    samplelist = ["TTbarNoHad",
                  "WJetsToLNu",
                  "ZJetsToLNu",
                  "QCD",
                  "ST",
                  "Diboson",
                  "Rare",
                  "Data_MET"
                  ]

    for sample in samplelist:
        command = ["./RunSimpleAnalyzer", sample, args.outdir]
        print "running command", " ".join(command)
        subprocess.call(command)

        print "/n/n/n"
        print "--------------------------------------"
        print "/n/n/n"
    
    
