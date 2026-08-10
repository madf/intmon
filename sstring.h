#pragma once

#include <array>
#include <cstdint>

/** @class SString
 *  @brief Stack-allocated string.
 */
template <uint8_t cap>
class SString
{
    public:
        inline static constexpr auto Capacity = cap;

        SString() : m_size(0) {}
        SString(uint8_t s, char fill) : m_size(s) { std::fill_n(m_data.begin(), s, fill); }
        SString(const char* d) : m_size(0) { for (; m_size < cap && d[m_size] != '\0'; ++m_size) m_data[m_size] = d[m_size]; --m_size; }
        SString(const SString& rhs) : m_size(rhs.m_size) { std::copy_n(rhs.m_data.begin(), rhs.m_size, m_data.begin()); }
        SString(SString&& rhs) : m_size(rhs.m_size) { m_data.swap(rhs.m_data); }

        SString& operator=(const char* d) { m_size = 0; for(; m_size < cap && d[m_size] != '\0'; ++m_size) m_data[m_size] = d[m_size]; ++m_size; return *this; }
        SString& operator=(const SString& rhs) { m_size = rhs.m_size; std::copy_n(rhs.m_data.begin(), rhs.m_size, m_data.begin()); return *this; }
        SString& operator=(SString&& rhs) { m_size = rhs.m_size; m_data.swap(rhs.m_data); return *this; }

        uint8_t size() const { return m_size; }
        const char* data() const { return m_data.data(); }
        char* data() { return m_data.data(); }

        const char& operator[](uint8_t i) const { return m_data[i]; }
        char& operator[](uint8_t i) { return m_data[i]; }

        void append(const char* d);
        void append(const char* d, uint8_t s);
        void append(const SString& rhs);
        void append(SString&& rhs);

    private:
        std::array<char, cap> m_data;
        uint8_t size;
};
