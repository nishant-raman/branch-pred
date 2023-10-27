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
		unsigned long int n;

	public:
		// Constructors
		Predictor(unsigned long int _m);						// bimodal
		Predictor(unsigned long int _m, unsigned long int _n);	// gshare

		PredStats stats;

		void branchPred(unsigned long int addr, char outcome);
		char getPred (unsigned long int index);
		unsigned long int getIndex (unsigned long int addr);		// bimodal
		unsigned long int getIndex (unsigned long int addr, unsigned long int bhr);	// gshare
		bool predCmp (char pred, char outcome);
		void updBHT(char outcome, unsigned long int index);
		void updBHR(char outcome);
		unsigned long int getSize () {return size;}
		unsigned int getBHT (unsigned long int index) {return bht[index];}

};

void printResults (Predictor* bp, char* bp_name);

#endif

