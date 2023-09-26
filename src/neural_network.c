#include "neural_network.h"

double sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

double dsigmoid(double x) {
    double sig = sigmoid(x);
    return sig * (1 - sig);
}

int evalGap(int full, int cell, int cellCnt) {
    return (full - cell * cellCnt) / (cellCnt - 1);
}

void NeuronPropagate(Neuron* this, double value) {
    if (value != NO_VALUE)
        this->value = value;
    else
        this->value = fmax(0, sigmoid(this->raw + this->bias));
    this->raw = 0;
    for (int i = 0; i < this->linkCnt; i++)
        this->link[i]->raw += this->value * this->weight[i];
}

void NeuronBackPropagate(Neuron* this, double expected, double rate) {
    double dvalue;
    if (expected != NO_VALUE) {
        dvalue = 2 * (this->value - expected);
    } else {
        dvalue = 0;
        for (int i = 0; i < this->linkCnt; i++) {
            dvalue += this->weight[i] * this->link[i]->draw;
            this->weight[i] -= this->value * this->link[i]->draw * rate;
        }
        dvalue /= this->linkCnt;
    }
    double drvt = dsigmoid(this->raw + this->bias) * dvalue;
    this->bias -= drvt * rate;
    this->draw = drvt;
}

double NeuronEvalCost(Neuron* this, double expected) {
    double diff = this->value - expected;
    return diff * diff;
}

SDL_Color NeuronEvalColor(Neuron* this) {
    int mapped = (int)(255 * tanh(this->value));
    return (SDL_Color){mapped, mapped, mapped, SDL_ALPHA_OPAQUE};
}

SDL_Color NeuronEvalWeightColor(Neuron* this, int idx) {
    double weight = tanh(this->weight[idx]);
    if (weight > 0)
        return (SDL_Color){0, (int)(204*weight), (int)(255*weight), SDL_ALPHA_OPAQUE};
    weight = -weight;
    return (SDL_Color){(int)(255*weight), (int)(68*weight), 0, SDL_ALPHA_OPAQUE};
}

void NeuralNetworkInit(NeuralNetwork* this, int layerCnt, int* neuronCnt) {
    srand(time(0));
    this->layerCnt = layerCnt;
    this->neuronCnt = malloc(sizeof(int) * layerCnt);
    memcpy(this->neuronCnt, neuronCnt, sizeof(int) * layerCnt);
    this->net = malloc(sizeof(Neuron*) * this->layerCnt);
    for (int i = this->layerCnt-1; i >= 0; i--) {
        this->net[i] = malloc(sizeof(Neuron) * this->neuronCnt[i]);
        for (int j = 0; j < this->neuronCnt[i]; j++) {
            Neuron* currNeuron = &this->net[i][j];
            currNeuron->bias = (double)rand() / RAND_MAX * 2 - 1;
            if (i == this->layerCnt-1) {
                currNeuron->linkCnt = 0;
                currNeuron->link = NULL;
                currNeuron->weight = NULL;
                continue;
            }
            currNeuron->linkCnt = this->neuronCnt[i+1];
            currNeuron->link = malloc(sizeof(Neuron*) * this->neuronCnt[i+1]);
            currNeuron->weight = malloc(sizeof(double) * this->neuronCnt[i+1]);
            for (int k = 0; k < this->neuronCnt[i+1]; k++) {
                currNeuron->link[k] = &this->net[i+1][k];
                currNeuron->weight[k] = (double)rand()/RAND_MAX * 2 - 1;
            }
        }
    }
}

void NeuralNetworkFree(NeuralNetwork* this) {
    if (!this)
        return;
    for (int i = 0; i < this->layerCnt; i++) {
        for (int j = 0; j < this->neuronCnt[i]; j++) {
            free(this->net[i][j].link);
            free(this->net[i][j].weight);
        }
        free(this->net[i]);
    }
    free(this->neuronCnt);
}

void NeuralNetworkPropagate(NeuralNetwork* this, double* input) {

    if (this->layerCnt == 0)
        return;

    for (int i = 0; i < this->neuronCnt[0]; i++)
        NeuronPropagate(&this->net[0][i], input[i]);

    for (int i = 1; i < this->layerCnt; i++)
        for (int j = 0; j < this->neuronCnt[i]; j++)
            NeuronPropagate(&this->net[i][j], NO_VALUE);
}

double NeuralNetworkEvalCost(NeuralNetwork* this, double* expected) {

    if (this->layerCnt == 0)
        return 0;

    int lastLayer = this->layerCnt-1;
    double cost = 0;
    for (int i = 0; i < this->neuronCnt[lastLayer]; i++)
        cost += NeuronEvalCost(&this->net[lastLayer][i], expected[i]);

    return cost;
}

void NeuralNetworkBackPropagate(NeuralNetwork* this, double* expected, double rate) {
    
    if (this->layerCnt == 0)
        return;

    int lastLayer = this->layerCnt-1;
    for (int i = 0; i < this->neuronCnt[lastLayer]; i++)
        NeuronBackPropagate(&this->net[lastLayer][i], expected[i], rate);

    for (int i = lastLayer-1; i >= 0; i--) 
        for (int j = 0; j < this->neuronCnt[i]; j++)
            NeuronBackPropagate(&this->net[i][j], NO_VALUE, rate);
}

