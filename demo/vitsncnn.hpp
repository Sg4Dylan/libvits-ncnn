#ifndef VITS_NCNN_H
#define VITS_NCNN_H

#include "stdint.h"
#include <iostream>
#include <vector>

using namespace std;

void InitNCNN();
void DestroyNCNN();

bool InitOpenJtalk();
bool TestGPU();
int CheckThreads();

wstring SplitSentence(wstring inputString);
vector<wstring> JapaneseCleaners(wstring inputString);

bool VitsInit(string path, uint8_t isVC, uint8_t isMulti, int32_t nVocab);
vector<float> VitsForward(vector<int> inputArray, uint8_t enableVulkan, uint8_t isMulti, int32_t sid, float noiseScale, float noiseScaleW, float lengthScale, int num_threads);

vector<uint8_t> ConvertToWave(const float* audio, uint64_t length, int32_t sr);

#endif