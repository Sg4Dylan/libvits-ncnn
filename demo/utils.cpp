#include "utils.hpp"

vector<wstring> rawSymbols = {L"_", L",", L".", L"!", L"?", L"-", L"A", L"E", L"I", L"N", L"O", L"Q", L"U", L"a", L"b", L"d", L"e", L"f", L"g", L"h", L"i", L"j", L"k", L"m", L"n", L"o", L"p", L"r", L"s", L"t", L"u", L"v", L"w", L"y", L"z",L"ʃ",L"ʧ",L"↓",L"↑",L" "};

map<wstring, wstring> symbolsToJapanese = {
    {L"％",L"パーセント"},
    {L"%",L"パーセント"}
};
wregex japaneseMarks(L"[^A-Za-z\\d\\u3005\\u3040-\\u30ff\\u4e00-\\u9fff\\uff11-\\uff19\\uff21-\\uff3a\\uff41-\\uff5a\\uff66-\\uff9d]");
wregex japaneseCharacters(L"[A-Za-z\\d\\u3005\\u3040-\\u30ff\\u4e00-\\u9fff\\uff11-\\uff19\\uff21-\\uff3a\\uff41-\\uff5a\\uff66-\\uff9d]");

wstring CleanInputs(wstring text) {
    text = regex_replace(text, wregex(L"\""), L"");
    text = regex_replace(text, wregex(L"\'"), L"");
    text = regex_replace(text, wregex(L"\t"), L"");
    text = regex_replace(text, wregex(L"\n"), L"、");
    text = regex_replace(text, wregex(L"”"), L"");
    return text;
}

map<wstring, int64_t> GetSymbols() {
    map<wstring, int64_t> _symbol;
    for (int i=0; i<rawSymbols.size(); i++)
        _symbol.insert(pair<wstring, int64_t>(rawSymbols[i], i));
    return _symbol;
}

vector<wstring> CutLens(const wstring& input) {
    vector<wstring> _Lens;
    wstring _tmpLen;
    for (const auto& chari : input)
    {
        if ((chari == L'\n') || (chari == L'\r')) {
            if (!_tmpLen.empty())
            {
                _Lens.push_back(_tmpLen);
                _tmpLen = L"";
            }
        }
        else {
            _tmpLen += chari;
        }
    }
    return _Lens;
}

vector<wstring> SplitString(wstring text, wchar_t delim) {
    wstringstream sstr(text);
    vector<wstring> v;
    while (sstr.good()) {
        wstring substr;
        getline(sstr, substr, delim);
        // wcout << L"substr=" << substr << endl;
        v.push_back(substr);
    }
    return v;
}

bool ContainsString(wstring text, wstring sub) {
    if (text.find(sub) != wstring::npos) {
        return true;
    }
    return false;
}

wstring SymbolsToJapanese(const wstring& text) {
    wstring result = text;
    for (const auto& s : symbolsToJapanese) {
        wregex pattern(s.first);
        result = regex_replace(result, pattern, s.second);
    }
    return result;
}

vector<wstring> SplitMarkText(const wstring& text) {
    vector<wstring> sentences;
    regex_token_iterator<wstring::const_iterator> it(text.begin(), text.end(), japaneseMarks, -1);
    regex_token_iterator<wstring::const_iterator> end;
    while (it != end) {
        sentences.push_back(*it++);
    }
    return sentences;
}

vector<wstring> FindMarks(const wstring& text) {
    vector<wstring> marks;
    regex_iterator<wstring::const_iterator> it(text.begin(), text.end(), japaneseMarks);
    regex_iterator<wstring::const_iterator> end;
    while (it != end) {
        marks.push_back(it->str());
        ++it;
    }
    return marks;
}

