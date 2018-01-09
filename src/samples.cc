#include "samples.h"

#include <iostream>
#include <cstdio>
#include <cstring>

namespace AnaSamples
{
    void FileSummary::readFileList() const
    {
        if(filelist_.size()) filelist_.clear();
        
        FILE *f = fopen(filePath.c_str(), "r");
        char buff[512];
        if(f)
        {
            while(!feof(f) && fgets(buff, 512, f))
            {
                for(char* k = strchr(buff, '\n'); k != 0; k = strchr(buff, '\n')) *k = '\0';
                filelist_.push_back(buff);
            }
            fclose(f);
        }
        else std::cout << "Filelist file \"" << filePath << "\" not found!!!!!!!" << std::endl;
    }

    void FileSummary::addCollection(std::string colName)
    {
        collections_.insert(colName);
    }

    std::map<std::string, FileSummary>& SampleSet::getMap()
    {
        return sampleSet_;
    }
    
    SampleSet::SampleSet(std::string fDir, double lumi) : fDir_(fDir), lumi_(lumi)
    {
        // ---------------
        // - backgrounds -
        // ---------------

        // branching ratio info from PDG
        double W_Lept_BR = 0.1086*3;
        double W_Had_BR = 1 - W_Lept_BR;
        double TTbar_SingleLept_BR = 0.43930872; // 2*W_Lept_BR*(1-W_Lept_BR)
        double TTbar_DiLept_BR = 0.10614564; // W_Lept_BR^2

        if(fDir.compare("condor") == 0)
        {
            fDir_ = "";
        }

        //TTbar samples
        // TTbarInc has LO xsec on McM : 502.20 pb. The NNLO is 831.76 pb. The k-factor for ttbar is: kt = 831.76/502.20 ~ 1.656233    
        // Calculated from PDG BRs'. Not from the kt * xSec in McM
        addSample("TTbarDiLep",         "data/filelists/TTbarDiLep.txt",         "slimmedTuple", 831.76*TTbar_DiLept_BR,         lumi, 30444678, 1.0, kGreen);
        addSample("TTbarSingleLepT",    "data/filelists/TTbarSingleLepT.txt",    "slimmedTuple", 831.76*0.5*TTbar_SingleLept_BR, lumi, 61901450, 1.0, kGreen);
        addSample("TTbarSingleLepTbar", "data/filelists/TTbarSingleLepTbar.txt", "slimmedTuple", 831.76*0.5*TTbar_SingleLept_BR, lumi, 59860282, 1.0, kGreen);


        // From https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#W_jets, kw = 1.21
        //addSample("WJetsToLNu_HT_70to100",    "data/filelists/WJetsToLNu_HT_70to100.txt",    "slimmedTuple", 1319,    lumi, 10034066,  1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_100to200",   "data/filelists/WJetsToLNu_HT_100to200.txt",   "slimmedTuple", 1345,    lumi, 79065975,  1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_200to400",   "data/filelists/WJetsToLNu_HT_200to400.txt",   "slimmedTuple", 359.7,   lumi, 38867206,  1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_400to600",   "data/filelists/WJetsToLNu_HT_400to600.txt",   "slimmedTuple", 48.91,   lumi, 7759701,   1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_600to800",   "data/filelists/WJetsToLNu_HT_600to800.txt",   "slimmedTuple", 12.05,   lumi, 17494743,  1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_800to1200",  "data/filelists/WJetsToLNu_HT_800to1200.txt",  "slimmedTuple", 5.501,   lumi, 7745467,   1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_1200to2500", "data/filelists/WJetsToLNu_HT_1200to2500.txt", "slimmedTuple", 1.329,   lumi, 6801534,   1.21, kMagenta+1);
        addSample("WJetsToLNu_HT_2500toInf",  "data/filelists/WJetsToLNu_HT_2500toInf.txt",  "slimmedTuple", 0.03216, lumi, 2637821,   1.21, kMagenta+1);
        
        //Z -> nunu
        // From https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#DY_Z, kz = 1.23
        addSample("ZJetsToNuNu_HT_100to200",   "data/filelists/ZJetsToNuNu_HT_100to200.txt",   "slimmedTuple", 280.35,    lumi, 24121326, 1.23*0.856710,  kTeal+4);
        addSample("ZJetsToNuNu_HT_200to400",   "data/filelists/ZJetsToNuNu_HT_200to400.txt",   "slimmedTuple", 77.67,     lumi, 24663714, 1.23*0.856710,  kTeal+4);
        addSample("ZJetsToNuNu_HT_400to600",   "data/filelists/ZJetsToNuNu_HT_400to600.txt",   "slimmedTuple", 10.73,     lumi, 9862869,  1.23*0.856710,  kTeal+4);
        addSample("ZJetsToNuNu_HT_600to800",   "data/filelists/ZJetsToNuNu_HT_600to800.txt",   "slimmedTuple", 0.853*3,   lumi, 5611895,  1.23*0.856710,  kTeal+4);
        addSample("ZJetsToNuNu_HT_800to1200",  "data/filelists/ZJetsToNuNu_HT_800to1200.txt",  "slimmedTuple", 0.394*3,   lumi, 2100324,  1.23*0.856710,  kTeal+4);
        addSample("ZJetsToNuNu_HT_1200to2500", "data/filelists/ZJetsToNuNu_HT_1200to2500.txt", "slimmedTuple", 0.0974*3,  lumi, 513471,   1.23*0.856710,  kTeal+4);
        addSample("ZJetsToNuNu_HT_2500toInf",  "data/filelists/ZJetsToNuNu_HT_2500toInf.txt",  "slimmedTuple", 0.00230*3, lumi, 405030,   1.23*0.856710,  kTeal+4);
       
        //QCD
        // Ref. https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns#QCD. But numbers are from McM.
        addSample("QCD_HT100to200"  , "data/filelists/QCD_HT100to200.txt"  ,"slimmedTuple", 27540000, lumi, 80547699, 1.0,  kBlue);
        addSample("QCD_HT200to300"  , "data/filelists/QCD_HT200to300.txt"  ,"slimmedTuple", 1735000 , lumi, 56502428, 1.0,  kBlue);
        addSample("QCD_HT300to500"  , "data/filelists/QCD_HT300to500.txt"  ,"slimmedTuple", 366800  , lumi, 54479109, 1.0,  kBlue);
        addSample("QCD_HT500to700"  , "data/filelists/QCD_HT500to700.txt"  ,"slimmedTuple", 29370   , lumi, 62271343, 1.0,  kBlue);
        addSample("QCD_HT700to1000" , "data/filelists/QCD_HT700to1000.txt" ,"slimmedTuple", 6524    , lumi, 45058463, 1.0,  kBlue);
        addSample("QCD_HT1000to1500", "data/filelists/QCD_HT1000to1500.txt","slimmedTuple", 1064    , lumi, 15064562, 1.0,  kBlue);
        addSample("QCD_HT1500to2000", "data/filelists/QCD_HT1500to2000.txt","slimmedTuple", 121.5   , lumi, 11826702, 1.0,  kBlue);
        addSample("QCD_HT2000toInf" , "data/filelists/QCD_HT2000toInf.txt" ,"slimmedTuple", 25.42   , lumi,  6039005, 1.0,  kBlue);


        //Other Samples
        // Aprox. NNLO
        addSample("tW_top_NoHad",     "data/filelists/tW_top_NoHad.txt",     "slimmedTuple", 35.6*(1-W_Had_BR*W_Had_BR), lumi, 11345619,  1.0,  kYellow);
        addSample("tW_antitop_NoHad", "data/filelists/tW_antitop_NoHad.txt", "slimmedTuple", 35.6*(1-W_Had_BR*W_Had_BR), lumi, 11408144,  1.0,  kYellow);

        addSample("ST_s",         "data/filelists/ST_s.txt",         "slimmedTuple", 10.32,  lumi, 811495 - 188505,  1.0,  kYellow);
        addSample("ST_t_top",     "data/filelists/ST_t_top.txt",     "slimmedTuple", 136.02, lumi, 66559176,  1.0,  kYellow);
        addSample("ST_t_antitop", "data/filelists/ST_t_antitop.txt", "slimmedTuple", 80.95,  lumi, 37834984,  1.0,  kYellow);

        addSample("tZq_ll",    "data/filelists/tZq_ll.txt",    "slimmedTuple", 0.0758,  lumi, 9177845 - 5331675,  1.0,  kYellow);
        addSample("ST_tWll",   "data/filelists/ST_tWll.txt",   "slimmedTuple", 0.01123, lumi, 50000,  1.0,  kYellow);
        addSample("ST_tWnunu", "data/filelists/ST_tWnunu.txt", "slimmedTuple", 0.02134, lumi, 100000,  1.0,  kYellow);


        // NLO --> negative weights!
        // (sign of gen weight) * (lumi*xsec)/(effective number of events): effective number of events = N(evt) with positive weight - N(evt) with negative weight
        addSample("TTZToLLNuNu", "data/filelists/TTZToLLNuNu.txt", "slimmedTuple", 0.2529, lumi, 1416634 - 516458,  1.0,  kOrange+2);
        addSample("TTZToQQ",     "data/filelists/TTZToQQ.txt",          "slimmedTuple", 0.5297, lumi, 435723 - 157814,  1.0,  kOrange+2);

        // NLO --> negative weights!
        addSample("TTWJetsToLNu", "data/filelists/TTWJetsToLNu.txt", "slimmedTuple", 0.2043, lumi, 3998407 - 1282158,   1.0,  kSpring+2);
        addSample("TTWJetsToQQ",  "data/filelists/TTWJetsToQQ.txt",  "slimmedTuple", 0.4062, lumi, 631804 - 201494,  1.0,  kSpring+2);

        addSample("TTTT",  "data/filelists/TTTT.txt",  "slimmedTuple", 0.009103, lumi, 177320 - 72680,  1.0,  kSpring+2);

        // NLO --> negative weights!  
        addSample("TTGJets", "data/filelists/TTGJets.txt", "slimmedTuple", 3.697, lumi, 3224372 - 1646539,  1.0,  kOrange+2);

        // ttH 
        addSample("ttHTobb",    "data/filelists/ttHTobb.txt",   "slimmedTuple", 0.2934,  lumi, 3809292 - 36700,   1.0,  kOrange+2);
        addSample("ttHToNonbb", "data/filelists/ttHToNonbb.txt", "slimmedTuple", 0.2151,  lumi, 3943524 - 37726,  1.0,  kOrange+2);

        // Di-boson
        // Ref. https://indico.cern.ch/event/439995/session/0/contribution/6/attachments/1143460/1638648/diboson_final.pdf (NNLO is given)
        addSample("WWTo4Q", "data/filelists/WWTo4Q.txt", "slimmedTuple", 51.723,  lumi, 1998400,  1.0,  kViolet+4); 
        addSample("WWTo2L2Nu", "data/filelists/WWTo2L2Nu.txt", "slimmedTuple", 12.178,  lumi, 1999000,  1.0,  kViolet+4); 
        addSample("WWToLNuQQ", "data/filelists/WWToLNuQQ.txt", "slimmedTuple", 49.997,  lumi, 8997800,  1.0,  kViolet+4); 
        
        // Ref. https://twiki.cern.ch/twiki/bin/viewauth/CMS/SummaryTable1G25ns (NLO from MCFM)
        addSample("WZ", "data/filelists/WZ.txt", "slimmedTuple", 47.13,  lumi, 3995828,  1.0,  kViolet+4);

        //addSample("ZZ", "data/filelists/ZZ_TuneCUETP8M1_13TeV-pythia8.txt", "slimmedTuple", 16.523, lumi, 989312,  1.0,  kViolet+4);
        addSample("ZZTo2Q2Nu", "data/filelists/ZZTo2Q2Nu.txt", "slimmedTuple", 4.04, lumi, 23902489 - 5716189,  1.0,  kViolet+4);
        addSample("ZZTo2L2Nu", "data/filelists/ZZTo2L2Nu.txt", "slimmedTuple", 0.564, lumi, 8842475,  1.0,  kViolet+4);
        addSample("ZZTo2L2Q", "data/filelists/ZZTo2L2Q.txt", "slimmedTuple", 3.22, lumi, 12038933 - 2720494,  1.0,  kViolet+4);
        addSample("ZZTo4Q", "data/filelists/ZZTo4Q.txt", "slimmedTuple"   , 6.842, lumi, 22590415 - 5321419,  1.0,  kViolet+4);
        addSample("ZZTo4L", "data/filelists/ZZTo4L.txt", "slimmedTuple", 1.256, lumi, 6669988,  1.0,  kViolet+4);

        // Tri-boson: negative weights!
        addSample("WWW", "data/filelists/WWW.txt", "slimmedTuple", 0.2086,  lumi, 225269 - 14731,  1.0,  kViolet+2);
        addSample("WWZ", "data/filelists/WWZ.txt", "slimmedTuple", 0.1651,  lumi, 235734 - 14266,  1.0,  kViolet+2);
        addSample("WZZ", "data/filelists/WZZ.txt", "slimmedTuple", 0.05565, lumi, 231583 - 15217,  1.0,  kViolet+2);
        addSample("ZZZ", "data/filelists/ZZZ.txt", "slimmedTuple", 0.01398, lumi, 231217 - 18020,  1.0,  kViolet+2);
        addSample("WZG", "data/filelists/WZG.txt", "slimmedTuple", 0.04123, lumi, 921527 - 76673,  1.0,  kViolet+2);
        addSample("WWG", "data/filelists/WWG.txt", "slimmedTuple", 0.2147 , lumi, 913515 - 85885,  1.0,  kViolet+2);

        // --------
        // - data -
        // --------
        addSample("Data_MET_2016", "data/filelists/Data_MET_2016.txt","slimmedTuple",  (5787968233.150 + 2573399420.069 + 4248383597.366 + 4008663475.924 + 3101618402.006 + 7540487746.602 + 8390540442.688 + 215149415.251)/1000000., 1.0,  kBlack);


        // ----------
        // - signal -
        // ----------
        
        // T2tt Number cross lumi number of events 
        addSample("SMS-T2tt_fastsim_mStop-800",  "data/filelists/SMS-T2tt_fastsim_mStop-800.txt",  "slimmedTuple", 0.0283338, lumi, 18533.0, 1.0, kRed, true);
        addSample("SMS-T2tt_fastsim_mStop-900",  "data/filelists/SMS-T2tt_fastsim_mStop-900.txt",  "slimmedTuple", 0.0128895 , lumi, 19667.0, 1.0, kRed, true);
        addSample("SMS-T2tt_fastsim_mStop-1000", "data/filelists/SMS-T2tt_fastsim_mStop-1000.txt", "slimmedTuple", 0.00615134, lumi, 19536.0, 1.0, kRed, true);
        addSample("SMS-T2tt_fastsim_mStop-1100", "data/filelists/SMS-T2tt_fastsim_mStop-1100.txt", "slimmedTuple", 0.00307413, lumi, 17516.0, 1.0, kRed, true);
        addSample("SMS-T2tt_fastsim_mStop-1200", "data/filelists/SMS-T2tt_fastsim_mStop-1200.txt", "slimmedTuple", 0.00159844, lumi, 16938.0, 1.0, kRed, true);

        // T1tttt
        addSample("SMS-T1tttt_fastsim_mGluino-1700", "data/filelists/SMS-T1tttt_fastsim_mGluino-1700.txt", "slimmedTuple", 0.00470323, lumi, 20163.0, 1.0, kRed, true);
        addSample("SMS-T1tttt_fastsim_mGluino-1800", "data/filelists/SMS-T1tttt_fastsim_mGluino-1800.txt", "slimmedTuple", 0.00276133, lumi, 19650.0, 1.0, kRed, true);
        addSample("SMS-T1tttt_fastsim_mGluino-1900", "data/filelists/SMS-T1tttt_fastsim_mGluino-1900.txt", "slimmedTuple", 0.00163547, lumi, 16778.0, 1.0, kRed, true);
        addSample("SMS-T1tttt_fastsim_mGluino-2000", "data/filelists/SMS-T1tttt_fastsim_mGluino-2000.txt", "slimmedTuple", 0.000981077, lumi, 21897.0, 1.0, kRed, true);
        addSample("SMS-T1tttt_fastsim_mGluino-2100", "data/filelists/SMS-T1tttt_fastsim_mGluino-2100.txt", "slimmedTuple", 0.000591918 , lumi, 20243.0, 1.0, kRed, true);
    }

