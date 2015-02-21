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
    setId(machineToInt(MERGE_NAME)) ;
    machine_name_ = MERGE_NAME;
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
            if( msg == SIGNAL ) {
                assert(src == DRIVER_NAME ) ;
                // Set D0
                outMsgs.push_back(createSetMsg(inMsg, 0)) ;
                outMsgs.push_back(createLockMsg(inMsg, REQUEST, 0));
                _state = 1;
                return 3;
            }
            else if (msg == SUCCESS) {
                return 3;
            }
            else if (msg == CLOCKFAIL) {
                assert(src == SYNC_NAME);
                _state = 10;
                return 3;
            }
            else if (msg == DEADLINE)
                return 3;
            else
                return -1;
            break ;
        case 1:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                assert(did == 0);
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 0;
                return 3;
            }
            else if( msg == SUCCESS ) {
                assert(src == LOCK_0_NAME) ;
                outMsgs.push_back(createSetMsg(inMsg, 1)) ;
                outMsgs.push_back(createLockMsg(inMsg, CREATE, 1)) ;
                _state = 2;
                return 3;
            }
            else if (msg == CLOCKFAIL) {
                assert(src == SYNC_NAME);
                _state = 10;
                return 3;
            }
            else if( msg == CANCEL) {
                assert(src == DRIVER_NAME) ;
                _state = 0 ;
                return 3;
            }
            else
                return -1;
            break;
        case 2:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                if( did == 1 || did == 0) {
                    outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                                   messageToInt(ABORT)));
                    _state = 8;
                    return 3;
                }
                else
                    assert(false);
            }
            else if( msg == SUCCESS ) {
                assert(src == LOCK_1_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                               messageToInt(SENSORON)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                                  messageToInt(ALIGN)));
                _state = 3;
                return 3;
            }
            else if (msg == CANCEL) {
                assert(src == DRIVER_NAME) ;
                _state = 8;
                return 3;
            } else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 8;
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else
                return 3;
            break;
        case 3:
            if( msg == GAPREADY ) {
                assert(src == SENSOR_NAME);
                outMsgs.push_back(createSetMsg(inMsg, 2));
                outMsgs.push_back(createLockMsg(inMsg, MOVE, 2));
                _state = 4;
                return 3;
            }
            else if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                if(did != 1)
                    return 3;
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                               messageToInt(SENSOROFF)));
                _state = 0 ;
                return 3;
            }
            else if( msg == CANCEL) {
                assert(src == DRIVER_NAME);
                cancelSeq(inMsg, outMsgs);
                _state = 8 ;
                return 3;
            } else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 8;
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else
                return -1;
            break;
        case 4:
            if( msg == SUCCESS ) {
                assert(src == LOCK_2_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(GREENLIGHT))) ;
                _state = 5;
                return 3;
            }
            else if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                assert(did < 2);
                if(did == 1) {
                    outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                                   messageToInt(ABORT)));   
                    outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                                   messageToInt(RESET)));
                    outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                                   messageToInt(SENSOROFF)));
                    _state = 7;
                    return 3;
                }
                else
                    return 3;
            }
            else if( msg == CANCEL ) {
                assert(src == DRIVER_NAME);
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                               messageToInt(SENSOROFF)));
                _state = 7;
                return 3;
            } else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 7;
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else
                return -1;
            break;
        case 5:
            if( msg == FINISH ) {
                assert(src == DRIVER_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET))) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                               messageToInt(SENSOROFF)));
                _state = 6 ;
                return 3;
            }
            else if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                if(did == 2) {
                    outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                                   messageToInt(ABORT)));
                    outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                                   messageToInt(RESET)));
                    outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                                   messageToInt(SENSOROFF)));
                    _state = 0;
                    return 3;
                }
                else
                    return 3;
            } else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME) ;
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 7;
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                               messageToInt(RESET)));
                outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                               messageToInt(ABORT)));
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else
                return -1;
            break;
        case 6:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                if(did == 2) {
                    _state = 0;
                    return 3;
                }
                else
                    return 3;
            }
            else if (msg == SUCCESS) {
                assert(src == LOCK_2_NAME);
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME);
                return 3;
            }
            else
                return -1;
            break ;
        case 7:
            if (msg == DEADLINE) {
                int did = inMsg->getParam(1) ;
                if(did == 2) {
                    _state = 0;
                    return 3;
                }
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                _state = 0;
                return 3;
            }
            else if (msg == SUCCESS) {
                assert(src == LOCK_1_NAME || src == LOCK_2_NAME);
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else
                return -1;
            break;
        case 8:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1) ;
                if(did == 1) {
                    _state = 0;
                    return 3;
                }
                else
                    return 3;
            }
            else if( msg == SUCCESS ) {
                assert(src == LOCK_1_NAME) ;
                return 3;
            }
            else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME);
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL)
                return 3;
            else
                return -1;
            break ;
        case 10:
            if( isEmergency(inMsg, outMsgs) ){
                return 3;
            }
            else if (msg == SUCCESS) {
                return 3;
            }
            else
                return -1;
            break;
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

bool Merge::isEmergency(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs) {
    string msg = IntToMessage(inMsg->destMsgId()) ;
    string src = IntToMachine(inMsg->subjectId()) ;
    if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
        assert(src == SENSOR_NAME) ;
        outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                       messageToInt(RESET)));
        outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                       messageToInt(ABORT)));
        return true;
    }
    else if (msg == COMMLOSS) {
        assert(src == TRBP_NAME);
        outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                       messageToInt(RESET)));
        outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                       messageToInt(ABORT)));
        return true;
    }
    else
        return false;
}

void Merge::abortSeq(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs)
{
    outMsgs.push_back(createOutput(inMsg, machineToInt(DRIVER_NAME),
                                   messageToInt(ABORT)));
    outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                   messageToInt(RESET)));
    outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                   messageToInt(SENSOROFF)));
}

void Merge::cancelSeq(MessageTuple* inMsg, vector<MessageTuple*>& outMsgs)
{
    outMsgs.push_back(createOutput(inMsg, machineToInt(CRUISE_MERGE_NAME),
                                   messageToInt(RESET)));
    outMsgs.push_back(createOutput(inMsg, machineToInt(SENSOR_NAME),
                                   messageToInt(SENSOROFF)));
}

MessageTuple* Merge::createOutput(MessageTuple* inMsg, int dest, int destMsg)
{
    return new MessageTuple(inMsg->srcID(), dest, inMsg->srcMsgId(), destMsg, macId());
}

SyncMessage* Merge::createSetMsg(MessageTuple *inMsg, int did)
{
    return new SyncMessage(inMsg->srcID(), machineToInt(SYNC_NAME),
                           inMsg->srcMsgId(), messageToInt(SET),
                           macId(), true, did);
}

LockMessage* Merge::createLockMsg(MessageTuple *inMsg, string purpose, int did)
{
    stringstream ss;
    ss << LOCK_NAME << "(" << did << ")" ;
    return new LockMessage(inMsg->srcID(), machineToInt(ss.str()),
                           inMsg->srcMsgId(), messageToInt(ATTEMPT),
                           macId(), purpose) ;
}
