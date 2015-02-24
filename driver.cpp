//
//  driver.cpp
//  verification-merge
//
//  Created by Shou-pon Lin on 8/27/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#include "driver.h"

Driver::Driver( Lookup* msg, Lookup* mac)
:StateMachine(msg, mac)
{
    setId(machineToInt(DRIVER_NAME));
    machine_name_ = DRIVER_NAME;
    reset();
}

int Driver::transit(MessageTuple *inMsg, vector<MessageTuple *> &outMsgs,
                    bool &high_prob, int startIdx)
{
    outMsgs.clear();
    high_prob = true ;
    
    if( startIdx != 0 )
        return -1;
    
    string msg = IntToMessage(inMsg->destMsgId());
    string src = IntToMachine(inMsg->subjectId());
    if (msg == CLOCKFAIL) {
        _state = 4;
        return 3;
    }
    switch (_state) {
        case 0:
            if( msg == DEADLINE )
                return 3;
            else
                return -1;
        case 1:
            if( msg == ABORT ) {
                assert(src == MERGE_NAME);
                _state = 0 ;
                return 3;
            }
            else if( msg == GREENLIGHT ) {
                assert(src == MERGE_NAME);
                _state = 2;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return 3;
            break;
        case 2:
            if( msg == ABORT ) {
                assert(src == MERGE_NAME);
                _state = 0 ;
                return 3;
            }
            else if( msg == DEADLINE )
                return 3;
            else
                return 3;
        case 4:
            return 3;
        default:
            return -1;
            break;
    }
    
    return -1;
}

int Driver::nullInputTrans(vector<MessageTuple *> &outMsgs, bool &high_prob,
                           int startIdx) {
  outMsgs.clear();
  high_prob = true;
  if (startIdx)
    return -1;

  auto sync_ptr = dynamic_cast<Sync*>(ProbVerifier::getMachine(SYNC_NAME));
  auto m_ptr = ProbVerifier::getMachine(MERGE_NAME);
  auto sensor_ptr = ProbVerifier::getMachine(SENSOR_NAME);
  auto trbp_ptr = ProbVerifier::getMachine(TRBP_NAME);
  switch (_state) {
    case 0:
      if (m_ptr->getState() == 0 &&
          sensor_ptr->getState() != 1 && sensor_ptr->getState() != 3 &&
          trbp_ptr->getState() != 1 &&
          sync_ptr->isAvailable(0) &&
          sync_ptr->isAvailable(1) &&
          sync_ptr->isAvailable(2)) {
        outMsgs.push_back(new MessageTuple(0, machineToInt(MERGE_NAME),
                                           0, messageToInt(SIGNAL), macId()));
        outMsgs.push_back(new MessageTuple(0, machineToInt(CRUISE_MERGE_NAME),
                                           0, messageToInt(REQUIRE), macId()));
        _state = 1;
        return 3;
      } else {
        return -1;
      }
      break;
    case 1:
            outMsgs.push_back(new MessageTuple(0, machineToInt(MERGE_NAME),
                                               0, messageToInt(CANCEL), macId()));
            _state = 0 ;
            return 3;
            break;
        case 2:
            outMsgs.push_back(new MessageTuple(0, machineToInt(MERGE_NAME),
                                               0, messageToInt(FINISH), macId()));
            _state = 0 ;
            return 3;
            break;
        default:
            return -1;
            break;
    }
    return -1;
}
