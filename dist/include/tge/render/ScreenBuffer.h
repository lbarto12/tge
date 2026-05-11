#pragma once
// IWYU pragma: private, include <tge/graphics.h>

#include "../models/util/Vector2.h"
#include "Color.h"
#include "Terminal.h"
#include <cwchar>
#include <string>
#include <utility>

namespace tge::render {

struct Pixel {
    wchar_t c = L' '; // No draw
    Color fg = Color::None;
    Color bg = Color::None;

    bool operator==(const Pixel& other) const {
        return this->c == other.c && this->fg == other.fg && this->bg == other.bg;
    }
    bool operator!=(const Pixel& other) const { return !(*this == other); }
};

class ScreenBuffer {
public:
    ScreenBuffer(const ScreenBuffer&) = delete;
    ScreenBuffer& operator=(const ScreenBuffer&) = delete;

    ScreenBuffer(const Vector2i& size) { this->SetBufferSize(size); }

    void DrawAtXY(const Vector2i& where, const Pixel& p) {
        if (!(0 <= where.x && where.x < size.x && 0 <= where.y && where.y < size.y)) return;
        this->next[where.y][where.x] = p;
    }

    void DrawStringAtXY(const Vector2i& where, const std::wstring& str, Color fg = Color::White,
                        Color bg = Color::None) {
        int x = 0;
        for (const wchar_t& c : str) {
            this->DrawAtXY({where.x + x, where.y}, {c, fg, bg});
            ++x;
        }
    }

    void SwapBuffer() {
#if defined(_WIN32)
        // Build entire frame as wide chars and flush with one WriteConsoleW call
        // dumb ahh windows.
        std::wstring out;
        out.reserve(static_cast<std::size_t>(this->size.x) * this->size.y * 24);

        auto appendN = [&out](const std::string& s) {
            for (unsigned char c : s) out += static_cast<wchar_t>(c);
        };

        wchar_t posbuf[32];
        for (int i = 0; i < this->size.y; ++i) {
            for (int j = 0; j < this->size.x; ++j) {
                if (!didPixelChange({j, i})) continue;
                const Pixel& p = this->next[i][j];
                out.append(posbuf, std::swprintf(posbuf, 32, L"\033[%d;%dH", i + 1, j + 1));
                appendN(RESET);
                if (p.fg != Color::None) appendN(fg(p.fg));
                if (p.bg != Color::None) appendN(bg(p.bg));
                out += p.c;
            }
        }
        appendN(RESET);
        DWORD written;
        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), out.data(), static_cast<DWORD>(out.size()), &written, nullptr);
#else
        for (int i = 0; i < this->size.y; ++i) {
            for (int j = 0; j < this->size.x; ++j) {
                if (!didPixelChange({j, i})) continue;
                const Pixel& p = this->next[i][j];
                Terminal::MoveTo(j, i);
                Terminal::ResetStyle();
                if (p.fg != Color::None) tge::render::Terminal::SetForeground(p.fg);
                if (p.bg != Color::None) tge::render::Terminal::SetBackground(p.bg);
                std::printf("%lc", static_cast<wint_t>(p.c));
            }
        }
        Terminal::ResetStyle();
        std::fflush(stdout);
#endif
        std::swap(this->buffer, this->next);
        clearBuffer(this->next);
    }

    void SetBufferSize(const Vector2i& size) {
        this->destroyBuffers();
        this->size = size;
        this->buffer = new Pixel*[size.y];
        this->next = new Pixel*[size.y];

        for (int i = 0; i < size.y; ++i) {
            this->buffer[i] = new Pixel[size.x];
            this->next[i] = new Pixel[size.x];
        }
    }

    ~ScreenBuffer() { this->destroyBuffers(); }

private:
    Vector2i size;
    Pixel** buffer = nullptr;
    Pixel** next = nullptr;

private:
    bool didPixelChange(const Vector2i& where) const {
        return this->next[where.y][where.x] != this->buffer[where.y][where.x];
    }

    void clearBuffer(Pixel** buf) {
        for (int i = 0; i < size.y; i++)
            for (int j = 0; j < size.x; j++) buf[i][j] = Pixel{};
    }

    void destroyBuffers() {
        if (!this->buffer || !this->next) return;
        for (int i = 0; i < size.y; ++i) {
            delete[] this->buffer[i];
            delete[] this->next[i];
        }
        delete[] this->buffer;
        delete[] this->next;
        this->buffer = nullptr;
        this->next = nullptr;
    }

public:
    static ScreenBuffer globalScreenBuffer;
};

// TODO: move to cpp
ScreenBuffer ScreenBuffer::globalScreenBuffer(tge::render::Terminal::Size());

} // namespace tge::render
