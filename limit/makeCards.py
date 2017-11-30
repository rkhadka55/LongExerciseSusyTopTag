import math
from optparse import OptionParser

class proc:
    def __init__(self,name,mass=0,count=0,weight=0,syst=0):
        self.name = name
        self.mass = mass
        self.count = count
        self.weight = weight
        self.syst = syst
    def rate(self):
        return self.count*self.weight
    def lines(self,num,tot):
        line_stat = self.name+"_stat_unc gmN "+"{:d}".format(self.count)+" "+' '.join(["{:0.5f}".format(self.weight) if i==num else "-" for i in xrange(tot)])
        line_syst = self.name+"_syst_unc lnN "+' '.join(["{:0.3f}".format(1+self.syst) if i==num else "-" for i in xrange(tot)])
        return [line_stat,line_syst]
    def __repr__(self):
        line = (
            self.name+(str(self.mass) if self.mass>0 else "")+"\n"
            "\t"+str(self.rate())+"\n"
            "\t"+str(self.syst)
        )
        return line

def makeCard(procs,obs,oname):
    # print out a card
    oname = oname+"_"+procs[0].name+"_"+str(procs[0].mass)+".txt"
    with open(oname,'w') as ofile:
        divider = 50
        ofile.write("imax 1 number of bins\n")
        ofile.write("jmax "+str(len(procs)-1)+" number of backgrounds\n")
        ofile.write("kmax * number of nuisances\n")
        ofile.write('-'*divider+"\n")
        ofile.write("bin ch1\n")
        ofile.write("observation {:0.0f}\n".format(obs))
        ofile.write('-'*divider+"\n")
        ofile.write("bin "+"ch1 "*len(procs)+"\n")
        ofile.write("process "+' '.join([proc.name for proc in procs])+"\n")
        ofile.write("process "+' '.join([str(i) for i in xrange(len(procs))])+"\n")
        ofile.write("rate "+' '.join(["{:0.3f}".format(p.rate()) for p in procs])+"\n")
        ofile.write('-'*divider+"\n")
        for i,p in enumerate(procs):
            lines = p.lines(i,len(procs))
            for line in lines:
                ofile.write(line+"\n")    
        
def makeCards(iname,oname):
    bkgs = []
    signals = []
    obs = 0
    # make processes from text (csv) input
    with open(iname,'r') as ifile:
        for line in ifile:
            line = line.rstrip()
            linesplit = line.split(',')
            if linesplit[0]=="obs":
                obs = int(linesplit[1])
            elif linesplit[0]=="signal" or linesplit[0]=="T1tttt" or linesplit[0]=="T2tt":
                signals.append(
                    proc(
                        linesplit[0],
                        mass = int(linesplit[1]),
                        count = int(linesplit[2]),
                        weight = float(linesplit[3]),
                        syst = float(linesplit[4])
                    )
                )
            else:
                bkgs.append(
                    proc(
                        linesplit[0],
                        count = int(linesplit[1]),
                        weight = float(linesplit[2]),
                        syst = float(linesplit[3])
                    )
                )

    for signal in signals:
        makeCard([signal]+bkgs,obs,oname)
        
    #print procs
    
if __name__=="__main__":
    parser = OptionParser()
    parser.add_option("-i","--input",dest="input",default="",help="input filename")
    parser.add_option("-o","--output",dest="output",default="",help="output filename prefix")
    (options, args) = parser.parse_args()
    
    if len(options.input)==0:
        parser.error("Must specify input filename")
    if len(options.output)==0:
        parser.error("Must specify output filename prefix")
    
    makeCards(options.input,options.output)
