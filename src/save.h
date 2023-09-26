#include <stdio.h>
#include <stdlib.h>

#include "neural_network.h"

#ifndef SAVE_H
#define SAVE_H

int NeuralNetworkSave(NeuralNetwork* this, const char* path);
int NeuralNetworkLoad(NeuralNetwork* this, const char* path);

#endif