    SampleCollection::SampleCollection(SampleSet& samples)
    {
        //Define sets of samples for later use
        addSampleSet(samples, "TTbarSingleLep", {"TTbarSingleLepT", "TTbarSingleLepTbar"});
        addSampleSet(samples, "TTbarDiLep", {"TTbarDiLep"});
        addSampleSet(samples, "TTbarNoHad", {"TTbarSingleLepT", "TTbarSingleLepTbar", "TTbarDiLep"});
        addSampleSet(samples, "ttbarW", {"TTbarSingleLepT", "TTbarSingleLepTbar", "TTbarDiLep","WJetsToLNu_HT_2500toInf", "WJetsToLNu_HT_1200to2500", "WJetsToLNu_HT_800to1200", "WJetsToLNu_HT_600to800", "WJetsToLNu_HT_400to600", "WJetsToLNu_HT_200to400", "WJetsToLNu_HT_100to200", /*"WJetsToLNu_HT_70to100",*/ "tW_top_NoHad", "tW_antitop_NoHad", "ST_s", "ST_t_top", "ST_t_antitop", "tZq_ll", "ST_tWll", "ST_tWnunu"});

        addSampleSet(samples, "WJetsToLNu", {"WJetsToLNu_HT_2500toInf", "WJetsToLNu_HT_1200to2500", "WJetsToLNu_HT_800to1200", "WJetsToLNu_HT_600to800", "WJetsToLNu_HT_400to600", "WJetsToLNu_HT_200to400", "WJetsToLNu_HT_100to200"/*, "WJetsToLNu_HT_70to100"*/});

        addSampleSet(samples, "ZJetsToNuNu", {"ZJetsToNuNu_HT_2500toInf", "ZJetsToNuNu_HT_1200to2500", "ZJetsToNuNu_HT_800to1200", "ZJetsToNuNu_HT_600to800", "ZJetsToNuNu_HT_400to600", "ZJetsToNuNu_HT_200to400", "ZJetsToNuNu_HT_100to200"});

        //addSampleSet(samples, "DYJetsToLL", {"DYJetsToLL_HT_2500toInf", "DYJetsToLL_HT_1200to2500", "DYJetsToLL_HT_800to1200", "DYJetsToLL_HT_600to800", "DYJetsToLL_HT_400to600", "DYJetsToLL_HT_200to400", "DYJetsToLL_HT_100to200", "DYJetsToLL_HT_70to100"});

        addSampleSet(samples, "QCD", {"QCD_HT2000toInf", "QCD_HT1500to2000", "QCD_HT1000to1500", "QCD_HT700to1000", "QCD_HT500to700", "QCD_HT300to500", "QCD_HT200to300"});

        addSampleSet(samples, "ST", {"tW_top_NoHad", "tW_antitop_NoHad", "ST_s", "ST_t_top", "ST_t_antitop", "tZq_ll", "ST_tWll", "ST_tWnunu"});

        addSampleSet(samples, "Diboson", {"WWTo4Q", "WWTo2L2Nu", "WWToLNuQQ", 
                    "WZ", 
                    "ZZTo2Q2Nu", "ZZTo2L2Nu", "ZZTo2L2Q", "ZZTo4Q", "ZZTo4L"});

        addSampleSet(samples, "Rare", {"TTWJetsToLNu", "TTWJetsToQQ", 
                    "TTZToLLNuNu", "TTZToQQ",
                    /*"TTGJets",*/ "TTTT",
                    "WWW", "WWZ", "WZZ", "ZZZ", "WWG", "WZG", 
                    "ttHTobb", "ttHToNonbb",
                    "WWTo4Q", "ZZTo2Q2Nu", "ZZTo4Q"
                    });

        addSampleSet(samples, "TTZ", {"TTZToLLNuNu"});//, "TTZToQQ"});

        addSampleSet(samples, "ALL_MC", {});

        addSampleSet(samples, "Data_MET", {"Data_MET_2016"});

        addSampleSet(samples, "Signal_fastsim_T2tt_mStop-800", {"SMS-T2tt_fastsim_mStop-800"});
        addSampleSet(samples, "Signal_fastsim_T2tt_mStop-900", {"SMS-T2tt_fastsim_mStop-900"});
        addSampleSet(samples, "Signal_fastsim_T2tt_mStop-1000", {"SMS-T2tt_fastsim_mStop-1000"});
        addSampleSet(samples, "Signal_fastsim_T2tt_mStop-1100", {"SMS-T2tt_fastsim_mStop-1100"});
        addSampleSet(samples, "Signal_fastsim_T2tt_mStop-1200", {"SMS-T2tt_fastsim_mStop-1200"});

        addSampleSet(samples, "Signal_fastsim_T1tttt_mGluino-1700", {"SMS-T1tttt_fastsim_mGluino-1700"});
        addSampleSet(samples, "Signal_fastsim_T1tttt_mGluino-1800", {"SMS-T1tttt_fastsim_mGluino-1800"});
        addSampleSet(samples, "Signal_fastsim_T1tttt_mGluino-1900", {"SMS-T1tttt_fastsim_mGluino-1900"});
        addSampleSet(samples, "Signal_fastsim_T1tttt_mGluino-2000", {"SMS-T1tttt_fastsim_mGluino-2000"});
        addSampleSet(samples, "Signal_fastsim_T1tttt_mGluino-2100", {"SMS-T1tttt_fastsim_mGluino-2100"});

    }

// if name contains the keyword "ALL", then:
// ] vss serves as a SKIPPING list and support keyword matching!
// ] if has "MC" --> refer to all fullsim MC
// ] if has "fastsim" --> refer to all fastsim
// ] if has "Data" --> refer to all data
    void SampleCollection::addSampleSet(SampleSet& samples, std::string name, std::vector<std::string> vss)
    {
        if(vss.size() > 1)
        {
            for(std::string& sn : vss)
            {
                if(sn.compare(name) == 0)
                {
                    std::cout << "You have named a sampleCollection the same as one of its member sampleSets, but it has more than one sampleSet!!!! This is bad!!!  Stop!!! Stop now!!!  This collection will be skipped until it is properly named." << std::endl;
                    return;
                }
            }
        }

        auto& map = samples.getMap();

// if keyword "ALL" appears, by-passing the regular adding procedure...
        if( name.find("ALL") != std::string::npos )
        {
           bool incl_fullsim = false;
           bool incl_fastsim = false;
           bool incl_Data = false;
           if( name.find("MC") != std::string::npos ) incl_fullsim = true;
           if( name.find("fastsim") != std::string::npos ) incl_fastsim = true;
           if( name.find("Data") != std::string::npos ) incl_Data = true;
           if( !incl_fullsim && !incl_fastsim && !incl_Data )
           {
              std::cout<<"WARNING ... will not add any samples with your requests ..."<<std::endl;
              return;
           }
           for(auto im : map)
           {
              std::string persn = im.first;
              bool excluded = false;
              for(std::string & exc_sn : vss )
              {
                 if( persn.find(exc_sn) != std::string::npos ){ excluded = true; break; }
              }
              if( excluded ) continue;
              if( !incl_fastsim && persn.find("fastsim") != std::string::npos ) continue;
              if( !incl_Data && persn.find("Data") != std::string::npos ) continue;
              im.second.addCollection(name);
              sampleSet_[name].push_back(im.second);
              nameVec_[name].push_back(im.first);
              totalLumiMap_[name] += im.second.lumi;
           }
           return;
        }

        for(std::string& sn : vss)
        {
            map[sn].addCollection(name);
            sampleSet_[name].push_back(samples[sn]);
            nameVec_[name].push_back(sn);
            totalLumiMap_[name] += samples[sn].lumi;
        }
    }

    std::vector<std::string>& SampleCollection::getSampleLabels(std::string name)
    {
        return nameVec_[name];
    }

    bool operator< (const FileSummary& lhs, const FileSummary& rhs)
    {
        return lhs.filePath < rhs.filePath || lhs.treePath < rhs.treePath;
    }

    bool operator== (const FileSummary& lhs, const FileSummary& rhs)
    {
        return lhs.filePath == rhs.filePath && lhs.treePath == rhs.treePath && lhs.xsec == rhs.xsec && lhs.lumi == rhs.lumi && lhs.kfactor == rhs.kfactor && lhs.nEvts == rhs.nEvts;
    }

    bool operator!= (const FileSummary& lhs, const FileSummary& rhs)
    {
        return !(lhs == rhs);
    }
}
