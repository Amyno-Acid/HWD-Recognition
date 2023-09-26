#include "save.h"

int NeuralNetworkSave(NeuralNetwork* this, const char* path) {

    FILE* file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "Failed to write to file: %s\n", path);
        return -1;
    }

    fwrite(&this->layerCnt, sizeof(int), 1, file);
    fwrite(this->neuronCnt, sizeof(int), this->layerCnt, file);

    for (int i = 0; i < this->layerCnt; i++) {
        for (int j = 0; j < this->neuronCnt[i]; j++) {
            fwrite(&this->net[i][j].bias, sizeof(double), 1, file);
            fwrite(this->net[i][j].weight, sizeof(double), this->net[i][j].linkCnt, file);
        }
    }

    fclose(file);
    return 0;
}

int NeuralNetworkLoad(NeuralNetwork* this, const char* path) {

    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to read from file: %s\n", path);
        return -1;
    }

    int layerCntTemp;
    int* neuronCntTemp = malloc(sizeof(int) * this->layerCnt);
    fread(&layerCntTemp, sizeof(int), 1, file);
    fread(neuronCntTemp, sizeof(int), layerCntTemp, file);
    NeuralNetworkInit(this, layerCntTemp, neuronCntTemp);
    free(neuronCntTemp);

    for (int i = 0; i < this->layerCnt; i++) {
        for (int j = 0; j < this->neuronCnt[i]; j++) {
            fread(&this->net[i][j].bias, sizeof(double), 1, file);
            fread(this->net[i][j].weight, sizeof(double), this->net[i][j].linkCnt, file);
        }
    }
    
    fclose(file);
    return 0;
}