wstring JapaneseToRomaji(wstring text) {
    auto symbolText = SymbolsToJapanese(text);
    auto sentences = SplitMarkText(symbolText);
    auto marks = FindMarks(symbolText);

    wstring res;
    for (int i=0; i<sentences.size(); i++) {
        auto sentence = sentences[i];
        if(regex_search(sentence, japaneseCharacters)) {
            if (res.length() > 0) {
                res += L" ";
            }
            auto labels = JapaneseCleaners(sentence);
            for (int j=0; j<labels.size(); j++) {
                auto label = labels[j];
                // L41
                wsmatch match;
                wregex pattern(L"\\-([^\\+]*)\\+");
                if (regex_search(label, match, pattern)) {
                    wstring phoneme = match[1].str();
                    if (phoneme != L"sil" && phoneme != L"pau") {
                        phoneme = regex_replace(phoneme, wregex(L"ch"), L"ʧ");
                        phoneme = regex_replace(phoneme, wregex(L"sh"), L"ʃ");
                        phoneme = regex_replace(phoneme, wregex(L"cl"), L"Q");
                        res += phoneme;
                    } else {
                        continue;
                    }
                }
                // L48
                int a1 = 0, a2 = 0, a3 = 0, a2_next = 0;
                if (regex_search(label, match, wregex(L"/A:(\\-?[0-9]+)\\+"))) {
                    a1 = stoi(match[1].str());
                }
                if (regex_search(label, match, wregex(L"\\+(\\d+)\\+"))) {
                    a2 = stoi(match[1].str());
                }
                if (regex_search(label, match, wregex(L"\\+(\\d+)/"))) {
                    a3 = stoi(match[1].str());
                }
                wstring t;
                if (regex_search(labels[j + 1], match, wregex(L"\\-([^\\+]*)\\+"))) {
                    t = match[1].str();
                }
                if (t == L"sil" || t == L"pau") {
                    a2_next = -1;
                } else {
                    if (regex_search(labels[j + 1], match, wregex(L"\\+(\\d+)\\+"))) {
                        a2_next = stoi(match[1].str());
                    }
                }
                // Accent phrase boundary
                if (a3 == 1 && a2_next == 1) {
                    res += L' ';
                } else if (a1 == 0 && a2_next == a2 + 1) {
                    res += L"↓";
                } else if (a2 == 1 && a2_next == 2) {
                    res += L"↑";
                }
            }
        }
        if (i < marks.size()) {
            wstring normalized = marks[i];
            // TODO: 使用第三方库来实现字符串规范化 (TN)
            normalized = regex_replace(normalized, wregex(L" "), L"");
            text += normalized;
        }
    }
    res += L".";
    return res;
}

wstring JapaneseCleanText1(wstring text) {
    wstring _text = JapaneseToRomaji(text);
    wstring cleaned = regex_replace(_text, wregex(L"([A-Za-z])$"), L"$1.");
    return cleaned;
}

wstring JapaneseCleanText2(wstring text) {
    wstring _text = JapaneseCleanText1(text);
    wstring cleaned = regex_replace(_text, wregex(L"ts"), L"ʦ");
    cleaned = regex_replace(cleaned, wregex(L"\\.\\.\\."), L"…");
    cleaned = regex_replace(cleaned, wregex(L"、"), L",");
    vector<wstring> brackets = {L"(", L")", L"{", L"}", L"[", L"]", L"<", L">", L"*"};
    for (const auto& b : brackets) {
        if (cleaned.find(b) != wstring::npos) {
            cleaned = regex_replace(cleaned, wregex(b), L"");
        }
    }
    return cleaned;
}

vector<int> WordsToLabels(wstring text, int cleanerType) {
    vector<int> labels;
    labels.push_back(0);

    // Symbol to ID
    auto symbolIdx = GetSymbols();

    // Clean text
    wstring cleanedText;

    if (cleanerType == 1) {
        cleanedText = JapaneseCleanText1(text);
    } else {
        cleanedText = JapaneseCleanText2(text);
    }

    // Symbol to text
    for (wchar_t symbol: cleanedText) {
        wstring s(1, symbol);
        if (!symbolIdx.contains(s)) {
            continue;
        }
        auto label = symbolIdx.at(s);
        labels.push_back(label);
        labels.push_back(0);
    }

    return labels;
}

vector<vector<int>> ConvertSentenceToLabels(wstring text) {
    // Split sentence
    auto sentences = SplitSentence(text);
    sentences = regex_replace(sentences, wregex(L"EOS\n"), L"");

    // Split into vector
    auto sentenceVec = SplitString(sentences, L'\n');
    sentenceVec.pop_back();
    for (wstring slice: sentenceVec) {
        wcout << "sentenceVec slice: " << slice << endl;
    }
    wcout << endl;

    // Labels
    wstring sentence;
    vector<vector<int>> res;
    for (int i=0; i<sentenceVec.size(); i++) {
        auto slice = sentenceVec[i];
        sentence += SplitString(slice, '\t')[0];
        
        
        if (ContainsString(slice, L"記号,読点") ||
            ContainsString(slice, L"記号,句点") ||
            ContainsString(slice, L"記号,一般") ||
            ContainsString(slice, L"記号,空白") ||
            i == sentenceVec.size() - 1
            ) {
                if (sentence.size() > 100) {
                    cout << "句子过长: " << sentence.size() << endl;
                }
                auto labels = WordsToLabels(sentence, 1);
                if (labels.size() < 1) {
                    continue;
                }
                wcout << "sentence: " << sentence << endl;
                wcout << "label: ";
                for (int l : labels) {
                    wcout << l << L", ";
                }
                wcout << endl;
                res.push_back(labels);
                sentence = L"";
        }
    }
    return res;
}

void saveToWav(vector<float> infer, string savePath) {
    // Convert to WAVE
    const float* audio = infer.data();
    uint64_t length = infer.size();
    auto result = ConvertToWave(audio, length, 22050);

    // Save to WAV
    ofstream file(savePath, ios::out | ios::binary);

    // 检查文件是否打开成功
    if (file) {
      file.write(reinterpret_cast<const char*>(result.data()), result.size());
      file.close();
    } else {
      wcerr << L"无法打开文件" << endl;
    }
}
