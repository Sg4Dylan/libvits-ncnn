#ifndef MOERENG_WAVE_H
#define MOERENG_WAVE_H

#include <cstdint>
#include <vector>
#include <string>
#include <string.h>

// convert pcm audio to wav format
char* PCMToWavFormat(float* audio, size_t length, int sampling_rate);

#endif //MOERENG_WAVE_H
