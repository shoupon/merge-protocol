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

#include "../prob_verify/parser.h"
#include "../prob_verify/pverify.h"
#include "../prob_verify/define.h"
#include "../prob_verify/sync.h"

#include "identifiers.h"

#include "merge.h"
#include "front.h"
#include "back.h"
#include "lock.h"
#include "trbp.h"
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
    return true;
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
        StateMachine::setLookup(psrPtr->getMsgTable(), psrPtr->getMacTable()) ;
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
        
        Lock* lock0 = new Lock(psrPtr->getMsgTable(), psrPtr->getMacTable(), 0) ;
        Lock* lock1 = new Lock(psrPtr->getMsgTable(), psrPtr->getMacTable(), 1) ;
        Lock* lock2 = new Lock(psrPtr->getMsgTable(), psrPtr->getMacTable(), 2) ;
        pvObj.addMachine(lock0);
        sync->addMachine(lock0);
        pvObj.addMachine(lock1);
        sync->addMachine(lock1);
        pvObj.addMachine(lock2);
        sync->addMachine(lock2);

        TRBP* trbp = new TRBP(psrPtr->getMsgTable(), psrPtr->getMacTable());
        pvObj.addMachine(trbp);
        sync->addMachine(trbp);
        
        Cruise* iccm = new Cruise(psrPtr->getMsgTable(), psrPtr->getMacTable(),
                                  MERGE_NAME, "m", ALIGN);
        Cruise* iccf = new Cruise(psrPtr->getMsgTable(), psrPtr->getMacTable(),
                                  FRONT_NAME, "f", MAINTAIN);
        Cruise* iccb = new Cruise(psrPtr->getMsgTable(), psrPtr->getMacTable(),
                                  BACK_NAME, "b", MAKEGAP);
        pvObj.addMachine(iccm);
        pvObj.addMachine(iccf);
        pvObj.addMachine(iccb);
        sync->addMachine(iccm);
        sync->addMachine(iccf);
        sync->addMachine(iccb);
        
        
        Driver* driver = new Driver(psrPtr->getMsgTable(), psrPtr->getMacTable());
        pvObj.addMachine(driver);
        sync->addMachine(driver);
        
        // Initialize SecureChecker
        MergeChecker mergeChk;
        MergeCheckerState mergeChkState;
        
        // Add checker into ProbVerifier
        pvObj.addChecker(&mergeChk);
        
        // Add a default service (stub)
        Service *srvc = new Service();
        srvc->reset();
        GlobalState::setService(srvc);
        
        // Specify the starting state
        GlobalState* startPoint = new GlobalState(pvObj.getMachinePtrs(), &mergeChkState);
        startPoint->setParser(psrPtr);
        
        // Specify the global states in the set RS (stopping states)
        // initial state
        StoppingState stop1(startPoint);
        stop1.addAllow(new StateSnapshot(0), 1) ;    // merge
        stop1.addAllow(new StateSnapshot(0), 2) ;    // front
        stop1.addAllow(new StateSnapshot(0), 3) ;    // back
        stop1.addAllow(new StateSnapshot(0), 4) ;    // lock 0
        stop1.addAllow(new StateSnapshot(0), 5) ;    // lock 1
        stop1.addAllow(new StateSnapshot(0), 6) ;    // lock 2
        stop1.addAllow(new StateSnapshot(0), 7) ;    // trbp
        stop1.addAllow(new StateSnapshot(0), 8) ;    // icc merge
        stop1.addAllow(new StateSnapshot(0), 9) ;    // icc front
        stop1.addAllow(new StateSnapshot(0), 10) ;   // icc back
        stop1.addAllow(new StateSnapshot(0), 11) ;   // driver
        pvObj.addSTOP(&stop1);
        
        StoppingState stop2(startPoint);
        stop1.addAllow(new StateSnapshot(5), 1) ;    // merge
        stop1.addAllow(new StateSnapshot(3), 2) ;    // front
        stop1.addAllow(new StateSnapshot(3), 3) ;    // back
        stop1.addAllow(new StateSnapshot(5), 6) ;    // lock 2
        stop1.addAllow(new StateSnapshot(2), 7) ;    // trbp
        stop1.addAllow(new StateSnapshot(1), 8) ;    // icc merge
        stop1.addAllow(new StateSnapshot(1), 9) ;    // icc front
        stop1.addAllow(new StateSnapshot(1), 10) ;   // icc back
        stop1.addAllow(new StateSnapshot(2), 11) ;   // driver
        pvObj.addSTOP(&stop2);
        
        /*
        StoppingState end1(startPoint);
        end1.addAllow(new StateSnapshot(0), 1) ;    // merge
        end1.addAllow(new StateSnapshot(0), 2) ;    // front
        end1.addAllow(new StateSnapshot(0), 3) ;    // back
        end1.addAllow(new StateSnapshot(0), 4) ;    // lock
        end1.addAllow(new StateSnapshot(0), 5) ;    // periodic
        end1.addAllow(new StateSnapshot(0), 6) ;    // icc merge
        end1.addAllow(new StateSnapshot(0), 7) ;    // icc front
        end1.addAllow(new StateSnapshot(0), 8) ;    // icc back
        end1.addAllow(new StateSnapshot(0), 9) ;    // coord sensor
        end1.addAllow(new StateSnapshot(1), 10);    // sensor front
        end1.addAllow(new StateSnapshot(0), 11) ;   // driver
        pvObj.addEND(&end1);*/
        
        // Driver is notified GREENLIGHT, but the gap is not ready yet
        StoppingState err1a(startPoint);
        err1a.addAllow(new StateSnapshot(2), 11);     // driver
        err1a.addAllow(new StateSnapshot(0), 7);      // trbp
        pvObj.addError(&err1a);
        
        StoppingState err1b(startPoint);
        err1b.addAllow(new StateSnapshot(2), 11);     // driver
        err1b.addAllow(new StateSnapshot(1), 7);      // trbp
        pvObj.addError(&err1b);
        
        // Cruise control is being engaged when trbp is not started
        StoppingState err2a(startPoint);
        err2a.addAllow(new StateSnapshot(0), 7);    // trbp
        err2a.addAllow(new StateSnapshot(1), 8);    // icc merge
        pvObj.addError(&err2a);

        StoppingState err2b(startPoint);
        err2b.addAllow(new StateSnapshot(0), 7);    // trbp
        err2b.addAllow(new StateSnapshot(1), 9);    // icc front
        pvObj.addError(&err2b);
        
        StoppingState err2c(startPoint);
        err2c.addAllow(new StateSnapshot(0), 7);    // trbp
        err2c.addAllow(new StateSnapshot(1), 10);   // icc back
        pvObj.addError(&err2c);
        
        // Driver is not notified when emergency takes place
        StoppingState err3a(startPoint);
        err3a.addAllow(new StateSnapshot(1), 7);    // trbp
        err3a.addAllow(new StateSnapshot(1), 11);   // driver
        //pvObj.addError(&err3a);
        
        StoppingState err3b(startPoint);
        err3b.addAllow(new StateSnapshot(1), 7);    // trbp
        err3b.addAllow(new StateSnapshot(2), 11);   // driver
        pvObj.addError(&err3b);
        
        // Driver is changing lanes but either the periodic update or the lock protocol
        // is not operating
        StoppingState err4a(startPoint);
        err4a.addAllow(new StateSnapshot(2), 11);   // driver
        err4a.addAllow(new StateSnapshot(0), 6) ;   // lock 2
        pvObj.addError(&err4a);
        
        StoppingState err4b(startPoint);
        err4b.addAllow(new StateSnapshot(2), 11);   // driver
        err4b.addAllow(new StateSnapshot(1), 6) ;   // lock 2
        pvObj.addError(&err4b);
        
        StoppingState err4c(startPoint);
        err4c.addAllow(new StateSnapshot(2), 11);   // driver
        err4c.addAllow(new StateSnapshot(2), 6) ;   // lock 2
        pvObj.addError(&err4c);
        
        StoppingState err4d(startPoint);
        err4d.addAllow(new StateSnapshot(2), 11);   // driver
        err4d.addAllow(new StateSnapshot(3), 6) ;   // lock 2
        pvObj.addError(&err4d);

        StoppingState err4e(startPoint);
        err4e.addAllow(new StateSnapshot(2), 11);   // driver
        err4e.addAllow(new StateSnapshot(4), 6) ;   // lock 2
        pvObj.addError(&err4e);

        
        // Driver is cleared to move but the cruise control is reset
        StoppingState err5a(startPoint);
        err5a.addAllow(new StateSnapshot(2), 11);   // driver
        err5a.addAllow(new StateSnapshot(0), 8);    // icc merge
        pvObj.addError(&err5a);

        StoppingState err5b(startPoint);
        err5b.addAllow(new StateSnapshot(2), 11);   // driver
        err5b.addAllow(new StateSnapshot(0), 9);    // icc front
        pvObj.addError(&err5b);
        
        StoppingState err5c(startPoint);
        err5c.addAllow(new StateSnapshot(2), 11);   // driver
        err5c.addAllow(new StateSnapshot(0), 10);   // icc back
        pvObj.addError(&err5c);
        
        
        // Driver is shown green light but the cruise control is in collision
        // avoidance mode
        StoppingState err6a(startPoint);
        err6a.addAllow(new StateSnapshot(2), 11);   // driver
        err6a.addAllow(new StateSnapshot(2), 8);    // icc merge
        pvObj.addError(&err6a);
        
        StoppingState err6b(startPoint);
        err6b.addAllow(new StateSnapshot(2), 11);   // driver
        err6b.addAllow(new StateSnapshot(2), 9);    // icc front
        pvObj.addError(&err6b);
        
        StoppingState err6c(startPoint);
        err6c.addAllow(new StateSnapshot(2), 11);   // driver
        err6c.addAllow(new StateSnapshot(2), 10);   // icc back
        pvObj.addError(&err6c);
        
        pvObj.addPrintStop(printStop) ;
        
        // Start the procedure of probabilistic verification.
        // Specify the maximum probability depth to be explored
        pvObj.start(10);
        
        // When complete, deallocate all machines
        delete sync ;
        delete merge;
        delete front;
        delete back;
        delete lock0;
        delete lock1;
        delete lock2;
        delete iccm;
        delete iccf ;
        delete iccb;
        delete trbp;
        
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

