#include "BorderedRectangle.h"
#include <string>

namespace tge {
class Input : public tge::BorderedRectangle {
public:
    void Init() override {
        this->SetSize({50, 3});
        this->SetBorderFromStyle(tge::BorderedRectangle::Style::RoundedLine);
    }

    void Update() override {
        if (auto kev = tge::Keyboard::GetKeyPressed()) {
            if (kev.key == tge::Key::Backspace)
                input.pop_back();
            else
                input += kev.chr;
        }

        const int lines = input.length() / (this->GetSize().x - 2) + 1;
        this->SetSize({this->GetSize().x, lines + 2});
    }

    void Render() override {
        tge::BorderedRectangle::Render();

        auto lines = splitLinesByWidth();

        for (size_t i = 0; i < lines.size(); ++i) {
            render.DrawStringAtXY(this->GetPosition() + tge::Vector2i{1, int(i) + 1}, lines[i]);
        }
    }

    std::wstring GetValue() const { return input; }
    std::wstring SetValue(const std::wstring& s) {
        this->input = s;
        return this->input;
    }

private:
    std::wstring input = L"";

    std::vector<std::wstring> splitLinesByWidth() {
        std::vector<std::wstring> result;
        std::wstring current = L"";
        for (size_t i = 0; i < input.length(); ++i) {
            auto c = input[i];
            if (current.size() > 0 && i % (this->GetSize().x - 2) == 0) {
                result.push_back(current);
                current = L"";
            }
            current += c;
        }

        result.push_back(current);

        return result;
    }
};

} // namespace tge
