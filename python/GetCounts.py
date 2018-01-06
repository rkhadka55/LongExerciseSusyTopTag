import sys, os, errno, numpy, math, optparse
import ROOT
import pandas as pd
from uncertainties import ufloat

def run_checks(options):
	if options.input_file_path != None: 
		if options.input_file_path[-1] != "/": # check for /
			options.input_file_path += "/"

	if options.datacard_templates != None: 
		if options.datacard_templates[-1] != "/": # check for /
			options.datacard_templates += "/"

	outputDirectory = ""
	if len(options.output_file_path): # check is output file path is set
	  outputDirectory = options.output_file_path + "/"
	  try:
		  os.mkdir(outputDirectory) # make the directory if necessary
	  except OSError as exc:
		  if exc.errno == errno.EEXIST and os.path.isdir(outputDirectory):
			  pass
		  else:
			  raise

	if len(options.samples)==0: # check if a user defined list of samples is set
		options.samples = ["Data_MET",
						   "ttbarW",
						   "ZJetsToNuNu",
						   "QCD",
						   "TTZ",
						   "Rare",
						   "Signal_fastsim_T2tt_mStop-1000",
						   "Signal_fastsim_T2tt_mStop-1100",
						   "Signal_fastsim_T2tt_mStop-1200",
						   "Signal_fastsim_T2tt_mStop-800",
						   "Signal_fastsim_T2tt_mStop-900",
						   "Signal_fastsim_T1tttt_mGluino-1700",
						   "Signal_fastsim_T1tttt_mGluino-1800",
						   "Signal_fastsim_T1tttt_mGluino-1900",
						   "Signal_fastsim_T1tttt_mGluino-2000",
						   "Signal_fastsim_T1tttt_mGluino-2100"
						   ]

	return options, outputDirectory

def get_aliases(samples):
	processDict = {#Sample Name `						  ("alias",  is signal, mass)
				  "Data_MET"							: ("obs",    False,     -1),
				  "ttbarW"								: ("ttbarW", False,     -1),
				  "ZJetsToNuNu"							: ("zinv",   False,     -1),
				  "QCD"									: ("qcd",    False,     -1),
				  "TTZ"									: ("ttz",    False,     -1),
				  "Rare"								: ("rare",   False,     -1),
				  "Signal_fastsim_T2tt_mStop-1000"		: ("T2tt",   True,      1000),
				  "Signal_fastsim_T2tt_mStop-1100"		: ("T2tt",   True,      1100),
				  "Signal_fastsim_T2tt_mStop-1200"		: ("T2tt",   True,      1200),
				  "Signal_fastsim_T2tt_mStop-800"		: ("T2tt",   True,      800),
				  "Signal_fastsim_T2tt_mStop-900"		: ("T2tt",   True,      900),
				  "Signal_fastsim_T1tttt_mGluino-1700"	: ("T1tttt", True,      1700),
				  "Signal_fastsim_T1tttt_mGluino-1800"	: ("T1tttt", True,      1800),
				  "Signal_fastsim_T1tttt_mGluino-1900"	: ("T1tttt", True,      1900),
				  "Signal_fastsim_T1tttt_mGluino-2000"	: ("T1tttt", True,      2000),
				  "Signal_fastsim_T1tttt_mGluino-2100"	: ("T1tttt", True,      2100)
				  }
	aliases = []
	for s in samples:
		aliases.append(processDict[s])
	return aliases

def get_input_histograms(samples, input_file_path):
	files = []
	histograms = []
	for isample,sample in enumerate(samples):
		files.append(ROOT.TFile.Open(input_file_path+sample+".root","READ")) 
		histograms.append((files[isample].Get("counts"),files[isample].Get("weight_sq")))
		#print type(histograms[isample][0])
		#print histograms[isample][0].GetName()
	return histograms

def get_avg_weights(histograms):
	avg_weight_histograms = []
	for h in histograms:
		#print type(h[0])
		#print type(h[1])
		#print h[0].GetName()
		avg_weight = h[1].Clone("avgWeight")
		avg_weight.Divide(h[0])
		avg_weight_histograms.append(avg_weight)
	return avg_weight_histograms

def get_counts(histograms,avg_weight_histograms):
	count_histograms = []
	for ih,h in enumerate(histograms):
		unweighted_count = h[0].Clone("unweighted_counts")
		unweighted_count.Divide(avg_weight_histograms[ih])
		count_histograms.append(unweighted_count)
	return count_histograms

