#pragma once

#include "i2cdev.h"
#include "fonts.h"

#include <string>
#include <vector>
#include <array>
#include <cstdint>

template <typename P>
class Display
{
    public:
        enum class Color
        {
            Black,
            White
        };
        explicit Display(uint8_t address) : m_dev(address)
        {
            sendCommand(0xAE); // Turn off

            sendCommand({0xA8, 0x1F}); // Multiplex ratio (HEIGHT - 1 = 32 - 1 = 0x20 - 1 = 0x1F)
            sendCommand({0x20, 0x00}); // Memory mode
            sendCommand({0x21, 0x00, 0x7F}); // Width, 0-127
            sendCommand({0x22, 0x00, 0x03}); // Height, pages 0-4

            sendCommand(0x40); // Start line
            sendCommand({0xD3, 0x00}); // Offset

            sendCommand(0xA0); // Remap?
            sendCommand(0xC0); // Scan inc?
            sendCommand({0xDA, 0x02}); // Pins config?
            sendCommand({0x81, 0x8F}); // Contrast

            sendCommand(0xA4); // Resume to RAM
            sendCommand(0xA6); // Normal display, 0xA7 - inverted

            sendCommand({0xD5, 0x80}); // Display refresh freq
            sendCommand({0xD9, 0xF1}); // ?
            sendCommand({0xDB, 0x20}); // Voltage level?

            //sendCommand(0x2E); // Stop scroll

            sendCommand({0x8D, 0x14}); // Chargepump?

            sendCommand(0xAF); // Turn on
        }

        bool ready() noexcept { return m_dev.ready(); }

        using Page = std::array<uint8_t, 128>;
        using Pages = std::array<Page, 32 / 8>;

        Pages& pages() noexcept { return m_pages; }

        bool update() noexcept
        {
            for (uint8_t i = 0; i < 4; ++i)
            {
                if (!sendCommand({0xB0 + i, 0x00, 0x10}))
                    return false;
                const auto& p = m_pages[i];
                if (!sendData(p.data(), p.size()))
                    return false;
            }
            return true;
        }

        void clear() noexcept
        {
            for (auto& p : m_pages)
                for (auto& v : p)
                    v = 0;
        }
        bool printAt(uint8_t x, uint8_t y, const Font& font, const std::string& text, size_t interCharSpace = 1) noexcept
        {
            auto pos = x;
            for (auto c : text)
            {
                if (pos != x)
                {
                    if (!bar(pos + font.width(), y, interCharSpace, font.height(), Color::Black))
                        return false;
                }
                if (!printCharAt(pos, y, font, c))
                    return false;
                pos += font.width() + interCharSpace;
            }
            return true;
        }
        bool printCharAt(uint8_t x, uint8_t y, const Font& font, char c) noexcept
        {
            if (x + font.width() > 127 ||
                y + font.height() > 31)
                return false;
            for (size_t i = 0; i < font.height(); ++i)
            {
                const auto line = font.data()[(c - 32) * font.height() + i];
                auto& p = m_pages[(y + i) / 8];
                auto offset = (y + i) % 8;
                for (size_t j = 0; j < font.width(); ++j)
                {
                    if (((line << j) & 0x8000) == 0x8000)
                    {
                        p[x + j] |= 1 << offset;
                    }
                    else
                    {
                        p[x + j] &= ~(1 << offset);
                    }
                }
            }
            return true;
        }

        bool bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color) noexcept
        {
            bool res = true;
            for (uint8_t i = 0; i < h; ++i)
            {
                if (y + i > 31)
                        return false;
                for (uint8_t j = 0; j < w; ++j)
                {
                    if (x + j > 127)
                    {
                        res = false;
                        break;
                    }
                    auto& p = m_pages[(y + i) / 8];
                    auto offset = (y + i) % 8;
                    if (color == Color::White)
                        p[x + j] |= 1 << offset;
                    else
                        p[x + j] &= ~(1 << offset);
                }
            }
            return res;
        }

    private:
        I2C::Device<P> m_dev;
        Pages m_pages;

        bool sendCommand(const std::vector<uint8_t>& cmds) noexcept
        {
            return m_dev.writeRegs(0x00, cmds.data(), cmds.size());
        }
        bool sendCommand(uint8_t cmd)
        {
            return m_dev.writeReg(0x00, cmd);
        }

        bool sendData(const void* data, size_t size) noexcept
        {
            return m_dev.writeRegs(0x40, data, size);
        }
};
