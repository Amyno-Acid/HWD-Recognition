#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SDL.h"
#include "SDL_ttf.h"

#include "visual.h"

#ifndef DATASET_H
#define DATASET_H

typedef unsigned char BYTE;
#define LABEL_NULL 10

#define IMAGE_SET_HEADER_SZ 16
#define LABEL_SET_HEADER_SZ 8

#define PIXEL_SZ 8

int flip(int n);

typedef struct ImageSet {
    int magic;
    int imageCnt;
    int rowCnt;
    int colCnt;
    FILE* file;
} ImageSet;

int ImageSetInit(ImageSet* this, const char* path);
void ImageSetFree(ImageSet* this);
BYTE* ImageSetFetch(ImageSet* this, int idx);

typedef struct LabelSet {
    int magic;
    int labelCnt;
    FILE* file;
} LabelSet;

int LabelSetInit(LabelSet* this, const char* path);
void LabelSetFree(LabelSet* this);
BYTE LabelSetFetch(LabelSet* this, int idx);

typedef struct TestManager {
    int idx;
    double* input;
    double* expected;
    ImageSet* imageSet;
    LabelSet* labelSet;
} TestManager;

int TestManagerInit(TestManager* this, const char* imageSetPath, const char* labelSetPath);
void TestManagerFree(TestManager* this);
bool TestManagerLoad(TestManager* this);
void TestManagerSeek(TestManager* this, int idx);
void TestManagerDraw(TestManager* this, Visualizer* visualizer, int x, int y);

#endif
