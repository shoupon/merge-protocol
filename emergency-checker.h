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
  EmergencyChecker(int trbp_pos, int sensor_pos,
                   int merge_pos, int front_pos, int back_pos)
      : trbp_pos_(trbp_pos), sensor_pos_(sensor_pos),
        merge_pos_(merge_pos), front_pos_(front_pos), back_pos_(back_pos) {}
  bool check(CheckerState* checker_state,
             const vector<StateSnapshot*>& machine_states);
  CheckerState* initState() { return new CheckerState(); }
private:
  const int trbp_pos_;
  const int sensor_pos_;
  const int merge_pos_;
  const int front_pos_;
  const int back_pos_;
};

#endif // MERGEPROTOCOL_EMERGENCYCHECKER_H
