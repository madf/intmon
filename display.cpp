#include "display.h"

bool Display::init()
{
    return sendCommand(0xAE) // Turn off

        && sendCommand({0xA8, 0x1F}) // Multiplex ratio (HEIGHT - 1 = 32 - 1 = 0x20 - 1 = 0x1F)
        && sendCommand({0x20, 0x00}) // Memory mode
        && sendCommand({0x21, 0x00, 0x7F}) // Width, 0-127
        && sendCommand({0x22, 0x00, 0x03}) // Height, pages 0-4

        && sendCommand(0x40) // Start line
        && sendCommand({0xD3, 0x00}) // Offset

        && sendCommand(0xA0) // Remap?
        && sendCommand(0xC0) // Scan inc?
        && sendCommand({0xDA, 0x02}) // Pins config?
        && sendCommand({0x81, 0x8F}) // Contrast

        && sendCommand(0xA4) // Resume to RAM
        && sendCommand(0xA6) // Normal display, 0xA7 - inverted

        && sendCommand({0xD5, 0x80}) // Display refresh freq
        && sendCommand({0xD9, 0xF1}) // ?
        && sendCommand({0xDB, 0x20}) // Voltage level?

        //&& sendCommand(0x2E) // Stop scroll

        && sendCommand({0x8D, 0x14}) // Chargepump?

        && sendCommand(0xAF); // Turn on
}

bool Display::update()
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

void Display::clear()
{
    for (auto& p : m_pages)
        for (auto& v : p)
            v = 0;
}

bool Display::printAt(uint8_t x, uint8_t y, const Font& font, const std::string& text, size_t interCharSpace)
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

bool Display::printCharAt(uint8_t x, uint8_t y, const Font& font, char c)
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

bool Display::bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, Color color)
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

bool Display::hline(uint8_t x, uint8_t y, uint8_t l, Color color)
{
    auto& p = m_pages[y / 8];
    const auto offset = y % 8;
    for (uint8_t i = 0; i < l; ++i)
    {
        if (x + i > 127)
            return false;
        if (color == Color::White)
            p[x + i] |= 1 << offset;
        else
            p[x + i] &= ~(1 << offset);
    }
    return true;
}

bool Display::vline(uint8_t x, uint8_t y, uint8_t l, Color color)
{
    for (uint8_t i = 0; i < l; ++i)
    {
        if (y + i > 31)
            return false;
        auto& p = m_pages[(y + i) / 8];
        const auto offset = (y + i) % 8;
        if (color == Color::White)
            p[x] |= 1 << offset;
        else
            p[x] &= ~(1 << offset);
    }
    return true;
}

bool Display::rect(uint8_t x, uint8_t y, uint8_t l, uint8_t h, Color color)
{
    return hline(x, y, l, color) &&
           hline(x, y + h - 1, l, color) &&
           vline(x, y, h, color) &&
           vline(x + l - 1, y, h, color);
}