def histograms_to_lists_by_bin(aliases,unweighted_count_histograms,avg_weight_histograms):
	counts_by_bins = {}
	unc_by_bins = {}
	weight_by_bins = {}
	for b in range(2,7):
		counts_by_bins[b-1] = pd.Series([ufloat(h.GetBinContent(b),h.GetBinError(b)) for h in unweighted_count_histograms], index=[a[0] if a[1]==False else a[0]+","+str(a[2]) for a in aliases])
		unc_by_bins[b-1] = pd.Series([h.GetBinError(b) for h in unweighted_count_histograms], index=[a[0] if a[1]==False else a[0]+","+str(a[2]) for a in aliases])
		weight_by_bins[b-1] = pd.Series([h.GetBinContent(b) for h in avg_weight_histograms], index=[a[0] if a[1]==False else a[0]+","+str(a[2]) for a in aliases])
	return (counts_by_bins,unc_by_bins,weight_by_bins)

def make_dataframes(aliases,counts_by_bins,unc_by_bins,weight_by_bins):
	pd.set_option("display.width",120)
	pd.set_option("display.float_format",'{:0.5f}'.format)
	counts = pd.DataFrame(counts_by_bins)
	counts_for_display = pd.DataFrame(counts_by_bins)
	weights = pd.DataFrame(weight_by_bins)
	fmter = lambda x: '{:.5u}'.format(x)
	for col in counts_for_display:
		counts_for_display[col] = counts_for_display[col].apply(fmter)
	print counts_for_display
	print weights
	return counts,weights

def make_datacards(templates,outputDirectory,dataframes):
	for bin in range (1,6):
		outf = open(outputDirectory+'input_ch%i.txt' % bin,'w')
		with open(templates+'input_ch%i.txt' % bin,'r') as inf:
			lines = inf.readlines()
			for line in lines:
				tmp = line.strip('\n').split(',')
				if tmp[0]=='T1tttt' or tmp[0]=='T2tt':
					tmp[0:2] = [','.join(tmp[0:2])]
				if tmp[0] in dataframes[0].index.values.tolist():
					current_index = dataframes[0].index.values.tolist().index(tmp[0])
					if tmp[0]=="obs":
						#print dataframes[0].index[current_index]+','+str(dataframes[0][bin][tmp[0]].nominal_value)
						outf.write(str(dataframes[0].index[current_index])+','+str(dataframes[0][bin][tmp[0]].nominal_value)+'\n')
					else:
						#print str(dataframes[0].index[current_index])+','+str(dataframes[0][bin].ix[current_index].nominal_value)+','+str(dataframes[1][bin].ix[current_index])+','+str(tmp[-1])
						outf.write(str(dataframes[0].index[current_index])+','+str(dataframes[0][bin].ix[current_index].nominal_value)+','+str(dataframes[1][bin].ix[current_index])+','+str(tmp[-1])+'\n')
		outf.close()

def main(options):
	options, outputDirectory = run_checks(options)
	print "Getting histograms ..."
	ROOT.TH1.AddDirectory(False)
	histograms = get_input_histograms(options.samples, options.input_file_path)
	print "Getting average weights ..."
	avg_weight_histograms = get_avg_weights(histograms)
	print "Getting unweighted counts ..."
	unweighted_count_histograms = get_counts(histograms,avg_weight_histograms)
	#stat_unc_histograms = get_stat_unc()
	print "Converting histograms to dictionaries of lists ..."
	counts_by_bins,unc_by_bins,weight_by_bins = histograms_to_lists_by_bin(get_aliases(options.samples),unweighted_count_histograms,avg_weight_histograms)
	print "Making dataframes and printing them ..."
	dataframes = make_dataframes(get_aliases(options.samples),counts_by_bins,unc_by_bins,weight_by_bins)
	make_datacards(options.datacard_templates,outputDirectory,dataframes)

if __name__ == '__main__':
	parser = optparse.OptionParser("usage: %prog [options]\n")
	parser.add_option ('-s', "--samples",            dest='samples',            action='store', default=[], type=str, nargs='+', help="List of sample names")
	parser.add_option ('-i', "--input_file_path",    dest="input_file_path",    action='store', default="myhistos/",             help="Path where the input datafiles are stored")
	parser.add_option ('-o', "--output_file_path",   dest="output_file_path",   action='store', default="myhistos_updated/",     help="Path where the input datafiles are stored")
	parser.add_option ('-t', "--datacard_templates", dest='datacard_templates', action='store', default="limit/cards/",          help="If provided, will use these cards as templates to make updated cards")
	options, args = parser.parse_args() 

	main(options)
