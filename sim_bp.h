#ifndef SIM_BP_H
#define SIM_BP_H

#include <vector>

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1;
    unsigned long int M2;
    unsigned long int N;
    char*             bp_name;
}bp_params;

class PredStats {
	private:
		int 	n_pred;
		int 	n_mispred;
		float	mispred_rate;
	
	public:
		PredStats() {
			n_pred = 0;
			n_mispred = 0;
			mispred_rate = 0;
		}
		
		int getNPred() {return n_pred;}
		int getNMispred() {return n_mispred;}
		float getMispredRate() {return mispred_rate;}
		void incrNPred() {n_pred++;}
		void incrNMispred() {n_mispred++;}
		void setMispredRate() {mispred_rate = (float) n_mispred*100/n_pred;}
};

class Predictor {
	private:
		// Branch History Table: vector of 2-bit counters
		std::vector<unsigned int> bht;
		unsigned long int m;
		unsigned long int size;
		bool en_gshare;
		// Global branch history: n-bit number
		unsigned long int bhr;
		unsigned long int bhr_rev;
		unsigned long int n;

	public:
		// Constructors
		Predictor(unsigned long int _m);						// bimodal
		Predictor(unsigned long int _m, unsigned long int _n);	// gshare

		PredStats stats;

		void branchPred (unsigned long int addr, char outcome);
		char getPred (unsigned long int index);
		unsigned long int getIndex (unsigned long int addr);		// bimodal
		unsigned long int getIndex (unsigned long int addr, bool en_gshare);	// gshare
		bool predCmp (char pred, char outcome);
		void updBHT (char outcome, unsigned long int index);
		void updBHR (char outcome);
		unsigned long int getSize () {return size;}
		unsigned int getBHT (unsigned long int index) {return bht[index];}

};

class HybridPred {
	private:
		unsigned long int k;
		unsigned long int size;
		std::vector<unsigned int> chooser;
		Predictor* bimodal;
		Predictor* gshare;
	
	public:
		HybridPred (unsigned long int _m1, unsigned long int _m2, unsigned long int _n, unsigned long int _k);

		PredStats stats;

		void branchPred (unsigned long int addr, char outcome);
		unsigned long int getIndex (unsigned long int addr);
		bool choose (unsigned long int index);
		void updChooser (bool bimodal_correct, bool gshare_correct, unsigned long int index);
		unsigned long int getChooserSize () {return size;}
		unsigned long int getGshareSize () {return gshare->getSize();}
		unsigned long int getBimodalSize () {return bimodal->getSize();}
		unsigned int getChooser (unsigned long int index) {return chooser[index];}
		unsigned int getGshareBHT (unsigned long int index) {return gshare->getBHT(index);}
		unsigned int getBimodalBHT (unsigned long int index) {return bimodal->getBHT(index);}
		
};

void printResults (Predictor* bp, HybridPred* hp, char* bp_name);

#endif

