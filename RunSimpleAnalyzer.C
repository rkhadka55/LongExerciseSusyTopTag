#include "SimpleAnalyzer.h"
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
    if (argc == 1)
    {
        std::cout << "Please provide a filename or filelist to run over" << std::endl;
        return 0;
    }
    // Read file
    std::string infile = argv[1];

    TChain* ch = new TChain( "slimmedTuple" ) ;
    // check if filename or filelist
    if(endsWith(infile, ".root"))
    {
        std::cout << "Reading file " << infile << std::endl;
        ch->Add(infile.c_str());
    }
    else 
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

    std::string outfile = "mytest.root";
    if (argc > 2)
    {
        outfile = argv[2];
    } 

    TFile* myfile = TFile::Open(outfile.c_str(), "RECREATE");

    SimpleAnalyzer t = SimpleAnalyzer(ch);
    std::cout << "Starting loop" << std::endl;
    t.Loop();

    myfile->Close();

    return 0;
}
