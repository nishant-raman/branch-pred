#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"

#define DBG 1

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

	Predictor* bp;
    
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

		//TODO instantiate hybrid
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
		bp->branchPred (addr, outcome);
		
    }
	
	printResults(bp, params.bp_name);
	
	delete bp;

    return 0;
}


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
}

void Predictor::branchPred (unsigned long int addr, char outcome) {
	// Determine which entry in the BHT to be checked
	unsigned long int index = 0;
	if (en_gshare)
		index = getIndex(addr, bhr);
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

unsigned long int Predictor::getIndex (unsigned long int addr, unsigned long int bhr) {
	unsigned long int index = 0;
	unsigned long int index_mask = (1<<m)-1;
	
	index = index_mask & (addr>>2);
	
	// xor upper n bits of (lower m bits of pc) with bhr
	index = index ^ (bhr<<(m-n));

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

}

void printResults (Predictor* bp, char* bp_name) {
	cout << "OUTPUT" << endl;
	cout << " number of predictions:    " << bp->stats.getNPred() << endl;
	cout << " number of mispredictions: " << bp->stats.getNMispred() << endl;
	cout << " misprediction rate:       " << setprecision(4) << bp->stats.getMispredRate() << "%" << endl;
	
	// FIXME uppercase
	string name = bp_name;
	transform(name.begin(), name.end(), name.begin(), ::toupper);
	cout << "FINAL " << name << " CONTENTS" << endl; 
	for (unsigned long int i=0; i<bp->getSize(); i++) {
		cout << " " << i <<	"	" << bp->getBHT(i) << endl;
	}
	
}
