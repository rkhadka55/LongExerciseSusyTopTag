#include "SimpleAnalyzer.h"
#include "TFile.h"
#include "TTree.h"
#include<iostream>


int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Please provide a filename to run over" << std::endl;
        return 0;
    }
    // Read file
    std::string infile = argv[1];
    std::cout << "Reading file " << infile << std::endl;

    TFile* myinfile = TFile::Open(infile.c_str());
    TTree* mytree = (TTree*) myinfile->Get("slimmedTuple");

    std::string outfile = "mytest.root";
    if (argc > 2)
    {
        outfile = argv[2];
    } 

    TFile* myfile = TFile::Open(outfile.c_str(), "RECREATE");

    SimpleAnalyzer t = SimpleAnalyzer(mytree);
    t.Loop();

    myfile->Close();

    return 0;
}
