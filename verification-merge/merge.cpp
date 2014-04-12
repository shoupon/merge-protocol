//
//  merge.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/22/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "merge.h"

Merge::Merge( Lookup* msg, Lookup* mac ): StateMachine(msg, mac)
{
    setId(machineToInt("merge")) ;
    reset() ;
}

int Merge::transit(MessageTuple *inMsg, vector<MessageTuple*> &outMsgs, bool &high_prob,
                   int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    
    if( startIdx != 0 )
        return -1;
    
    string msg = IntToMessage(inMsg->destMsgId()) ;
    string src = IntToMachine(inMsg->subjectId()) ;
    switch (_state) {
        case 0:
            if( msg == "SIGNAL" ) {
                assert(src == "driver") ;
                // Set D0
                outMsgs.push_back(createSetMsg(inMsg, 0)) ;
                outMsgs.push_back(createLockMsg(inMsg, "REQUEST"));
                _state = 1;
                return 3;
            }
            else
                return 3;
            break ;
        case 1:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                assert(did == 0);
                _state = 0;
                return 3;
            }
            else if( msg == "SUCCESS" ) {
                assert(src == "lock") ;
                outMsgs.push_back(createSetMsg(inMsg, 1)) ;
                outMsgs.push_back(createLockMsg(inMsg, "CREATE")) ;
                MessageTuple* tmsg = createOutput(inMsg, machineToInt("trbp"),
                                                  messageToInt("START"));
                outMsgs.push_back(tmsg);
                _state = 2;
                return 3;
            }
            else if( msg == "CANCEL") {
                assert(src == "driver") ;
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 2:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                if( did == 1 || did == 0) {
                    MessageTuple* tmsg = createOutput(inMsg, machineToInt("trbp"),
                                                      messageToInt("STOP"));
                    outMsgs.push_back(tmsg);
                    _state = 0;
                    return 3;
                }
                else
                    assert(false);
            }
            else if( msg == "SUCCESS" ) {
                assert(src == "lock") ;
                outMsgs.push_back(createSetMsg(inMsg, 1)) ;
                outMsgs.push_back(createLockMsg(inMsg, "CREATE")) ;
                MessageTuple* cmsg = createOutput(inMsg, machineToInt("cruise(m)"),
                                                  messageToInt("ALIGN"));
                outMsgs.push_back(cmsg);
                _state = 3;
                return 3;
            }
            else if( msg == "CANCEL") {
                assert(src == "driver") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                               messageToInt("STOP")));
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 3:
            if( msg == "READY" ) {
                assert(src == "trbp");
                outMsgs.push_back(createSetMsg(inMsg, 2));
                outMsgs.push_back(createLockMsg(inMsg, "MERGE"));
                _state = 4;
                return 3;
            }
            else if( msg == "EMERGENCY") {
                assert(src == "trbp");
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET"))) ;
                _state = 0;
                return 3 ;
            }
            else if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                if(did != 1)
                    return 3;
                outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                               messageToInt("STOP")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET")));
                _state = 0 ;
                return 3;
            }
            else if( msg == "CANCEL" ) {
                assert(src == "driver");
                outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                               messageToInt("STOP")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET")));
                _state = 0 ;
                return 3;
            }
            else
                return -1;
            break;
        case 4:
            if( msg == "SUCCESS" ) {
                assert(src == "lock") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("CLEARTOMOVE"))) ;
                _state = 4;
                return 3;
            }
            else if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                if(did == 1 || did == 2) {
                    outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                                   messageToInt("STOP")));
                    outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                                   messageToInt("ABORT")));
                    outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                                   messageToInt("RESET")));
                    return 0;
                }
                else
                    return 3;
            }
            else if( msg == "EMERGENCY") {
                assert(src == "trbp");
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET"))) ;
                _state = 0;
                return 3 ;
            }
            else if( msg == "CANCEL" ) {
                assert(src == "driver");
                outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                               messageToInt("STOP")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET")));
                _state = 0 ;
                return 3;
            }
            else
                return -1;
            break;
        case 5:
            if( msg == "COMPLETE" ) {
                assert(src == "driver") ;
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET"))) ;
                _state = 6 ;
                return 3;
            }
            else if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                if(did == 2) {
                    outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                                   messageToInt("STOP")));
                    outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                                   messageToInt("ABORT")));
                    outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                                   messageToInt("RESET")));
                    return 0;
                }
                else
                    return 3;
            }
            else if( msg == "EMERGENCY") {
                assert(src == "trbp");
                outMsgs.push_back(createOutput(inMsg, machineToInt("driver"),
                                               messageToInt("ABORT")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET"))) ;
                _state = 0;
                return 3 ;
            }
            else if( msg == "CANCEL" ) {
                assert(src == "driver");
                outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                               messageToInt("STOP")));
                outMsgs.push_back(createOutput(inMsg, machineToInt("cruise(m)"),
                                               messageToInt("RESET")));
                _state = 0 ;
                return 3;
            }
            else
                return 3;
            break;
        case 6:
            if( msg == "DEADLINE" ) {
                int did = inMsg->getParam(1) ;
                if(did == 2) {
                    outMsgs.push_back(createOutput(inMsg, machineToInt("trbp"),
                                                   messageToInt("STOP")));
                    return 0;
                }
                else
                    return 3;
            }
            else if( msg == "EMERGENCY") {
                assert(src == "trbp");
                _state = 0;
                return 3 ;
            }
            else
                return 3;
            break ;
        default:
            return -1;
            break;
    }
    return -1;
}

int Merge::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    return -1;
}

MessageTuple* Merge::createOutput(MessageTuple* inMsg, int dest, int destMsg)
{
    return new MessageTuple(inMsg->srcID(), dest, inMsg->srcMsgId(), destMsg, macId());
}

SyncMessage* Merge::createSetMsg(MessageTuple *inMsg, int did)
{
    return new SyncMessage(inMsg->srcID(), machineToInt("sync"),
                           inMsg->srcMsgId(), messageToInt("SET"),
                           macId(), true, did);
}

LockMessage* Merge::createLockMsg(MessageTuple *inMsg, string purpose)
{
    return new LockMessage(inMsg->srcID(), machineToInt("lock"),
                           inMsg->srcMsgId(), messageToInt("ATTEMPT"),
                           macId(), purpose) ;
}