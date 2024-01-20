#pragma once

#include <cstdint>
#include <cstddef>

class Font
{
    public:
        static Font font6x8();
        static Font font7x10();
        static Font font11x18();
        static Font font16x26();

        size_t width() const { return m_width; }
        size_t height() const { return m_height; }
        const uint16_t* data() const { return m_data; }

    private:
        size_t m_width;
        size_t m_height;
        const uint16_t* m_data;

        Font(size_t w, size_t h, const uint16_t* d)
                : m_width(w),
                  m_height(h),
                  m_data(d)
        {}
};
