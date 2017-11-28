#include "SimpleAnalyzer.h"
#include "samples.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include<iostream>
#include<fstream>

bool endsWith (std::string const &fullString, std::string const &extension) {
    if (fullString.size() >= extension.size()) {
        return (0 == fullString.compare (fullString.size() - extension.size(), extension.size(), extension));
    } else {
        return false;
    }
}

int main(int argc, char *argv[])
{
    // Get information about samples
    AnaSamples::SampleSet        ss(AnaSamples::fileDir);
    AnaSamples::SampleCollection sc(ss);

    double weight = 1;
    TChain* ch = new TChain( "slimmedTuple" ) ;

    bool isROOT = false;
    bool isTXT = false;
    if (argc == 1)
    {
        std::cout << "Please provide a sample name, filename (ending with .root), or filelist (ending with .txt) to run over" << std::endl;
        return 0;
    }
    std::string infile = argv[1];
    if(endsWith(infile, ".root"))
        isROOT = true;
    else if(endsWith(infile, ".txt"))
        isTXT = true;
    std::string dataset = infile;

    std::string outfile = "mytest.root";
    std::string outdir = ".";
    if(!(isTXT || isROOT))
        outfile = dataset+".root";
    if (argc > 2)
    {
        outdir = argv[2];
    } 
    if (argc > 3)
    {
        outfile = argv[3];
    } 
    std::string fullpath = outdir + "/" + outfile;
    TFile* myfile = TFile::Open(fullpath.c_str(), "RECREATE");


    // check if sample name, filename, or filelist
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
        t.Loop(weight);
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
            t.Loop(weight);
        }
        t.WriteHistos();
    }

    myfile->Close();

    return 0;
}
