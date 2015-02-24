//
//  emergency-checker.cpp
//  merge-protocol
//
//  Created by Shou-pon Lin on 2/23/15.
//  Copyright (c) 2015 Shou-pon Lin. All rights reserved.
//

#ifndef MERGEPROTOCOL_EMERGENCYCHECKER_H
#define MERGEPROTOCOL_EMERGENCYCHECKER_H

#include "../prob_verify/pverify.h"
#include "../prob_verify/statemachine.h"
#include "../prob_verify/checker.h"

#include "identifiers.h"

class EmergencyChecker: public Checker {
public:
  EmergencyChecker();
  bool check(CheckerState* checker_state,
             const vector<StateSnapshot*>& machine_states);
  CheckerState* initState() { return new CheckerState(); }
private:
  const int trbp_pos_;
  const int sensor_pos_;
  const int driver_pos_;
  const int merge_pos_;
  const int front_pos_;
  const int back_pos_;
  const int cc_merge_pos_;
  const int cc_front_pos_;
  const int cc_back_pos_;
};

#endif // MERGEPROTOCOL_EMERGENCYCHECKER_H
