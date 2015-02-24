//
//  back.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "back.h"

Back::Back( Lookup* msg, Lookup* mac )
:StateMachine(msg, mac)
{
    setId(machineToInt(BACK_NAME)) ;
    machine_name_ = BACK_NAME;
    reset() ;
}

int Back::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                  int startIdx)
{
    outMsgs.clear() ;
    high_prob = true ;
    
    if( startIdx != 0 )
        return -1;
    
    string msg = IntToMessage(inMsg->destMsgId()) ;
    string src = IntToMachine(inMsg->subjectId()) ;

    auto sensor_ptr = ProbVerifier::getMachine(SENSOR_NAME);
    auto trbp_ptr = ProbVerifier::getMachine(TRBP_NAME);
    switch (_state) {
        case 0:
            if( msg == COOPERATE ) {
                if (trbp_ptr->getState() == 1) {
                  _state = 5;
                } else {
                  _state = 1;
                }
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else if( isEmergency(inMsg, outMsgs) ) {
                outMsgs.clear();
                _state = 0;
                return 3;
            }
            else if (msg == CLOCKFAIL) {
                _state = 5;
                return 3;
            }
            else
                return -1;
            break;
        case 1:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1);
                if( did == 0 ) {
                    _state = 0;
                    return 3;
                }
                else
                    return -1;
            }
            else if( msg == COOPERATE ) {
                assert(src == LOCK_1_NAME) ;
                if (sensor_ptr->getState() == 1) {
                  _state = 5;
                } else {
                  _state = 2;
                  outMsgs.push_back(new MessageTuple(
                      inMsg->srcID(), machineToInt(CRUISE_BACK_NAME),
                      inMsg->srcMsgId(), messageToInt(MAKEGAP), macId()));
                }
                return 3;
            }
            else if (msg == CLOCKFAIL) {
                _state = 5;
                return 3;
            } else if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
                assert(src == SENSOR_NAME) ;
                _state = 0;
                return 3;
            }
            else if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                _state = 0;
                return 3;
            }
            else
                return -1;
        case 2:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1);
                if( did == 0 )
                    return 3;
                else if( did != 1 )
                    return -1;
                // did == 1
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt(CRUISE_BACK_NAME),
                                                   inMsg->srcMsgId(),
                                                   messageToInt(RESET),
                                                   macId()));
                _state = 0;
                return 3;
            }
            else if( isEmergency(inMsg, outMsgs) )
                return 3;
            else if( msg == COOPERATE ) {
                assert(src == LOCK_2_NAME);
                _state = 3;
                return 3;
            }
            else if (msg == CLOCKFAIL){
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt(CRUISE_BACK_NAME),
                                                   inMsg->srcMsgId(),
                                                   messageToInt(RESET),
                                                   macId()));
                _state = 5;
                return 3;
            }
            else
                return -1;
            break ;
            
        case 3:
            if( msg == DEADLINE ) {
                int did = inMsg->getParam(1);
                if( did == 0 || did == 1)
                    return 3;
                else if( did > 2 )
                    assert(false) ;
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt(CRUISE_BACK_NAME),
                                                   inMsg->srcMsgId(),
                                                   messageToInt(RESET),
                                                   macId()));
                _state = 0 ;
                return 3;
            }
            else if( isEmergency(inMsg, outMsgs) )
                return 3;
            else if (msg == CLOCKFAIL){
                outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                                   machineToInt(CRUISE_BACK_NAME),
                                                   inMsg->srcMsgId(),
                                                   messageToInt(RESET),
                                                   macId()));
                _state = 5;
                return 3;
            }
            else
                return -1;
        case 5:
            if (msg == COMMLOSS) {
                assert(src == TRBP_NAME);
                return 3;
            } else if (msg == DEADLINE || msg == CLOCKFAIL) {
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

bool Back::isEmergency(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs) {
    string msg = IntToMessage(inMsg->destMsgId()) ;
    string src = IntToMachine(inMsg->subjectId()) ;
    if (msg == EMERGENCY || msg == GAPTAKEN || msg == INCONSISTENT) {
        assert(src == SENSOR_NAME) ;
        outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                           machineToInt(CRUISE_BACK_NAME),
                                           inMsg->srcMsgId(),
                                           messageToInt(RESET),
                                           macId()));
        _state = 0;
        return true;
    }
    else if (msg == COMMLOSS) {
        assert(src == TRBP_NAME);
        outMsgs.push_back(new MessageTuple(inMsg->srcID(),
                                           machineToInt(CRUISE_BACK_NAME),
                                           inMsg->srcMsgId(),
                                           messageToInt(RESET),
                                           macId()));
        _state = 0;
        return true;
    }
    else
        return false;
}
