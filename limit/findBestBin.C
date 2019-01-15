string makeFileName(string path, string signame, int mass, int bin){
	return path+"/higgsCombine_simpleCard_ch"+to_string(bin)+"_"+signame+"_"+to_string(mass)+".Asymptotic.mH"+to_string(mass)+".root";
}

//usage:
//root -l 'findBestBin.C("cards","T2tt",800,{1,2,3,4,5})'
void findBestBin(string path, string signame, int mass, vector<int> bins){
	double rmin = 1e10;
	double bestbin = -1;
	//loop over bins and find the best central value of r
	for(auto bin : bins){
		//open file
        string fname = makeFileName(path,signame,mass,bin);
		TFile* file = TFile::Open(fname.c_str());
		if(!file) {
			cout << "Couldn't open " << fname << endl;
			continue;
		}

		//get tree
		TTree* limit = (TTree*)file->Get("limit");
		if(!limit) {
			cout << "Couldn't get limit tree from " << fname << endl;
			continue;
		}

		//TTree::Draw() stores quantities in arrays
		int npts = limit->Draw("limit","abs(quantileExpected-0.5)<0.01","goff");
		double* rtmp = limit->GetV1();
		if(npts<=0) {
			cout << "Didn't find any r values!" << endl;
			continue;
		}

		//check against min
		if(rtmp[0] < rmin){
			rmin = rtmp[0];
			bestbin = bin;
		}
	}

	//print result
	if(bestbin!=-1){
       cout << makeFileName(path,signame,mass,bestbin) << endl;
	}
}

//usage:
//	root -l
//	.L findBestBin.C
//	findAll()
void findAll(string path="cards/"){
	vector<string> signames = {"T2tt","T1tttt"};
	for(auto signame : signames){
		vector<int> masses = {};
		if(signame=="T2tt") masses = {800,900,1000,1100,1200};
		else masses = {1700,1800,1900,2000,2100};
		for(auto mass : masses){
            findBestBin(path,signame,mass,{1,2,3,4,5});
		}
	}
}
