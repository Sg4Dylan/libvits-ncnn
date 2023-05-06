#include "utils.hpp"

using namespace std;

void testSingle(vector<vector<int>> readyInput) {
    // Vits init
    bool isInit = VitsInit("atri/", false, false, 40);
    wcout << endl << "isInit: " << isInit << endl;

    // Vits forward
    vector<float> infer;
    for (vector<int> inputSlice: readyInput) {
        auto res = VitsForward(
            inputSlice, 
            false, // 是否启用 Vulkan
            false, // 是否包含多 Speaker
            0,     // Speaker 序号
            .667,  // 噪声比例
            .8,    // 噪声偏差
            1.1,   // 时长比例
            1);    // 处理线程数
        wcout << L"Slice result length: " << res.size() << endl;
        infer.insert(infer.end(), res.begin(), res.end());
    }

    // Save
    saveToWav(infer, "output_single.wav");
}

void testMulti(vector<vector<int>> readyInput) {
    // Vits init
    bool isInit = VitsInit("365_epochs/", false, true, 40);
    wcout << endl << "isInit: " << isInit << endl;

    for (int i=0; i<7; i++) {
        // Vits forward
        vector<float> infer;
        for (vector<int> inputSlice: readyInput) {
            auto res = VitsForward(
                inputSlice, 
                false, // 是否启用 Vulkan
                true,  // 是否包含多 Speaker
                i,     // Speaker 序号
                .667,  // 噪声比例
                .8,    // 噪声偏差
                1.1,   // 时长比例
                1);    // 处理线程数
            wcout << L"Slice result length: " << res.size() << endl;
            infer.insert(infer.end(), res.begin(), res.end());
        }

        // Save
        saveToWav(infer, format("output_multi_{}.wav", i));
    }
    
}

int main(int argc, char** argv) {
    InitNCNN();
    InitOpenJtalk();

    locale::global(locale(""));

    wcout << L"Testing GPU: " << TestGPU() << endl;
    // wcout << L"Check avaliable thread count: " << CheckThreads() << endl;

    // Raw input
    wstring rawInput; // = L"こんにちは。"; // L"どんなことを言わせても、良いんだよ";
    wcout << "Please input: " << endl;
    getline(wcin, rawInput);
    wcout << "Your rawInput: " << rawInput << endl;

    // Clear input
    auto cleanInput = CleanInputs(rawInput);
    wcout << "cleanInput: " << cleanInput << endl << endl;

    // Convert input
    auto readyInput = ConvertSentenceToLabels(cleanInput);

    // Single-tone model test
    testSingle(readyInput);

    // Multi-tone model test
    testMulti(readyInput);

    DestroyNCNN();
}
