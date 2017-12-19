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

parser = optparse.OptionParser("usage: %prog [options]\n")

parser.add_option ('-f', "--dataFilePath", dest="dataFilePath", action='store', default="data",          help="Path where the input datafiles are stored")
parser.add_option ('-d', "--directory",    dest='directory',    action='store', default="inputPlots",    help="Directory to store outputs")
 
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

print "PROCESSING TTBAR VALIDATION DATA"

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

dataTTbar, dataTTbarGen = getDataTTbar("trainingTuple_TTbarSingleLepT_0_division_2_TTbarSingleLepT_test_0.h5", "trainingTuple_TTbarSingleLepTbar_0_division_2_TTbarSingleLepTbar_test_0.h5")

print "CREATING HISTOGRAMS"

#input variable histograms
inputLabels = dataTTbar.as_matrix(["genConstiuentMatchesVec", "genTopMatchesVec"])
genMatches = (inputLabels[:,0] == 3) & (inputLabels[:,1] == 1)

genTopData = dataTTbar[genMatches == 1]
genBGData = dataTTbar[genMatches != 1]
minTTbar = dataTTbar.min()
maxTTbar = dataTTbar.max()

varsname = ['cand_dThetaMin', 'cand_dThetaMax', 'cand_dRMax', 'cand_eta', 'cand_m',
 'cand_phi', 'cand_pt', 'cand_p', 'dR12_lab', 'dR13_lab', 'dR1_23_lab',
 'dR23_lab', 'dRPtTop', 'dRPtW', 'dTheta12',
 'dTheta13', 'dTheta23', 'j12_m', 'j12_m_lab', 'j13_m', 'j13_m_lab', 'j23_m',
 'j23_m_lab', 'sd_n2', 'j1_p_top', 'j1_theta_top', 'j1_phi_top',
 'j2_p_top', 'j2_theta_top', 'j2_phi_top', 'j3_p_top', 'j3_theta_top',
 'j3_phi_top', 'j1_qgLikelihood', 'j1_qgPtD',
 'j1_qgAxis2', 'j1_recoJetschargedHadronEnergyFraction',
 'j1_recoJetschargedEmEnergyFraction', 'j1_recoJetsneutralEnergyFraction',
 'j1_qgMult', 'j1_CSV', 'j1_CSV_lab', 'j1_QGL', 'j1_QGL_lab', 'j1_eta_lab',
 'j1_m', 'j1_m_lab', 'j1_p', 'j1_phi_lab', 'j1_pt_lab',
 'j1_qgAxis2_lab', 'j1_qgMult_lab', 'j1_qgPtD_lab',
 'j2_qgLikelihood', 'j2_qgPtD',
 'j2_qgAxis2', 'j2_recoJetschargedHadronEnergyFraction',
 'j2_recoJetschargedEmEnergyFraction',
 'j2_recoJetsneutralEnergyFraction',
 'j2_qgMult', 'j2_CSV', 'j2_CSV_lab', 'j2_QGL', 'j2_QGL_lab', 'j2_eta_lab',
 'j2_m', 'j2_m_lab', 'j2_p', 'j2_phi_lab', 'j2_pt_lab',
 'j2_qgAxis2_lab', 'j2_qgMult_lab', 'j2_qgPtD_lab',
 'j3_qgLikelihood', 'j3_qgPtD',
 'j3_qgAxis2', 'j3_recoJetschargedHadronEnergyFraction',
 'j3_recoJetschargedEmEnergyFraction',
 'j3_recoJetsneutralEnergyFraction',
 'j3_qgMult', 'j3_CSV', 'j3_CSV_lab', 'j3_QGL', 'j3_QGL_lab', 'j3_eta_lab',
 'j3_m', 'j3_m_lab', 'j3_p', 'j3_phi_lab', 'j3_pt_lab', 'j3_qgAxis1_lab',
 'j3_qgAxis2_lab', 'j3_qgMult_lab', 'j3_qgPtD_lab']

for var in varsname:
  try:
    print var
    plt.clf()
    bins = numpy.linspace(minTTbar[var], maxTTbar[var], 21)
    ax = genTopData.hist(column=var, weights=genTopData["sampleWgt"],  bins=bins, grid=False, normed=True, fill=False, histtype='step', label="gen top")
    genBGData      .hist(column=var, weights=genBGData["sampleWgt"],   bins=bins, grid=False, normed=True, fill=False, histtype='step', label="gen bkg", ax=ax)
    plt.legend()
    plt.xlabel(var)
    plt.ylabel("Normalized events")
    plt.yscale('log')
    plt.savefig(outputDirectory + var + ".png")
    plt.close()
  except UnboundLocalError:
    pass
  except ValueError:
    pass
