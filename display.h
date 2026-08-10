#pragma once

#include "i2cdev.h"
#include "fonts.h"
#include "fstring.h"
#include "fsbuilder.h"

#include <string>
#include <vector>
#include <array>
#include <cstdint>

class Display
{
    public:
        enum class Color
        {
            Black,
            White
        };
        enum class Background
        {
            Yes,
            No
        };
        static constexpr auto NoBG = Background::No;

        template <typename P>
        Display(P& port, uint8_t address)
            : m_dev(port, address)
        {
        }

        bool init();

        using Page = std::array<uint8_t, 128>;
        using Pages = std::array<Page, 32 / 8>;

        Pages& pages() { return m_pages; }

        bool update();

        void clear();

        template <typename E>
        bool printAt(uint8_t x, uint8_t y, const Font& font, const FSExpr<E>& text, size_t interCharSpace, Background bg)
        {
            auto pos = x;
            for (auto it = text.iter(); it; it.next())
            {
                if (bg == Background::Yes && pos != x)
                {
                    if (!bar(pos + font.width(), y, interCharSpace, font.height(), Color::Black))
                        return false;
                }
                if (!printCharAt(pos, y, font, *it))
                    return false;
                pos += font.width() + interCharSpace;
            }
            return true;
        }

        template <uint8_t S>
        bool printAt(uint8_t x, uint8_t y, const Font& font, const FString<S>& text, size_t interCharSpace = 1)
        {
            return printAt(x, y, font, text.data(), text.size(), interCharSpace, Background::Yes);
        }

        bool printAt(uint8_t x, uint8_t y, const Font& font, const std::string& text, size_t interCharSpace = 1) { return printAt(x, y, font, text, interCharSpace, Background::Yes); }
        bool printAt(uint8_t x, uint8_t y, const Font& font, const std::string& text, size_t interCharSpace, Background bg)
        {
            return printAt(x, y, font, text.c_str(), text.length(), interCharSpace, bg);
        }
        bool printAt(uint8_t x, uint8_t y, const Font& font, const char* text, size_t textSize, size_t interCharSpace, Background bg);
        bool printCharAt(uint8_t x, uint8_t y, const Font& font, char c);
        bool bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color);
        bool hline(uint8_t x, uint8_t y, uint8_t l, Color color);
        bool vline(uint8_t x, uint8_t y, uint8_t l, Color color);
        bool rect(uint8_t x, uint8_t y, uint8_t l, uint8_t h, Color color);
        bool line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, Color color);
        bool point(uint8_t x, uint8_t y, Color color);

    private:
        I2C::Device m_dev;
        Pages m_pages;

        bool sendCommand(const std::vector<uint8_t>& cmds)
        {
            return m_dev.writeRegs(0x00, cmds.data(), cmds.size());
        }
        bool sendCommand(uint8_t cmd)
        {
            return m_dev.writeReg(0x00, cmd);
        }

        bool sendData(const void* data, size_t size)
        {
            return m_dev.writeRegs(0x40, data, size);
        }
};
