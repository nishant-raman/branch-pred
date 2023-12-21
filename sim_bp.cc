#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"

#define DBG 0
#define HASH_SHIFT 1 	// shift bhr left (1) or right (0)
#define HASH_LOC 0 		// xor bhr at low parts of pc or high

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/

using namespace std;

int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file

	Predictor* bp = NULL;
	HybridPred* hp = NULL;
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n %s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
		
		bp = new Predictor(params.M2);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n %s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);
	
		bp = new Predictor(params.M1, params.N);
    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n %s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

		hp = new HybridPred(params.M1, params.M2, params.N, params.K);
    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
	unsigned long int i=0;
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        
        outcome = str[0];
        if (DBG) {
        if (outcome == 't')
            printf("=%lu %lx %s\n", i, addr, "t");           // Print and test if file is read correctly
        else if (outcome == 'n')
            printf("=%lu %lx %s\n", i, addr, "n");          // Print and test if file is read correctly
        }
		i++;
        /*************************************
            Add branch predictor code here
        **************************************/
    	if (strcmp(params.bp_name, "hybrid") == 0)
			hp->branchPred (addr, outcome);
		else
			bp->branchPred (addr, outcome);
		
    }
	
	printResults(bp, hp, params.bp_name);
	
    if (strcmp(params.bp_name, "hybrid") == 0)
		delete hp;
	else
		delete bp;

    return 0;
}


// -------------------------------------------------------------------
// 			Predictor Class function declarations

Predictor::Predictor (unsigned long int _m) : m(_m) {
	size = 1<<_m;
	bht.resize(size, 2);
	en_gshare = false;
}

Predictor::Predictor (unsigned long int _m, unsigned long int _n) : m(_m), n(_n) {
	size = 1<<_m;
	bht.resize(size, 2);
	en_gshare = true;
	bhr = 0;
	bhr_rev = 0;
}

void Predictor::branchPred (unsigned long int addr, char outcome) {
	// Determine which entry in the BHT to be checked
	unsigned long int index = 0;
	if (en_gshare)
		index = getIndex(addr, en_gshare);
	else
		index = getIndex(addr);

	if (DBG) {
		cout << "	BP:	" << index << "	" << bht[index] << endl;
	}
		
	// Determine if branch is to be taken or not
	char prediction = getPred(index);
	// Is prediction correct?
	bool correct = predCmp(prediction, outcome);
	
	// Update
	updBHT(outcome, index);
	if (en_gshare)
		updBHR(outcome);
	
	if (DBG) {
		cout << "	BU:	" << index << "	" << bht[index] << endl;
	}

	// Stats update
	stats.incrNPred();
	if (!correct)
		stats.incrNMispred();
	stats.setMispredRate();

}

unsigned long int Predictor::getIndex (unsigned long int addr) {
	unsigned long int index = 0;
	unsigned long int index_mask = (1<<m)-1;
	
	// index is lower m bits of pc (m+1:2)
	index = index_mask & (addr>>2);

	return index;
}

unsigned long int Predictor::getIndex (unsigned long int addr, bool en_gshare) {
	unsigned long int index = 0;
	unsigned long int index_mask = (1<<m)-1;
	
	index = index_mask & (addr>>2);
	
	// xor upper n bits of (lower m bits of pc) with bhr
	if (HASH_LOC) {
		if (HASH_SHIFT)
			index = index ^ (bhr<<(m-n));
		else
			index = index ^ (bhr_rev<<(m-n));
	} else {
	// xor lower n bits of (lower m bits of pc) with bhr
		if (HASH_SHIFT)
			index = index ^ (bhr);
		else
			index = index ^ (bhr_rev);
	}

	return index;
}

char Predictor::getPred (unsigned long int index) {
	// If counter is 0 or 1 : not taken
	// If counter is 2 or 3: taken
	if (bht[index] < 2)
		return 'n';
	else
		return 't';
}

bool Predictor::predCmp (char pred, char outcome) {
	// Does prediction match outcome?
	if (pred == outcome)
		return true;
	else
		return false;
}

void Predictor::updBHT(char outcome, unsigned long int index) {
	// If branch taken then increment counter
	// 		Dont increment if counter is 3
	// If branch not taken then decrement counter
	// 		Dont decrement if counter is 0
	if (outcome == 't') {
		if (bht[index] < 3)
			bht[index]++;
	} else {
		if(bht[index] > 0)
			bht[index]--;
	}
}

void Predictor::updBHR(char outcome) {
	// If branch taken append 1 at LSB of BHR
	// Else append 0
	bhr = bhr << 1;
	if (outcome == 't')
		bhr |= ((unsigned long int) 1);	
	unsigned long int bhr_mask = (1<<n)-1;
	bhr &= bhr_mask;
	
	// If branch taken append 1 at MSB of BHR
	// Else append 0
	// Implemented as reversing the LSB BHR
	unsigned long int tmp = bhr;
	for (unsigned long int i=0; i<n; i++) {
		bhr_rev <<= 1;
		if (tmp&1)
			bhr_rev ^= 1;
		tmp >>= 1;
	}
	bhr_rev &= bhr_mask;
}

