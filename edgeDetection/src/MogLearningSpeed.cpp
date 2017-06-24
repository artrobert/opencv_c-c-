//
// Created by artin on 24/6/2017.
//

#include "MogLearningSpeed.h"

float mogLearningSpeed = 0.5;
const float mogLearningBeforeStart = 0.05; // the mog learning rate before the motion started
const float mogLearningAfterStart = 0.001; // the mog learning rate after the motion started

void setLearningBeforeStart() {
    mogLearningSpeed = mogLearningBeforeStart;
}

void setLearningAfterStart() {
    mogLearningSpeed = mogLearningAfterStart;
}

