#ifndef UTILS_H
#define UTILS_H

#include "vitsncnn.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <regex>

#include <clocale>

using namespace std;

wstring CleanInputs(wstring text);
map<wstring, int64_t> GetSymbols();
vector<wstring> CutLens(const wstring& input);
vector<wstring> SplitString(wstring text, wchar_t delim);
bool ContainsString(wstring text, wstring sub);
wstring SymbolsToJapanese(const wstring& text);
vector<wstring> SplitMarkText(const wstring& text);
vector<wstring> FindMarks(const wstring& text);

wstring JapaneseToRomaji(wstring text);
wstring JapaneseCleanText1(wstring text);
wstring JapaneseCleanText2(wstring text);

vector<int> WordsToLabels(wstring text, int cleanerType);
vector<vector<int>> ConvertSentenceToLabels(wstring text);

void saveToWav(vector<float> infer, string savePath);

#endif