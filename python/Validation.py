import sys
import os
import errno
import pandas as pd
import numpy
import math
from DataGetter import DataGetter
import optparse
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import pickle

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-c', "--disc",         dest='discCut',      action='store', default=0.6, type=float, help="Discriminator cut")
parser.add_option ('-a', "--mvaFile",      dest='mvaFile',      action='store', default="",              help="Mva training file")
parser.add_option ('-f', "--dataFilePath", dest="dataFilePath", action='store', default="data",          help="Path where the input datafiles are stored")
parser.add_option ('-d', "--directory",    dest='directory',    action='store', default="",              help="Directory to store outputs")
 
options, args = parser.parse_args()

outputDirectory = ""
if len(options.directory):
  outputDirectory = options.directory + "/"
  try:
      os.mkdir(outputDirectory)
  except OSError as exc:
      if exc.errno == errno.EEXIST and os.path.isdir(outputDirectory):
          pass
      else:
          raise

#disc cut
discCut = options.discCut

print "RETRIEVING MODEL FILE"

import xgboost as xgb

if len(options.mvaFile):
    bst = xgb.Booster(model_file=options.mvaFile) # load data
else:
    bst = xgb.Booster(model_file=outputDirectory+"./TrainingModel.xgb") # load data


print "PROCESSING TTBAR VALIDATION DATA"

#Let us see if the input variables have been set by JSON
import json

allVars = ["cand_m", "j12_m", "j13_m", "j23_m"]
try:
    with open(outputDirectory+"variables.json", "r") as f:
        allVars = json.load(f)
except IOError:
    print "Unable to load variable file variables.json"


dg = DataGetter.DefinedVariables(allVars)
varsname = dg.getList()

if options.dataFilePath != None: 
    if options.dataFilePath[-1] != "/": # check for /
        options.dataFilePath += "/"
    options.dataPath = options.dataFilePath


def getData(dataName):
    if ".pkl" in dataName:
        dataAll = pd.read_pickle(dataName)
    elif ".h5" in dataName:
        import h5py
        f = h5py.File(options.dataPath + dataName, "r")
        npData = f["reco_candidates"][:]
        columnHeaders = f["reco_candidates"].attrs["column_headers"]
        indices = [npData[:,0].astype(numpy.int), npData[:,1].astype(numpy.int)]
        dataAll = pd.DataFrame(npData[:,2:], index=pd.MultiIndex.from_arrays(indices), columns=columnHeaders[2:])
    
        npDataGen = f["gen_tops"][:]
        columnHeadersGen = f["gen_tops"].attrs["column_headers"]
        indicesGen = [npDataGen[:,0].astype(numpy.int), npDataGen[:,1].astype(numpy.int)]
        dataGen = pd.DataFrame(npDataGen[:,2:], index=pd.MultiIndex.from_arrays(indicesGen), columns=columnHeadersGen[2:])
    
        f.close()
    
    return dataAll, dataGen

def getDataTTbar(tSample, tbarSample):
    dataTTbarTAll,    dataTTbarTGen    = getData(tSample)
    dataTTbarTbarAll, dataTTbarTbarGen = getData(tbarSample)

    NTEVENTS = 60995379.0
    NTBAREVENTS = 60125087.0
    dataTTbarTAll.sampleWgt *= NTEVENTS/dataTTbarTAll.shape[0]
    dataTTbarTbarAll.sampleWgt *= NTBAREVENTS/dataTTbarTbarAll.shape[0]
    dataTTbarTGen.sampleWgt *= NTEVENTS/dataTTbarTGen.shape[0]
    dataTTbarTbarGen.sampleWgt *= NTBAREVENTS/dataTTbarTbarGen.shape[0]

    dataTTbarAll = pd.concat([dataTTbarTAll, dataTTbarTbarAll])
    dataTTbarGen = pd.concat([dataTTbarTGen, dataTTbarTbarGen])

    return dataTTbarAll, dataTTbarGen

def getDataZnunu(sampleInfo):
    data = []
    dataGen = []

    for key in sampleInfo:
        dataAll,    dataGenAll    = getData(key)

        NEVENTS = sampleInfo[key]
        dataAll.sampleWgt *= NEVENTS/dataAll.index.levels[0][-1]
        dataGenAll.sampleWgt *= NEVENTS/dataGenAll.index.levels[0][-1]
        
        data.append(dataAll)
        dataGen.append(dataGenAll)

    dataAll = pd.concat(data)
    dataGen = pd.concat(dataGen)

    return dataAll, dataGen