void NeuralNetworkDraw(NeuralNetwork* this, SDL_Renderer* renderer, int x, int y, int w, int h) {

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    int maxNeuronCnt = this->neuronCnt[0];
    for (int i = 1; i < this->layerCnt; i++)
        maxNeuronCnt = max(maxNeuronCnt, this->neuronCnt[i]);

    if ((NEURON_SZ + MIN_GAP) * maxNeuronCnt - MIN_GAP > h)
        maxNeuronCnt = h / (NEURON_SZ + MIN_GAP);

    int xGap = evalGap(w, NEURON_SZ, this->layerCnt);
    int yGap = evalGap(h, NEURON_SZ, maxNeuronCnt);

    int*** coord = malloc(sizeof(int**) * this->layerCnt);
    int currX = x;
    for (int i = 0; i < this->layerCnt; i++) {
        int displayNeuronCnt = min(maxNeuronCnt, this->neuronCnt[i]);
        coord[i] = malloc(sizeof(int*) * displayNeuronCnt);
        int currY = y + (h - (NEURON_SZ + yGap) * displayNeuronCnt + yGap) / 2;
        for (int j = 0; j < displayNeuronCnt; j++) {
            coord[i][j] = malloc(sizeof(int) * 2);
            coord[i][j][0] = currX;
            coord[i][j][1] = currY;
            currY += NEURON_SZ + yGap;
        }
        currX += NEURON_SZ + xGap;
    }

    for (int i = 0; i < this->layerCnt-1; i++) {
        int displayNeuronCnt = min(maxNeuronCnt, this->neuronCnt[i]);
        for (int j = 0; j < displayNeuronCnt; j++) {
            int nextDisplayNeuronCnt = min(maxNeuronCnt, this->neuronCnt[i+1]);
            for (int k = 0; k < nextDisplayNeuronCnt; k++) {
                SDL_Color color = NeuronEvalWeightColor(&this->net[i][j], k);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderDrawLine(
                    renderer,
                    coord[i][j][0] + NEURON_SZ/2, coord[i][j][1] + NEURON_SZ/2,
                    coord[i+1][k][0] + NEURON_SZ/2, coord[i+1][k][1] + NEURON_SZ/2
                );
            }
        }
    }
    for (int i = 0; i < this->layerCnt; i++) {
        int displayNeuronCnt = min(maxNeuronCnt, this->neuronCnt[i]);
        for (int j = 0; j < displayNeuronCnt; j++) {
            SDL_Color color = NeuronEvalColor(&this->net[i][j]);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &(SDL_Rect){
                coord[i][j][0], coord[i][j][1], NEURON_SZ, NEURON_SZ
            });
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &(SDL_Rect){
                coord[i][j][0], coord[i][j][1], NEURON_SZ, NEURON_SZ
            });
        }
        if (displayNeuronCnt < this->neuronCnt[i]) {
            int lastX = coord[i][displayNeuronCnt-1][0] + NEURON_SZ/2;
            int lastY = coord[i][displayNeuronCnt-1][1] + NEURON_SZ;
            SDL_RenderDrawPoints(
                renderer,
                (SDL_Point[]){
                    (SDL_Point){lastX, lastY + 5},
                    (SDL_Point){lastX, lastY + 10},
                    (SDL_Point){lastX, lastY + 15}
                }, 3
            );
        }
    }

    for (int i = 0; i < this->layerCnt; i++) {
        int displayNeuronCnt = min(maxNeuronCnt, this->neuronCnt[i]);
        for (int j = 0; j < displayNeuronCnt; j++)
            free(coord[i][j]);
        free(coord[i]);
    }
    free(coord);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
}

void NeuralNetworkDrawMeta(NeuralNetwork* this, Visualizer* visualizer, double* expected, int x, int y) {

    Neuron* lastLayer = this->net[this->layerCnt - 1];
    int neuronCnt = this->neuronCnt[this->layerCnt - 1];
    
    int* order = malloc(sizeof(int) * neuronCnt);
    for (int i = 0; i < neuronCnt; i++)
        order[i] = i;
    for (int i = neuronCnt-1; i > 0; i--)
        for (int j = 0; j < i; j++)
            if (lastLayer[order[j]].value < lastLayer[order[j+1]].value)
                swap(order[j], order[j+1]);
    
    double sum = 0;
    for (int i = 0; i < neuronCnt; i++)
        sum += lastLayer[i].value;
    
    SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
    if (expected[order[0]] > 0)
        color.r = color.b = 0;

    char percentage[32];
    for (int i = 0; i < neuronCnt; i++) {
        sprintf(percentage, "Label %d: %.3f (%.3f\%)", order[i], lastLayer[order[i]].value, lastLayer[order[i]].value / sum * 100);
        VisualizerDrawText(visualizer, percentage, x, y, color);
        y += 20;
    }

    char cost[32];
    sprintf(cost, "Cost: %f", NeuralNetworkEvalCost(this, expected));
    VisualizerDrawText(visualizer, cost, x, y, color);

    free(order);
}

