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

#include "merge.h"
#include "front.h"
#include "back.h"
#include "lock.h"
#include "periodic.h"
#include "coordinatedsensor.h"
#include "sensorf.h"
#include "cruise.h"
#include "driver.h"

#include "mergechecker.h"

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
        
        Merge* merge = new Merge(psrPtr->getMsgTable(), psrPtr->getMacTable() );
        Front* front = new Front(psrPtr->getMsgTable(), psrPtr->getMacTable() );
        Back* back = new Back(psrPtr->getMsgTable(), psrPtr->getMacTable() );
        pvObj.addMachine(merge);
        pvObj.addMachine(front);
        pvObj.addMachine(back);
        sync->setMaster(merge);
        sync->addMachine(front);
        sync->addMachine(back);
        
        Lock* lock = new Lock(psrPtr->getMsgTable(), psrPtr->getMacTable()) ;
        pvObj.addMachine(lock);
        sync->addMachine(lock);
        
        Periodic* period = new Periodic(psrPtr->getMsgTable(), psrPtr->getMacTable());
        pvObj.addMachine(period);
        sync->addMachine(period);
        
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
        
        CoordinatedSensor* coord = new CoordinatedSensor(psrPtr->getMsgTable(),
                                                         psrPtr->getMacTable() );
        pvObj.addMachine(coord);
        sync->addMachine(coord);
        
        SensorF* sensf = new SensorF(psrPtr->getMsgTable(), psrPtr->getMacTable() );
        pvObj.addMachine(sensf);
        sync->addMachine(sensf);
        
        Driver* driver = new Driver(psrPtr->getMsgTable(), psrPtr->getMacTable());
        pvObj.addMachine(driver);
        sync->addMachine(driver);
        
        // Initialize SecureChecker
        MergeChecker mergeChk;
        MergeCheckerState mergeChkState;
        
        // Add checker into ProbVerifier
        pvObj.addChecker(&mergeChk);
        
        // Specify the starting state
        GlobalState* startPoint = new GlobalState(pvObj.getMachinePtrs(), &mergeChkState);
        startPoint->setParser(psrPtr);
        
        // Specify the global states in the set RS (stopping states)
        // initial state: master = 0, slave = 0
        
        StoppingState stop1(startPoint);
        stop1.addAllow(new StateSnapshot(0), 1) ;    // merge
        stop1.addAllow(new StateSnapshot(0), 2) ;    // front
        stop1.addAllow(new StateSnapshot(0), 3) ;    // back
        stop1.addAllow(new StateSnapshot(0), 4) ;    // lock
        stop1.addAllow(new StateSnapshot(0), 5) ;    // periodic
        stop1.addAllow(new StateSnapshot(0), 6) ;    // icc merge
        stop1.addAllow(new StateSnapshot(0), 7) ;    // icc front
        stop1.addAllow(new StateSnapshot(0), 8) ;    // icc back
        stop1.addAllow(new StateSnapshot(0), 9) ;    // coord sensor
        stop1.addAllow(new StateSnapshot(0), 10) ;   // sensor front
        stop1.addAllow(new StateSnapshot(0), 11) ;   // driver
        pvObj.addSTOP(&stop1);
        
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
        delete merge;
        delete front;
        delete back;
        delete lock;
        delete period;
        delete iccm;
        delete iccf ;
        delete iccb;
        delete coord;
        delete sensf ;
        
        delete startPoint;
        
        delete psrPtr;
        
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