dataTTbarAll, dataTTbarGen = getDataTTbar("trainingTuple_TTbarSingleLepT_0_division_2_TTbarSingleLepT_test_0.h5", "trainingTuple_TTbarSingleLepTbar_0_division_2_TTbarSingleLepTbar_test_0.h5")

#Apply baseline cuts
dataTTbarAll = dataTTbarAll[dataTTbarAll.Njet >= 4]
dataTTbar = dataTTbarAll[dataTTbarAll.ncand > 0]

dataTTbarGen = dataTTbarGen[dataTTbarGen.Njet >= 4]

dataTTbarAllTrain, dataTTbarGenTrain = getDataTTbar("trainingTuple_TTbarSingleLepT_0_division_0_TTbarSingleLepT_training_0.h5", "trainingTuple_TTbarSingleLepTbar_0_division_0_TTbarSingleLepTbar_training_0.h5")

#Apply baseline cuts
dataTTbarAllTrain = dataTTbarAllTrain[dataTTbarAllTrain.Njet >= 4]
dataTTbarTrain = dataTTbarAllTrain[dataTTbarAllTrain.ncand > 0]

dataTTbarGenTrain = dataTTbarGenTrain[dataTTbarGenTrain.Njet >= 4]

print "CALCULATING TTBAR DISCRIMINATORS"

xgData = xgb.DMatrix(dataTTbar.as_matrix(varsname))
dataTTbarAns = bst.predict(xgData)

xgData = xgb.DMatrix(dataTTbar.as_matrix(varsname))
dataTTbarAnsTrain = bst.predict(xgb.DMatrix(dataTTbarTrain.as_matrix(varsname)))

discCutTTbar = numpy.array([options.discCut]*len(dataTTbarAns))

discCutTTbarTrain = numpy.array([options.discCut]*len(dataTTbarAnsTrain))


print "CREATING HISTOGRAMS"

#Discriminator plot

inputLabels = dataTTbar.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatches = (inputLabels[:,0] == 3) & (inputLabels[:,1] == 1)

inputLabelsTrain = dataTTbarTrain.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatchesTrain = (inputLabelsTrain[:,0] == 3) & (inputLabelsTrain[:,1] == 1)

plt.clf()
plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator.png")
plt.close()

plt.clf()
filterVec = (dataTTbar.cand_pt < 100).as_matrix()
filterVecTrain = (dataTTbarTrain.cand_pt < 100).as_matrix()
plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_0_100.png")
plt.close()

plt.clf()
filterVec = ((100 <= dataTTbar.cand_pt) & (dataTTbar.cand_pt < 200)).as_matrix()
filterVecTrain = ((100 <= dataTTbarTrain.cand_pt) & (dataTTbarTrain.cand_pt < 200)).as_matrix()
plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_100_200.png")
plt.close()

plt.clf()
filterVec = ((200 <= dataTTbar.cand_pt) & (dataTTbar.cand_pt < 300)).as_matrix()
filterVecTrain = ((200 <= dataTTbarTrain.cand_pt) & (dataTTbarTrain.cand_pt < 300)).as_matrix()
plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_200_300.png")
plt.close()

plt.clf()
filterVec = ((300 <= dataTTbar.cand_pt) & (dataTTbar.cand_pt < 400)).as_matrix()
filterVecTrain = ((300 <= dataTTbarTrain.cand_pt) & (dataTTbarTrain.cand_pt < 400)).as_matrix()
plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_300_400.png")
plt.close()

