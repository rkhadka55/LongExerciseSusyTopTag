#include "SimpleAnalyzer.h"
#include "samples.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include<iostream>
#include<fstream>
#include<getopt.h>

bool endsWith (std::string const &fullString, std::string const &extension) {
    if (fullString.size() >= extension.size()) {
        return (0 == fullString.compare (fullString.size() - extension.size(), extension.size(), extension));
    } else {
        return false;
    }
}

int main(int argc, char *argv[])
{
    // Parse command line arguments
    int opt;
    int option_index = 0;
    static struct option long_options[] = {
        {"input",       required_argument, 0, 'I'}, // sample name, filelist, or root file name
        {"numEvts",     required_argument, 0, 'N'}, // run over fewer events, mainly for testing purposes
        {"outputdir",   required_argument, 0, 'D'}, // where to put the output files
        {"outputfile",  required_argument, 0, 'F'}, // automatically formed from input if you provide a sample name instead of a filename or filelist
        {"weight",      required_argument, 0, 'W'}, // not needed when input is a sample name
    };

    std::string infile = "";
    int maxevents = -1;
    std::string outfile = "mytest.root";
    std::string outdir = ".";
    double weight = 1.;

    while((opt = getopt_long(argc, argv, "D:N:I:F:", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
        case 'I':
            infile = optarg;
            break;

        case 'N':
            maxevents = int(atoi(optarg));
            break;

        case 'D':
            outdir = optarg;
            break;

        case 'F':
            outfile = optarg;
            break;

        case 'W':
            weight = atof(optarg);
            break;
        }
    }


    // Get information about samples
    AnaSamples::SampleSet        ss(AnaSamples::fileDir);
    AnaSamples::SampleCollection sc(ss);

    TChain* ch = new TChain( "slimmedTuple" ) ;

    // Figure out what the input type is
    bool isROOT = false;
    bool isTXT = false;
    if (infile=="")
    {
        std::cout << "Please provide a sample name, filename (ending with .root), or filelist (ending with .txt) to run over. (Use option -I)" << std::endl;
        return 0;
    }
    if(endsWith(infile, ".root"))
        isROOT = true;
    else if(endsWith(infile, ".txt"))
        isTXT = true;
    std::string dataset = infile;

    if(!(isTXT || isROOT))
        outfile = dataset+".root";
    std::string fullpath = outdir + "/" + outfile;
    TFile* myfile = TFile::Open(fullpath.c_str(), "RECREATE");

    if(isROOT || isTXT)
    {
        if(isROOT)
        {
            std::cout << "Reading file " << infile << std::endl;
            ch->Add(infile.c_str());
        }
        else if(isTXT)
        {
            std::cout << "Making TChain from filelist " << infile << std::endl;

            std::ifstream ifs_files;
            ifs_files.open(infile);
            while(ifs_files.good()) 
            {
                TString tsline ;
                tsline.ReadLine( ifs_files ) ;
                if ( !ifs_files.good() ) break ;
                char fname[1000] ;
                sscanf( tsline.Data(), "%s", fname ) ;
                ch -> Add( fname ) ;            
            }
        }
        std::cout << "Chain has " << ch->GetEntries() << " entries." << std::endl;

        SimpleAnalyzer t = SimpleAnalyzer(ch);
        std::cout << "Starting loop" << std::endl;
        t.InitHistos();
        t.Loop(weight, maxevents);
        t.WriteHistos();
    }
    else
    {
        std::cout << "Running over sampleset " << dataset << std::endl;
        std::vector<AnaSamples::FileSummary> fs = sc[dataset];
        std::cout << "Found " << fs.size() << " file summaries" << std::endl;
        SimpleAnalyzer t = SimpleAnalyzer(ch);
        t.InitHistos();
        for (auto f : fs)
        {
            TChain* new_ch = new TChain("slimmedTuple");
            t.Init(new_ch);
            f.addFilesToChain(new_ch);
            //std::cout << "Chain has " << new_ch->GetEntries() << " entries." << std::endl;
            weight = f.getWeight();
            std::cout << "Starting loop for " << f.tag << std::endl;
            t.Loop(weight, maxevents, f.isFastSim_);
        }
        t.WriteHistos();
    }

    myfile->Close();

    return 0;
}
