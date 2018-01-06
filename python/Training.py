import os
import errno
from optparse import OptionParser
from DataGetter import DataGetter
from glob import glob
import json

def mainXGB(options):

  import xgboost as xgb

  print "PROCESSING TRAINING DATA"

  #training variables 
  allVars = ["cand_m", "j12_m", "j13_m", "j23_m"]
  
  #save variables used in training
  fname = "variables.json"
  try:
    with open(options.directory + fname,"w") as f:
      json.dump(allVars, f, sort_keys=True, indent=4)
  except IOError:
    print "Unable to save configuration to",fname
    
  # Import data
  dg = DataGetter(allVars)
  trainData = dg.importData(samplesToRun = tuple(glob(options.dataFilePath + "/trainingTuple_TTbarSingleLepT*_0_division_0_TTbarSingleLepT*_training_0.h5")), prescale=True, ptReweight=False)

  print "TRAINING XGB"

  # Create xgboost classifier
  xgData = xgb.DMatrix(trainData["data"], label=trainData["labels"][:,0], weight=trainData["weights"][:,0])
  param = {'max_depth':5, 'eta':0.03, 'nthread':4 , 'objective':'binary:logistic'}
  gbm = xgb.train(param, xgData, num_boost_round=500)
  
  #Dump output from training
  gbm.save_model(options.directory + 'TrainingModel.xgb')

if __name__ == '__main__':

  #Option parsing 
  parser = OptionParser()

  #We add the following options in order to run things other than tensorflow
  parser.add_option ('-d', "--directory",     dest='directory',     action='store',                    help="Directory to store outputs (default .)")
  parser.add_option ('-f', "--dataFilePath",  dest="dataFilePath",  action='store', default="data",    help="Path where the input datafiles are stored")

  options, args = parser.parse_args()

  #create output directory if it does not already exist 
  if len(options.directory):
    if options.directory[-1] != "/": options.directory += "/"
    try:
      os.mkdir(options.directory)
    except OSError as exc:
      if exc.errno == errno.EEXIST and os.path.isdir(options.directory):
        pass
      else:
        raise

  #run the approperaite main function 
  mainXGB(options)

  print "TRAINING DONE!"
