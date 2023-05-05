#include "utils.hpp"

using namespace std;

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
            1.1,    // 时长比例
            1);    // 处理线程数
        wcout << L"Slice result length: " << res.size() << endl;
        infer.insert(infer.end(), res.begin(), res.end());
    }
    
    // Convert to WAVE
    const float* audio = infer.data();
    uint64_t length = infer.size();
    auto result = ConvertToWave(audio, length, 22050);

    // Save to WAV
    ofstream file("output.wav", ios::out | ios::binary);

    // 检查文件是否打开成功
    if (file) {
      file.write(reinterpret_cast<const char*>(result.data()), result.size());
      file.close();
    } else {
      wcerr << L"无法打开文件" << endl;
    }

    DestroyNCNN();
}
