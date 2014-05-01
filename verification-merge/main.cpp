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

        // Create group of processes that are linked to the same timing stack
        // (Group of processes that located within same agent)
        vector<const StateMachine*> mergeCar;
        mergeCar.push_back(merge);
        mergeCar.push_back(trbp);
        mergeCar.push_back(iccm);
        mergeCar.push_back(lock0);
        mergeCar.push_back(lock1);
        mergeCar.push_back(lock2);
        mergeCar.push_back(driver);
        sync->addFailureGroup(mergeCar);
        vector<const StateMachine*> frontCar;
        frontCar.push_back(front);
        frontCar.push_back(trbp);
        frontCar.push_back(iccf);
        frontCar.push_back(lock0);
        frontCar.push_back(lock1);
        frontCar.push_back(lock2);
        frontCar.push_back(driver);
        sync->addFailureGroup(frontCar);
        vector<const StateMachine*> backCar;
        backCar.push_back(back);
        backCar.push_back(trbp);
        backCar.push_back(iccb);
        backCar.push_back(lock0);
        backCar.push_back(lock1);
        backCar.push_back(lock2);
        backCar.push_back(driver);
        sync->addFailureGroup(backCar);

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
        stop1.addAllow(new StateSnapshot(0), 1) ;      // merge
        //stop1.addAllow(new StateSnapshot(0), 2) ;      // front
        //stop1.addAllow(new StateSnapshot(0), 3) ;      // back
        stop1.addAllow(new LockSnapshot(0,0,""), 4) ;    // lock 0
        stop1.addAllow(new StateSnapshot(0), 7) ;      // trbp
        stop1.addAllow(new StateSnapshot(0), 8) ;      // icc merge
        //stop1.addAllow(new StateSnapshot(0), 9) ;      // icc front
        //stop1.addAllow(new StateSnapshot(0), 10) ;     // icc back
        stop1.addAllow(new StateSnapshot(0), 11) ;     // driver
        pvObj.addSTOP(&stop1);
        
        StoppingState stop1a(startPoint);
        stop1a.addAllow(new StateSnapshot(1), 1) ;      // merge
        stop1a.addAllow(new LockSnapshot(1,0,REQUEST), 4) ;    // lock 0
        stop1a.addAllow(new StateSnapshot(0), 7) ;      // trbp
        stop1a.addAllow(new StateSnapshot(0), 8) ;      // icc merge
        stop1a.addAllow(new StateSnapshot(1), 11) ;     // driver
        pvObj.addSTOP(&stop1a);

        StoppingState stop2(startPoint);
        stop2.addAllow(new StateSnapshot(5), 1) ;      // merge
        stop2.addAllow(new StateSnapshot(3), 2) ;      // front
        stop2.addAllow(new StateSnapshot(3), 3) ;      // back
        stop2.addAllow(new LockSnapshot(5,2,MOVE), 6) ; // lock 2
        stop2.addAllow(new StateSnapshot(2), 7) ;      // trbp
        stop2.addAllow(new StateSnapshot(1), 8) ;      // icc merge
        stop2.addAllow(new StateSnapshot(1), 9) ;      // icc front
        stop2.addAllow(new StateSnapshot(1), 10) ;     // icc back
        stop2.addAllow(new StateSnapshot(2), 11) ;     // driver
        pvObj.addSTOP(&stop2);
        
        StoppingState stopclockfail1(startPoint);
        stopclockfail1.addAllow(new StateSnapshot(7), 1) ;      // merge
        //stopclockfail1.addAllow(new StateSnapshot(4), 2) ;      // front
        //stopclockfail1.addAllow(new StateSnapshot(4), 3) ;      // back
        stopclockfail1.addAllow(new StateSnapshot(4), 7) ;      // trbp
        stopclockfail1.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        stopclockfail1.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        stopclockfail1.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        stopclockfail1.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addSTOP(&stopclockfail1);

        StoppingState stopclockfail1a(startPoint);
        stopclockfail1a.addAllow(new StateSnapshot(10), 1) ;      // merge
        //stopclockfail1a.addAllow(new StateSnapshot(4), 2) ;      // front
        //stopclockfail1a.addAllow(new StateSnapshot(4), 3) ;      // back
        stopclockfail1a.addAllow(new StateSnapshot(4), 7) ;      // trbp
        stopclockfail1a.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        stopclockfail1a.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        stopclockfail1a.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        stopclockfail1a.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addSTOP(&stopclockfail1a);
        
        StoppingState stopclockfail2(startPoint);
        //stopclockfail2.addAllow(new StateSnapshot(7), 1) ;      // merge
        stopclockfail2.addAllow(new StateSnapshot(5), 2) ;      // front
        //stopclockfail2.addAllow(new StateSnapshot(4), 3) ;      // back
        stopclockfail2.addAllow(new StateSnapshot(4), 7) ;      // trbp
        stopclockfail2.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        stopclockfail2.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        stopclockfail2.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        stopclockfail2.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addSTOP(&stopclockfail2);

        StoppingState stopclockfail3(startPoint);
        //stopclockfail3.addAllow(new StateSnapshot(7), 1) ;      // merge
        //stopclockfail3.addAllow(new StateSnapshot(4), 2) ;      // front
        stopclockfail3.addAllow(new StateSnapshot(5), 3) ;      // back
        stopclockfail3.addAllow(new StateSnapshot(4), 7) ;      // trbp
        stopclockfail3.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        stopclockfail3.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        stopclockfail3.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        stopclockfail3.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addSTOP(&stopclockfail3);
        
        StoppingState stopclockfail4(startPoint);
        stopclockfail4.addAllow(new StateSnapshot(0), 1) ;      // merge
        stopclockfail4.addAllow(new StateSnapshot(0), 2) ;      // front
        stopclockfail4.addAllow(new StateSnapshot(0), 3) ;      // back
        stopclockfail4.addAllow(new StateSnapshot(4), 7) ;      // trbp
        stopclockfail4.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        stopclockfail4.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        stopclockfail4.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        stopclockfail4.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addSTOP(&stopclockfail4);

        StoppingState end3clock(startPoint);
        end3clock.addAllow(new StateSnapshot(10), 1) ;      // merge
        end3clock.addAllow(new StateSnapshot(5), 2) ;      // front
        end3clock.addAllow(new StateSnapshot(5), 3) ;      // back
        end3clock.addAllow(new StateSnapshot(4), 7) ;      // trbp
        end3clock.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        end3clock.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        end3clock.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        end3clock.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addEND(&end3clock);

        StoppingState end3clocka(startPoint);
        end3clocka.addAllow(new StateSnapshot(7), 1) ;      // merge
        end3clocka.addAllow(new StateSnapshot(5), 2) ;      // front
        end3clocka.addAllow(new StateSnapshot(5), 3) ;      // back
        end3clocka.addAllow(new StateSnapshot(4), 7) ;      // trbp
        end3clocka.addProhibit(new StateSnapshot(1), 8) ;      // icc merge
        end3clocka.addProhibit(new StateSnapshot(1), 9) ;      // icc front
        end3clocka.addProhibit(new StateSnapshot(1), 10) ;     // icc back
        end3clocka.addAllow(new StateSnapshot(4), 11) ;     // driver
        pvObj.addEND(&end3clocka);

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

        StoppingState err7a(startPoint);
        err7a.addAllow(new StateSnapshot(10), 1) ;      // merge
        err7a.addAllow(new StateSnapshot(2), 11);   // driver
        pvObj.addError(&err7a);
        
        StoppingState err7b(startPoint);
        err7b.addAllow(new StateSnapshot(5), 2) ;      // front
        err7b.addAllow(new StateSnapshot(2), 11);   // driver
        pvObj.addError(&err7b);
        
        StoppingState err7c(startPoint);
        err7c.addAllow(new StateSnapshot(5), 3) ;      // back
        err7c.addAllow(new StateSnapshot(2), 11);   // driver
        pvObj.addError(&err7c);
        
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

