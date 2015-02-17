//
//  trbp.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 4/11/14.
//  Copyright (c) 2014 Shou-pon Lin. All rights reserved.
//

#include "trbp.h"


TRBP::TRBP( Lookup* msg, Lookup* mac ):StateMachine(msg, mac)
{
    setId(machineToInt(TRBP_NAME)) ;
    machine_name_ = TRBP_NAME;
    reset();
}

int TRBP::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs, bool &high_prob,
                      int startIdx)
{
    outMsgs.clear();
    high_prob = true;
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    
    if( startIdx != 0 )
        return -1;
    switch (_state) {
        case 0:
            if( msg == TRBPON ) {
                assert(src == MERGE_NAME);
                _state = 1;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else if (msg == CLOCKFAIL){
                _state = 2;
                return 3;
            }
            else
                return -1;
            break;
        case 1:
            if( msg == TRBPOFF ) {
                assert(src == MERGE_NAME) ;
                _state = 0 ;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else if (msg == CLOCKFAIL) {
                outMsgs.push_back(loss(MERGE_NAME));
                outMsgs.push_back(loss(FRONT_NAME));
                outMsgs.push_back(loss(BACK_NAME));
                outMsgs.push_back(createMsg(inMsg, SENSOR_NAME, TRBPFAIL));
                _state = 2;
                return 3;
            }
            else if (msg == REQUIRE)
                return 3;
            else
                return -1;
            break;
        case 2:
            if( msg == DEADLINE )
                return 3;
            else if (msg == CLOCKFAIL) 
                return 3;
            else if (msg == TRBPOFF) {
                assert(src == MERGE_NAME);
                return 3;
            }
            else
                return -1;
            break;
        default:
            return -1;
            break;
    }
}

int TRBP::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob, int startIdx)
{
    outMsgs.clear() ;
    if( _state != 1 )
        return -1;
    high_prob = true;
    switch (_state) {
        case 0:
        case 2:
            return -1;
        case 1:
            if( startIdx == 0 ) {
                outMsgs.push_back(loss(MERGE_NAME));
                outMsgs.push_back(loss(FRONT_NAME));
                outMsgs.push_back(loss(BACK_NAME));
                outMsgs.push_back(createMsg(0, SENSOR_NAME, TRBPFAIL));
                _state = 0;
                high_prob = false;
                return 10;
            }
            else {
                return -1;
            }
            break;
            
        default:
            return -1;
            break;
    }
}

MessageTuple* TRBP::createMsg(MessageTuple *inMsg, const string& dest, string msg)
{
    if( inMsg == 0 )
        return new MessageTuple(0, machineToInt(dest),
                                0, messageToInt(msg), macId()) ;
    else
        return new MessageTuple(inMsg->srcID(), machineToInt(dest),
                                inMsg->srcMsgId(), messageToInt(msg), macId()) ;
}


MessageTuple* TRBP::loss(const string &dest)
{
    return createMsg(0, dest, COMMLOSS);
}