// -------------------------------------------------------------------
// 			Hybrid Predictor Class function declarations

HybridPred::HybridPred (unsigned long int _m1, unsigned long int _m2, unsigned long int _n, unsigned long int _k) : k(_k) {
	size = 1<<k;
	chooser.resize(size, 1);

	bimodal = new Predictor(_m2);
	gshare	= new Predictor (_m1, _n);

}

void HybridPred::branchPred (unsigned long int addr, char outcome) {

	unsigned long int bimodal_index, gshare_index, chooser_index;
	char bimodal_pred, gshare_pred;
	bool chose_gshare, bimodal_correct, gshare_correct;

	// getIndex for bimodal
	// getPred for bimodal
	bimodal_index 	= bimodal->getIndex(addr);
	bimodal_pred 	= bimodal->getPred(bimodal_index);
	// getIndex for gshare
	// getPred for gshare
	gshare_index 	= gshare->getIndex(addr, true);
	gshare_pred 	= gshare->getPred(gshare_index);
	// getIndex for chooser
	// choose
	chooser_index 	= getIndex(addr);
	chose_gshare 	= choose(chooser_index);
	// updBHT of gshare or bimodal
	// updBHR of gshare
	if (chose_gshare)
		gshare->updBHT(outcome, gshare_index);
	else
		bimodal->updBHT(outcome, bimodal_index);
	gshare->updBHR(outcome);
	// predCmp for bimodal
	// predCmp for gshare
	// updChooser
	bimodal_correct	= bimodal->predCmp(bimodal_pred, outcome);
	gshare_correct	= gshare->predCmp(gshare_pred, outcome);
	updChooser(bimodal_correct, gshare_correct, chooser_index);
	// upd stats
	stats.incrNPred();
	if (chose_gshare && !gshare_correct)
		stats.incrNMispred();
	else if (!chose_gshare && !bimodal_correct)
		stats.incrNMispred();
	stats.setMispredRate();

}

unsigned long int HybridPred::getIndex (unsigned long int addr) {
	unsigned long int index = 0;
	unsigned long int index_mask = (1<<k)-1;
	
	// index is lower m bits of pc (k+1:2)
	index = index_mask & (addr>>2);

	return index;
}

bool HybridPred::choose (unsigned long int index) {
	// If counter is 0 or 1 : bimodal
	// If counter is 2 or 3: gshare
	if (chooser[index] < 2)
		return false;
	else
		return true;
}

void HybridPred::updChooser (bool bimodal_correct, bool gshare_correct, unsigned long int index) {
	// If only gshare correct then increment counter
	// 		Dont increment if counter is 3
	// If only bimodal correct then decrement counter
	// 		Dont decrement if counter is 0
	// If both correct/incorrect dont change
	if (gshare_correct && !bimodal_correct) {
		if (chooser[index] < 3)
			chooser[index]++;
	} else if (!gshare_correct && bimodal_correct) {
		if(chooser[index] > 0)
			chooser[index]--;
	}
}

// -------------------------------------------------------------------
// 					Glocal Function declarations

void printResults (Predictor* bp, HybridPred* hp, char* bp_name) {
	int n_pred, n_mispred;
	float mispred_rate;
	if (hp) {
		n_pred 		= hp->stats.getNPred();
		n_mispred 	= hp->stats.getNMispred();
		mispred_rate = hp->stats.getMispredRate();
	} else {
		n_pred 		= bp->stats.getNPred();
		n_mispred 	= bp->stats.getNMispred();
		mispred_rate = bp->stats.getMispredRate();
	}
	cout << "OUTPUT" << endl;
	printf(" number of predictions:    %d\n", n_pred);
	printf(" number of mispredictions: %d\n", n_mispred);
	printf(" misprediction rate:       %.2f%%\n", mispred_rate);
	
	string name;
	if (!hp) {
		name = bp_name;
		transform(name.begin(), name.end(), name.begin(), ::toupper);
		cout << "FINAL " << name << " CONTENTS" << endl; 
		for (unsigned long int i=0; i<bp->getSize(); i++) {
			cout << " " << i <<	"	" << bp->getBHT(i) << endl;
		}
	} else {
		name = "CHOOSER";
		cout << "FINAL " << name << " CONTENTS" << endl; 
		for (unsigned long int i=0; i<hp->getChooserSize(); i++) {
			cout << " " << i <<	"	" << hp->getChooser(i) << endl;
		}
		name = "GSHARE";
		cout << "FINAL " << name << " CONTENTS" << endl; 
		for (unsigned long int i=0; i<hp->getGshareSize(); i++) {
			cout << " " << i <<	"	" << hp->getGshareBHT(i) << endl;
		}
		name = "BIMODAL";
		cout << "FINAL " << name << " CONTENTS" << endl; 
		for (unsigned long int i=0; i<hp->getBimodalSize(); i++) {
			cout << " " << i <<	"	" << hp->getBimodalBHT(i) << endl;
		}
	}
	
}