plt.clf()
filterVec = ((400 <= dataTTbar.cand_pt)).as_matrix()
filterVecTrain = ((400 <= dataTTbarTrain.cand_pt)).as_matrix()
plt.hist(dataTTbarAns[filterVec & (genMatches == 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches == 1)], bins=50, normed=True, label="Gen Matched",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[filterVec & (genMatches != 1)], weights=dataTTbar["sampleWgt"][filterVec & (genMatches != 1)], bins=50, normed=True, label="Not gen matched", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain == 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain == 1)], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", edgecolor="red")
plt.hist(dataTTbarAnsTrain[filterVecTrain & (genMatchesTrain != 1)], weights=dataTTbarTrain["sampleWgt"][filterVecTrain & (genMatchesTrain != 1)], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_400_Inf.png")
plt.close()

plt.clf()
plt.hist(dataTTbarAns[genMatches == 1], weights=dataTTbar["sampleWgt"][genMatches == 1], bins=50, normed=True, label="Gen Matched Validation",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[genMatches != 1], weights=dataTTbar["sampleWgt"][genMatches != 1], bins=50, normed=True, label="Not gen matched Validation", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[genMatchesTrain == 1], weights=dataTTbarTrain["sampleWgt"][genMatchesTrain == 1], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="red")
plt.hist(dataTTbarAnsTrain[genMatchesTrain != 1], weights=dataTTbarTrain["sampleWgt"][genMatchesTrain != 1], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_validVsTrain.png")
plt.close()

plt.clf()
plt.hist(dataTTbarAns[dataTTbar.cand_pt.as_matrix() > 200][genMatches[dataTTbar.cand_pt.as_matrix() > 200] == 1], weights=dataTTbar[dataTTbar.cand_pt.as_matrix() > 200]["sampleWgt"][genMatches[dataTTbar.cand_pt.as_matrix() > 200] == 1], bins=50, normed=True, label="Gen Matched Validation",     fill=False, histtype='step', edgecolor="red")
plt.hist(dataTTbarAns[dataTTbar.cand_pt.as_matrix() > 200][genMatches[dataTTbar.cand_pt.as_matrix() > 200] != 1], weights=dataTTbar[dataTTbar.cand_pt.as_matrix() > 200]["sampleWgt"][genMatches[dataTTbar.cand_pt.as_matrix() > 200] != 1], bins=50, normed=True, label="Not gen matched Validation", fill=False, histtype='step', edgecolor="blue")
plt.hist(dataTTbarAnsTrain[dataTTbarTrain.cand_pt.as_matrix() > 200][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] == 1], weights=dataTTbarTrain[dataTTbarTrain.cand_pt.as_matrix() > 200]["sampleWgt"][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] == 1], bins=50, normed=True, label="Gen Matched Train",     fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="red")
plt.hist(dataTTbarAnsTrain[dataTTbarTrain.cand_pt.as_matrix() > 200][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] != 1], weights=dataTTbarTrain[dataTTbarTrain.cand_pt.as_matrix() > 200]["sampleWgt"][genMatchesTrain[dataTTbarTrain.cand_pt.as_matrix() > 200] != 1], bins=50, normed=True, label="Not gen matched Train", fill=False, histtype='step', linestyle="dotted", linewidth=2, edgecolor="blue")
plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Normalized events")
plt.savefig(outputDirectory + "discriminator_validVsTrain_highpt.png")
plt.close()

#plot efficiency

effPtBins = numpy.hstack([[0], numpy.linspace(50, 200, 7), numpy.linspace(250, 500, 6), [600, 700, 800, 1000]])
ptNum, _ = numpy.histogram(dataTTbar[genMatches == 1][dataTTbarAns[genMatches == 1] > discCutTTbar[genMatches == 1]]["genConstMatchGenPtVec"], bins=effPtBins, weights=dataTTbar[genMatches == 1][dataTTbarAns[genMatches == 1] > discCutTTbar[genMatches == 1]]["sampleWgt"])
ptDen, _ = numpy.histogram(dataTTbarGen["genTopPt"], bins=effPtBins, weights=dataTTbarGen["sampleWgt"])

effPt = ptNum/ptDen

ptNumTrain, _ = numpy.histogram(dataTTbarTrain[genMatchesTrain == 1][dataTTbarAnsTrain[genMatchesTrain == 1] > discCutTTbarTrain[genMatchesTrain == 1]]["genConstMatchGenPtVec"], bins=effPtBins, weights=dataTTbarTrain[genMatchesTrain == 1][dataTTbarAnsTrain[genMatchesTrain == 1] > discCutTTbarTrain[genMatchesTrain == 1]]["sampleWgt"])
ptDenTrain, _ = numpy.histogram(dataTTbarGenTrain["genTopPt"], bins=effPtBins, weights=dataTTbarGenTrain["sampleWgt"])

effPtTrain = ptNumTrain/ptDenTrain

