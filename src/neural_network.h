#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "SDL.h"
#include "SDL_ttf.h"

#include "visual.h"

#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b
#define swap(a,b) a=a^b,b=a^b,a=a^b

#define NO_VALUE INT_MIN
#define NEURON_SZ 16
#define MIN_GAP 4

double sigmoid(double x);
double dsigmoid(double x);
int evalGap(int full, int cell, int cellCnt);

typedef struct Neuron {
    double raw;
    double draw;
    double value;
    double bias;
    int linkCnt;
    struct Neuron** link;
    double* weight;
} Neuron;

void NeuronPropagate(Neuron* this, double value);
void NeuronBackPropagate(Neuron* this, double expected, double rate);
double NeuronEvalCost(Neuron* this, double expected);
SDL_Color NeuronEvalColor(Neuron* this);
SDL_Color NeuronEvalWeightColor(Neuron* this, int idx);

typedef struct NeuralNetwork {
    int layerCnt;
    int* neuronCnt;
    Neuron** net;
} NeuralNetwork;

void NeuralNetworkInit(NeuralNetwork* this, int layerCnt, int* neuronCnt);
void NeuralNetworkFree(NeuralNetwork* this);
void NeuralNetworkFree(NeuralNetwork* this);
void NeuralNetworkPropagate(NeuralNetwork* this, double* input);
double NeuralNetworkEvalCost(NeuralNetwork* this, double* expected);
void NeuralNetworkBackPropagate(NeuralNetwork* this, double* expected, double rate);
void NeuralNetworkDraw(NeuralNetwork* this, SDL_Renderer* renderer, int x, int y, int w, int h);
void NeuralNetworkDrawMeta(NeuralNetwork* this, Visualizer* visualizer, double* expected, int x, int y);

#endif
