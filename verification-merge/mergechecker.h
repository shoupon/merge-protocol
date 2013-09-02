//
//  mergechecker.h
//  verification-merge
//
//  Created by Shou-pon Lin on 8/26/13.
//  Copyright (c) 2013 Shou-pon Lin. All rights reserved.
//

#ifndef MERGE_CHK_H
#define MERGE_CHK_H

#include <iostream>
#include <vector>
using namespace std;

#include "../prob_verify/statemachine.h"
#include "../prob_verify/checker.h"

class MergeChecker;

class MergeCheckerState: public CheckerState
{
    friend MergeChecker;
public:
    MergeCheckerState() {}
    MergeCheckerState* clone()  { return new MergeCheckerState();  }
};

class MergeChecker: public Checker
{
public:
    MergeChecker() {}
};

#endif /* defined(MERGE_CHK_H) */
