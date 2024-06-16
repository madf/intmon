#pragma once

#include "i2cdev.h"
#include "fonts.h"

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

        bool printAt(uint8_t x, uint8_t y, const Font& font, const std::string& text, size_t interCharSpace = 1);
        bool printCharAt(uint8_t x, uint8_t y, const Font& font, char c);
        bool bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color);
        bool hline(uint8_t x, uint8_t y, uint8_t l, Color color);
        bool vline(uint8_t x, uint8_t y, uint8_t l, Color color);
        bool rect(uint8_t x, uint8_t y, uint8_t l, uint8_t h, Color color);

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