plt.clf()
if not numpy.any(effPt):
    effPt[-1] = 0.00001
plt.hist(effPtBins[:-1], bins=effPtBins, weights=effPt,      fill=False, histtype='step', label="Test")
plt.hist(effPtBins[:-1], bins=effPtBins, weights=effPtTrain, fill=False, histtype='step', label="Training", linestyle="dotted")
#plt.legend(loc='upper right')
plt.xlabel("Candidate Pt [GeV]")
plt.ylabel("Efficiency")
plt.savefig(outputDirectory + "efficiency.png")
plt.close()

#input variable histograms

genTopData = dataTTbar[genMatches == 1]
genBGData = dataTTbar[genMatches != 1]
recoTopData = dataTTbar[dataTTbarAns > discCutTTbar]
recoBGData = dataTTbar[dataTTbarAns < discCutTTbar]
minTTbar = dataTTbar.min()
maxTTbar = dataTTbar.max()

for var in varsname:
    plt.clf()
    bins = numpy.linspace(minTTbar[var], maxTTbar[var], 21)
    ax = recoTopData .hist(column=var, weights=recoTopData["sampleWgt"], bins=bins, grid=False, normed=False, fill=False, histtype='step',                     label="reco top")
    recoBGData       .hist(column=var, weights=recoBGData["sampleWgt"],  bins=bins, grid=False, normed=False, fill=False, histtype='step',                     label="reco bg", ax=ax)
    genTopData       .hist(column=var, weights=genTopData["sampleWgt"],  bins=bins, grid=False, normed=False, fill=False, histtype='step', linestyle="dotted", label="gen top", ax=ax)
    genBGData        .hist(column=var, weights=genBGData["sampleWgt"],   bins=bins, grid=False, normed=False, fill=False, histtype='step', linestyle="dotted", label="gen bkg", ax=ax)
    plt.legend()
    plt.xlabel(var)
    plt.ylabel("Normalized events")
    #plt.yscale('log')
    plt.savefig(outputDirectory + var + ".png")
    plt.close()


#purity plots

plt.clf()

ptBins = numpy.hstack([[0], numpy.linspace(50, 200, 7), numpy.linspace(250, 500, 6), [600, 700, 800, 1000]])
purityNum, _ = numpy.histogram(dataTTbar["cand_pt"][dataTTbarAns > discCutTTbar][genMatches[dataTTbarAns > discCutTTbar] == 1], bins=ptBins, weights=dataTTbar["sampleWgt"][dataTTbarAns > discCutTTbar][genMatches[dataTTbarAns > discCutTTbar] == 1])
purityDen,_  = numpy.histogram(dataTTbar["cand_pt"][dataTTbarAns > discCutTTbar],                  bins=ptBins, weights=dataTTbar["sampleWgt"][dataTTbarAns > discCutTTbar])

purityDen[purityDen == 0.0] = 100000.0

purity = purityNum/purityDen

if not numpy.any(purity):
    purity[-1] = 0.001

