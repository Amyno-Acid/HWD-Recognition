#include "dataset.h"

int flip(int n) {
    int ret = 0;
    do {
        ret = (ret << 8) | (n & 0xFF);
    } while (n >>= 8);
    return ret;
}

int ImageSetInit(ImageSet* this, const char* path) {

    this->file = fopen(path, "rb");
    if (!this->file) {
        fprintf(stderr, "Failed to open image set: %s\n", path);
        return -1;
    }
    fread(this, sizeof(int), 4, this->file);
    this->magic = flip(this->magic);
    this->imageCnt = flip(this->imageCnt);
    this->rowCnt = flip(this->rowCnt);
    this->colCnt = flip(this->colCnt);

    if (this->magic != 2051) {
        fprintf(stderr, "Not a image set file: %s\n", path);
        return -1;
    }

    return 0;
}

void ImageSetFree(ImageSet* this) {
    if (!this)
        return;
    fclose(this->file);
    free(this);
}

BYTE* ImageSetFetch(ImageSet* this, int idx) {

    if (!(0 <= idx && idx < this->imageCnt)) {
        fprintf(stderr, "Image index out of bound: %d\n", idx);
        return NULL;
    }

    const int imageSz = this->rowCnt * this->colCnt;
    BYTE* ret = malloc(imageSz);

    int offset = IMAGE_SET_HEADER_SZ + imageSz * idx;
    fseek(this->file, offset, SEEK_SET);
    fread(ret, 1, imageSz, this->file);
    
    return ret;
}

int LabelSetInit(LabelSet* this, const char* path) {

    this->file = fopen(path, "rb");
    if (!this->file) {
        fprintf(stderr, "Failed to open label set: %s\n", path);
        return -1;
    }
    fread(this, sizeof(int), 2, this->file);
    this->magic = flip(this->magic);
    this->labelCnt = flip(this->labelCnt);

    if (this->magic != 2049) {
        fprintf(stderr, "Not a label set file: %s\n", path);
        return -1;
    }

    return 0;
}

void LabelSetFree(LabelSet* this) {
    if (!this)
        return;
    fclose(this->file);
    free(this);
}

BYTE LabelSetFetch(LabelSet* this, int idx) {
    
    if (!(0 <= idx && idx < this->labelCnt)) {
        fprintf(stderr, "Label index out of bound: %d\n", idx);
        return LABEL_NULL;
    }

    BYTE ret;
    int offset = LABEL_SET_HEADER_SZ + idx;
    fseek(this->file, offset, SEEK_SET);
    fread(&ret, 1, 1, this->file);

    return ret;
}

int TestManagerInit(TestManager* this, const char* imageSetPath, const char* labelSetPath) {
    this->idx = 0;
    this->input = NULL;
    this->expected = NULL;
    this->imageSet = malloc(sizeof(ImageSet));
    this->labelSet = malloc(sizeof(LabelSet));
    if (ImageSetInit(this->imageSet, imageSetPath)) {
        fprintf(stderr, "Failed to initialize image set: %s\n", imageSetPath);
        return -1;
    }
    if (LabelSetInit(this->labelSet, labelSetPath)) {
        fprintf(stderr, "Failed to initialize label set: %s\n", labelSetPath);
        return -1;
    }
    if (this->imageSet->imageCnt != this->labelSet->labelCnt) {
        fprintf(
            stderr, "Number of images and labels don't match: %d - %d",
            this->imageSet->imageCnt,
            this->labelSet->labelCnt
        );
        return -1;
    }
    return 0;
}

void TestManagerFree(TestManager* this) {
    if (!this)
        return;
    free(this->input);
    free(this->expected);
    ImageSetFree(this->imageSet);
    LabelSetFree(this->labelSet);
}

bool TestManagerLoad(TestManager* this) {

    if (this->idx == this->imageSet->imageCnt) {
        return 0;
    }

    free(this->input);
    free(this->expected);
    
    BYTE* pixels = ImageSetFetch(this->imageSet, this->idx);
    BYTE label = LabelSetFetch(this->labelSet, this->idx);

    const int imageSz = this->imageSet->rowCnt * this->imageSet->colCnt;
    this->input = malloc(sizeof(double) * imageSz);
    for (int i = 0; i < imageSz; i++)
        this->input[i] = pixels[i] / 255.;
    this->expected = calloc(10, sizeof(double));
    this->expected[label] = 1;

    ++this->idx;
    free(pixels);
    return 1;
}

void TestManagerSeek(TestManager* this, int idx) {
    
    if (!(0 <= idx && idx < this->imageSet->imageCnt)) {
        fprintf(stderr, "Test index out of bound");
        return;
    }

    this->idx = idx;
}

void TestManagerDraw(TestManager* this, Visualizer* visualizer, int x, int y) {

    if (!this->input || !this->expected) {
        fprintf(stderr, "No test loaded\n");
        return;
    }

    char progress[32];
    sprintf(progress, "Test: %d/%d", this->idx, this->imageSet->imageCnt);
    VisualizerDrawText(visualizer, progress, x, y, (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    y += 20;
    char label[32];
    for (int i = 0; i < 10; i++)
        if (this->expected[i] == 1) {
            sprintf(label, "Label: %d", i);
            break;
        }
    VisualizerDrawText(visualizer, label, x, y, (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE});

    y += 20;
    const int imageSz = this->imageSet->rowCnt * this->imageSet->colCnt;
    for (int i = 0; i < imageSz; i++) {
        int alpha = (int)(255 * this->input[i]);
        int row = i / this->imageSet->rowCnt;
        int col = i % this->imageSet->colCnt;
        SDL_SetRenderDrawColor(visualizer->renderer, alpha, alpha, alpha, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(visualizer->renderer, &(SDL_Rect){
            x + col * PIXEL_SZ, y + row * PIXEL_SZ, PIXEL_SZ, PIXEL_SZ
        });
    }
    SDL_SetRenderDrawColor(visualizer->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect(visualizer->renderer, &(SDL_Rect){
        x, y, PIXEL_SZ * this->imageSet->colCnt, PIXEL_SZ * this->imageSet->rowCnt
    });

    SDL_SetRenderDrawColor(visualizer->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
}
