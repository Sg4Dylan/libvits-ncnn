#include "vitsncnn.hpp"

static ncnn::UnlockedPoolAllocator g_blob_pool_allocator;
static ncnn::PoolAllocator g_workspace_pool_allocator;
static SynthesizerTrn* net_g = nullptr;
static OpenJtalk openJtalk;
static Option opt;

void InitNCNN() {
    ncnn::create_gpu_instance();
}

void DestroyNCNN() {
    ncnn::destroy_gpu_instance();
    if (net_g != nullptr){
        delete net_g;
        net_g = nullptr;
    }
}

bool InitOpenJtalk() {
    return openJtalk.init("open_jtalk_dic_utf_8-1.11");
}

bool TestGPU() {
    int n = get_gpu_count();
    if (n != 0) return true;
    return false;
}

int CheckThreads() {
    return ncnn::get_physical_big_cpu_count();
}

wstring SplitSentence(wstring inputString) {
    string stext = utf8_encode(inputString);
    string res = openJtalk.words_split(stext.c_str());
    return utf8_decode(res);
    // cout << "SplitSentence result: " << res << endl;
}

vector<wstring> JapaneseCleaners(wstring inputString) {
    auto features = openJtalk.run_frontend(inputString);
    auto labels = openJtalk.make_label(features);
    return labels;
}

bool VitsInit(string path, uint8_t isVC, uint8_t isMulti, int32_t nVocab) {
    if (net_g != nullptr) {
        delete net_g;
        net_g = nullptr;
    }

    wcout << endl << "Model dir: " << utf8_decode(path) << endl;

    net_g = new SynthesizerTrn();

    opt.lightmode = true;
    opt.use_packing_layout = true;
    opt.num_threads = get_big_cpu_count();
    opt.blob_allocator = &g_blob_pool_allocator;
    opt.workspace_allocator = &g_workspace_pool_allocator;

    // use vulkan compute
    if (ncnn::get_gpu_count() != 0) {
        opt.use_vulkan_compute = true;
    }

    return net_g->init(path.c_str(), isVC, isMulti, nVocab, opt);
}

vector<float> VitsForward(vector<int> inputArray, uint8_t enableVulkan, uint8_t isMulti, int32_t sid, float noiseScale, float noiseScaleW, float lengthScale, int num_threads) {
    if (net_g == nullptr) return vector<float>();

    // jarray to ncnn mat
    Mat data(inputArray.size(), 1);
    for (int i = 0; i < data.c; i++) {
        float *p = data.channel(i);
        for (int j = 0; j < inputArray.size(); j++) {
            p[j] = (float) inputArray[j];
        }
    }

    // inference
    opt.num_threads = num_threads;
    auto output = net_g->forward(data, opt, enableVulkan, isMulti, sid,
                                 noiseScale, noiseScaleW, lengthScale);
    if (output.empty()) {
        vector<float>();
    }
    auto outputLen = output.w * output.h;
    vector<float> res {(float*)output.data, (float*)output.data + outputLen};

    return res;
}

vector<uint8_t> ConvertToWave(const float* audio, uint64_t length, int32_t sr) {
    auto wave = PCMToWavFormat((float*) audio, length, sr);
    if (wave == nullptr) return vector<uint8_t>();
    auto waveLen = length*4 + 44;
    vector<uint8_t> res {(uint8_t*)wave, (uint8_t*)wave + waveLen};
    return res;
}