plt.hist(ptBins[:-1], bins=ptBins, weights=purity, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("pT [GeV]")
plt.ylabel("Purity")
plt.savefig(outputDirectory + "purity.png")
plt.close()

plt.clf()

discBins = numpy.linspace(0, 1, 21)
purityDiscNum, _ = numpy.histogram(dataTTbarAns[genMatches == 1], bins=discBins, weights=dataTTbar["sampleWgt"][genMatches == 1])
purityDiscDen,_  = numpy.histogram(dataTTbarAns,                  bins=discBins, weights=dataTTbar["sampleWgt"])

purityDiscDen[purityDiscDen == 0.0] = 100000.0

purityDisc = purityDiscNum/purityDiscDen

if not numpy.any(purityDisc):
    purityDisc[-1] = 0.0000001

plt.hist(discBins[:-1], bins=discBins, weights=purityDisc, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("Discriminator")
plt.ylabel("Purity")
plt.savefig(outputDirectory + "purity_disc.png")
plt.close()

print "PROCESSING ZNUNU VALIDATION DATA"

dataZnunuAll, _ = getDataZnunu({"trainingTuple_ZJetsToNuNu_HT_100to200_0_division_2_ZJetsToNuNu_HT_100to200_test_0.h5": 24006616.0,
                                "trainingTuple_ZJetsToNuNu_HT_200to400_0_division_2_ZJetsToNuNu_HT_200to400_test_0.h5": 24450102.0,
                                "trainingTuple_ZJetsToNuNu_HT_400to600_0_division_2_ZJetsToNuNu_HT_400to600_test_0.h5": 9627133.0,
                                "trainingTuple_ZJetsToNuNu_HT_600to800_0_division_2_ZJetsToNuNu_HT_600to800_test_0.h5": 5671792.0,
                                "trainingTuple_ZJetsToNuNu_HT_800to1200_0_division_2_ZJetsToNuNu_HT_800to1200_test_0.h5": 2170137.0,
                                "trainingTuple_ZJetsToNuNu_HT_1200to2500_0_division_2_ZJetsToNuNu_HT_1200to2500_test_0.h5": 513471.0,
                                "trainingTuple_ZJetsToNuNu_HT_2500toInf_0_division_2_ZJetsToNuNu_HT_2500toInf_test_0.h5": 405030.0
                            })

dataZnunuAll = dataZnunuAll[dataZnunuAll.Njet >= 4]
dataZnunu = dataZnunuAll[dataZnunuAll.ncand > 0]

print "CALCULATING ZNUNU DISCRIMINATORS"

xgData = xgb.DMatrix(dataZnunu.as_matrix(varsname))
dataZnunuAns = bst.predict(xgData)

discCutZnunu = numpy.array([options.discCut]*len(dataZnunuAns))

#calculate fake rate 

plt.clf()

metBins = numpy.linspace(0, 1000, 21)
frMETNum, _ = numpy.histogram(dataZnunu[dataZnunuAns > discCutZnunu]["MET"].ix[:,0], bins=metBins, weights=dataZnunu[dataZnunuAns > discCutZnunu]["sampleWgt"].ix[:,0])
frMETDen,_  = numpy.histogram(dataZnunuAll["MET"].ix[:,0],                      bins=metBins, weights=dataZnunuAll["sampleWgt"].ix[:,0])

frMETNum[frMETDen < 1e-10] = 0.0
frMETDen[frMETDen < 1e-10] = 1.0
frMET = frMETNum/frMETDen

if not numpy.any(frMET):
    frMET[-1] = 0.0000001

plt.hist(metBins[:-1], bins=metBins, weights=frMET, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("MET [GeV]")
plt.ylabel("Fake rate")
plt.savefig(outputDirectory + "fakerate_met.png")
plt.close()

plt.clf()

njBins = numpy.linspace(0, 20, 21)
frNjNum, _ = numpy.histogram(dataZnunu[dataZnunuAns > discCutZnunu]["Njet"].ix[:,0], bins=njBins, weights=dataZnunu[dataZnunuAns > discCutZnunu]["sampleWgt"].ix[:,0])
frNjDen,_  = numpy.histogram(dataZnunuAll["Njet"].ix[:,0],                      bins=njBins, weights=dataZnunuAll["sampleWgt"].ix[:,0])

frNjNum[frNjDen < 1e-10] = 0.0
frNjDen[frNjDen < 1e-10] = 1.0
frNj = frNjNum/frNjDen

if not numpy.any(frNj):
    frNj[-1] = 0.0000001

plt.hist(njBins[:-1], bins=njBins, weights=frNj, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("N jets")
plt.ylabel("Fake rate")
plt.savefig(outputDirectory + "fakerate_njets.png")
plt.close()

plt.clf()

frCandPtNum, _ = numpy.histogram(dataZnunu[dataZnunuAns > discCutZnunu]["cand_pt"].ix[:,0], bins=ptBins, weights=dataZnunu[dataZnunuAns > discCutZnunu]["sampleWgt"].ix[:,0])
frCandPtDen, _ = numpy.histogram(dataZnunu["cand_pt"].ix[:,0],                         bins=ptBins, weights=dataZnunu["sampleWgt"].ix[:,0])

frCandPtNum[frCandPtDen < 1e-10] = 0.0
frCandPtDen[frCandPtDen < 1e-10] = 1.0
frCandPt = frCandPtNum/frCandPtDen

if not numpy.any(frCandPt):
    frCandPt[-1] = 0.0000001

plt.hist(ptBins[:-1], bins=ptBins, weights=frCandPt, fill=False, histtype='step')
#plt.legend(loc='upper right')
plt.xlabel("Candidate pt")
plt.ylabel("Fake rate")
plt.savefig(outputDirectory + "fakerate_cand_pt.png")
plt.close()

print "CALCULATING ROC CURVES"

cuts = numpy.hstack([numpy.arange(0.0, 0.05, 0.005), numpy.arange(0.05, 0.95, 0.01), numpy.arange(0.95, 1.00, 0.005)])

PtCutList = [0, 100, 200, 300, 400]
PtCutMap = {}

evtwgt = dataTTbar["sampleWgt"]
evtwgtZnunu = dataZnunu["sampleWgt"]

candPtTTbar = dataTTbar["cand_pt"]
candPtZnunu = dataZnunu["cand_pt"]

dataTTbarAnsRoc = (dataTTbarAns - dataTTbarAns.min()) / dataTTbarAns.ptp()
dataZnunuAnsRoc = (dataZnunuAns - dataZnunuAns.min()) / dataZnunuAns.ptp()

for i in xrange(len(PtCutList)):
    pt_min = PtCutList[i]
    pt_max = -1 # final pt cut
    if i < len(PtCutList) - 1: # not final pt cut
        pt_max = PtCutList[i+1]

    if pt_max > 0:
        cutKey = "pt_{0}_to_{1}".format(pt_min, pt_max)
        print "Calculating ROC Curve for Pt from {0} to {1} GeV; cutKey = {2}".format(pt_min, pt_max, cutKey)
    else:
        cutKey = "pt_{0}_to_infinity".format(pt_min)
        print "Calculating ROC Curve for Pt greater than {0} GeV; cutKey = {1}".format(pt_min, cutKey)

    TPRPtCut = []
    FPRPtCut = []
    FPRZPtCut = []

    if pt_max > 0: # not final pt cut
        NevtTPRPtCut = evtwgt[(dataTTbar.genConstiuentMatchesVec == 3) & (candPtTTbar > pt_min) & (candPtTTbar < pt_max)].sum()
        NevtFPRPtCut = evtwgt[(dataTTbar.genConstiuentMatchesVec != 3) & (candPtTTbar > pt_min) & (candPtTTbar < pt_max)].sum()
        NevtZPtCut = evtwgtZnunu[(candPtZnunu > pt_min) & (candPtZnunu < pt_max)].sum()
    else: # final pt cut (from min to infinity)
        NevtTPRPtCut = evtwgt[(dataTTbar.genConstiuentMatchesVec == 3) & (candPtTTbar > pt_min)].sum()
        NevtFPRPtCut = evtwgt[(dataTTbar.genConstiuentMatchesVec != 3) & (candPtTTbar > pt_min)].sum()
        NevtZPtCut = evtwgtZnunu[(candPtZnunu > pt_min)].sum()
    
    for cut in cuts:
        if pt_max > 0: # not final pt cut
            TPRPtCut.append( evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec == 3) & (candPtTTbar > pt_min) & (candPtTTbar < pt_max)].sum() / NevtTPRPtCut )
            FPRPtCut.append( evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec != 3) & (candPtTTbar > pt_min) & (candPtTTbar < pt_max)].sum() / NevtFPRPtCut )
            FPRZPtCut.append( evtwgtZnunu[(dataZnunuAnsRoc > cut) & (candPtZnunu > pt_min) & (candPtZnunu < pt_max)].sum() / NevtZPtCut )
        else: # final pt cut (from min to infinity)
            TPRPtCut.append( evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec == 3) & (candPtTTbar > pt_min)].sum() / NevtTPRPtCut )
            FPRPtCut.append( evtwgt[(dataTTbarAnsRoc > cut) & (dataTTbar.genConstiuentMatchesVec != 3) & (candPtTTbar > pt_min)].sum() / NevtFPRPtCut )
            FPRZPtCut.append( evtwgtZnunu[(dataZnunuAnsRoc > cut) & (candPtZnunu > pt_min)].sum() / NevtZPtCut )

    PtCutMap[cutKey] = {"TPR" : TPRPtCut, "FPR" : FPRPtCut, "FPRZ" : FPRZPtCut, "PtMin" : pt_min, "PtMax" : pt_max }


#Dump roc to file
fileObject = open(outputDirectory + "roc.pkl",'wb')
pickle.dump(PtCutMap, fileObject)
fileObject.close()

print "VALIDATION DONE!"
