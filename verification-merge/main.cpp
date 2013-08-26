//
//  main.cpp
//  secure
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>
using namespace std;

#include "parser.h"
#include "fsm.h"
#include "pverify.h"
#include "define.h"
#include "sync.h"

#include "cruise.h"

ProbVerifier pvObj ;
GlobalState* startPoint;

bool printStop(GlobalState* left, GlobalState* right)
{
    /*
     StoppingState stopZero(left);
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 1); // lock 0
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 2); // lock 1
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 3); // lock 2
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 4); // lock 3
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 5); // lock 4
     stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 6); // lock 5
     //stopZero.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 7); // lock 6
     */
    /*
     if( stopZero.match(left) && stopZero.match(right))
     return true ;
     //return false;
     }
     else
     return false;*/
    return false;
}

int nSlaves = 3;
int nParty = nSlaves + 1;

int main( int argc, char* argv[] )
{
    try {
        // Declare the names of component machines so as to register these names as id's in
        // the parser
        Parser* psrPtr = new Parser() ;
        // Create StateMachine objects
        // Add the state machines into ProbVerifier
        // Register the machines that are triggered by deadline (sync)
        Sync* sync = new Sync(nParty, psrPtr->getMsgTable(), psrPtr->getMacTable() );
        pvObj.addMachine(sync);
        
        Coordinator* coord = new Coordinator(psrPtr->getMsgTable(), psrPtr->getMacTable()) ;
        pvObj.addMachine(coord) ;
        
        Cohort** cohorts = new Cohort*[nSlaves] ;
        for( int i = 1; i <= nSlaves ; ++i ) {
            Cohort* coho = new Cohort(i, psrPtr->getMsgTable(), psrPtr->getMacTable()) ;
            pvObj.addMachine(coho) ;
            cohorts[i-1] = coho ;
        }
        
        Master* mstr = new Master(nParty,psrPtr->getMsgTable(),psrPtr->getMacTable());
        pvObj.addMachine(mstr) ;
        sync->setMaster(mstr) ;
        
        Slave** slaves = new Slave*[nSlaves] ;
        for( int i = 1 ; i <= nSlaves ; ++i ) {
            Slave* slav = new Slave(nParty,i,
                                    psrPtr->getMsgTable(),psrPtr->getMacTable());
            pvObj.addMachine(slav) ;
            sync->addMachine(slav) ;
            slaves[i-1] = slav ;
        }
        Channel* chan = new Channel(nParty, psrPtr->getMsgTable(), psrPtr->getMacTable()) ;
        pvObj.addMachine(chan);
        sync->addMachine(chan) ;
        
        Cruise* iccm = new Cruise(psrPtr->getMsgTable(), psrPtr->getMacTable(),
                                  "merge", "m", "ALIGN");
        Cruise* iccf = new Cruise(psrPtr->getMsgTable(), psrPtr->getMacTable(),
                                  "front", "f", "MAINTAIN");
        Cruise* iccb = new Cruise(psrPtr->getMsgTable(), psrPtr->getMacTable(),
                                  "back", "b", "CREATE");
        pvObj.addMachine(iccm);
        pvObj.addMachine(iccf);
        pvObj.addMachine(iccb);
        sync->addMachine(iccm);
        sync->addMachine(iccf);
        sync->addMachine(iccb);
        
        // Initialize SecureChecker
        SecureChecker secChk(nSlaves);
        SecureCheckerState secChkState;
        
        // Add checker into ProbVerifier
        pvObj.addChecker(&secChk);
        
        // Specify the starting state
        GlobalState* startPoint = new GlobalState(pvObj.getMachinePtrs(), &secChkState);
        startPoint->setParser(psrPtr);
        
        // Specify the global states in the set RS (stopping states)
        // initial state: master = 0, slave = 0
        
        StoppingState stop1(startPoint);
        stop1.addAllow(new NodeSnapshot(0), 1) ;   // coordinator
        stop1.addAllow(new NodeSnapshot(0), 2) ;   // cohort 1
        stop1.addAllow(new NodeSnapshot(0), 3) ;   // cohort 2
        stop1.addAllow(new NodeSnapshot(0), 4) ;   // cohort 3
        pvObj.addSTOP(&stop1);
        
        StoppingState stop2(startPoint);
        stop1.addAllow(new NodeSnapshot(2), 1) ;   // coordinator
        stop1.addAllow(new NodeSnapshot(1), 2) ;   // cohort 1
        stop1.addAllow(new NodeSnapshot(1), 3) ;   // cohort 2
        stop1.addAllow(new NodeSnapshot(1), 4) ;   // cohort 3
        pvObj.addSTOP(&stop2);
        
        // Specify the error states
        // One of the slaves is not locked
        /*
         StoppingState lock3FFree(startPoint) ;
         lock3FFree.addAllow(new LockSnapshot(-1,-1,-1,-1,0), 1); // lock 0 in state 0
         lock3FFree.addAllow(new LockSnapshot(10,0,1,-1,4), 3); // lock 2 in state 4
         pvObj.addError(&lock3FFree);
         */
        
        pvObj.addPrintStop(printStop) ;
        //pvObj.addPrintStop();
        
        // Start the procedure of probabilistic verification.
        // Specify the maximum probability depth to be explored
        pvObj.start(10);
        
        // When complete, deallocate all machines
        delete sync ;
        delete mstr ;
        delete coord;
        for( int i = 0 ; i < nSlaves ; ++i ) {
            delete slaves[i] ;
            delete cohorts[i];
        }
        delete[] slaves ;
        delete[] cohorts;
        delete chan ;
        
        delete startPoint;
        
    } catch( runtime_error& re ) {
        cerr << "Runtime error:" << endl
        << re.what() << endl ;
    } catch (exception e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << "Something wrong." << endl;
    }
    
    system("Pause");
    
    return 0;
}

