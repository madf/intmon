#pragma once

#include <array>
#include <cstdint>
#include <cstring>

/** @class FString
 *  @brief Fixed-size string.
 */
template <uint8_t S>
class FString
{
    public:
        inline static constexpr auto Size = S;

        FString() {}
        FString(uint8_t s, char fill) { std::fill_n(m_data.begin(), s, fill); }
        FString(const char* d) { assign(d); }
        FString(const FString& rhs) { assign(rhs); }
        FString(FString&& rhs) { assign(std::move(rhs)); }

        ~FString() = default;

        template <uint8_t Pos, uint8_t Count>
        void fillAt(char fill)
        {
            static_assert(Pos + Count <= Size);
            fillAtImpl(Pos, fill, Count);
        }

        template <uint8_t Pos>
        void setDigitAt(uint8_t v)
        {
            static_assert(Pos < Size);
            setDigitAtImpl(Pos, v);
        }

        void assign(const char* d) { std::strncpy(m_data.data(), d, Size); }
        void assign(const FString& rhs) { std::memcpy(m_data.data(), rhs.m_data.data(), Size); }
        void assign(FString&& rhs) { m_data.swap(rhs.m_data); }

        FString& operator=(const char* d) { assign(d); return *this; }
        FString& operator=(const FString& rhs) { assign(rhs); return *this; }
        FString& operator=(FString&& rhs) { assign(std::move(rhs)); return *this; }

        uint8_t size() const { return Size; }
        const char* data() const { return m_data.data(); }
        char* data() { return m_data.data(); }

        const char& operator[](uint8_t i) const { return m_data[i]; }
        char& operator[](uint8_t i) { return m_data[i]; }

        template <uint8_t RSize>
        FString<Size + RSize> append(const FString<RSize>& rhs) const
        {
            FString<Size + RSize> res;
            std::memcpy(res.data(), m_data.data(), Size);
            std::memcpy(&res[Size], rhs.data(), RSize);
            return res;
        }
        template <uint8_t RSize>
        FString<Size + RSize> append(FString<RSize>&& rhs) const
        {
            return append(rhs);
        }
        template <uint8_t RSize>
        FString<Size + RSize> append(const char (&rhs)[RSize]) const
        {
            FString<Size + RSize> res;
            std::memcpy(res.data(), m_data.data(), Size);
            std::memcpy(&res[Size], rhs, RSize);
            return res;
        }

        int compare(const char* rhs) const { return std::strncmp(m_data.data(), rhs, Size); }
        int compare(const FString& rhs) const { return std::strncmp(m_data.data(), rhs.data(), Size); }

    private:
        std::array<char, Size> m_data;

        void fillAtImpl(uint8_t pos, char fill, uint8_t count) { std::memset(&m_data[pos], fill, count); }
        void setDigitAtImpl(uint8_t pos, uint8_t v) { m_data[pos] = static_cast<char>('0' + v % 10); }
};

template <uint8_t LSize, uint8_t RSize>
inline
FString<LSize + RSize> operator+(const FString<LSize>& lhs, const FString<RSize>& rhs) { return lhs.append(rhs); }

template <uint8_t LSize, uint8_t RSize>
inline
FString<LSize + RSize> operator+(const FString<LSize>& lhs, FString<RSize>&& rhs) { return lhs.append(std::move(rhs)); }

template <uint8_t LSize, uint8_t RSize>
inline
FString<LSize + RSize> operator+(const FString<LSize>& lhs, const char (&rhs)[RSize]) { return lhs.append(rhs); }

template <uint8_t LSize, uint8_t RSize>
inline
FString<LSize + RSize> operator+(FString<LSize>&& lhs, const FString<RSize>& rhs) { return lhs.append(rhs); }

template <uint8_t LSize, uint8_t RSize>
inline
FString<LSize + RSize> operator+(FString<LSize>&& lhs, FString<RSize>&& rhs) { return lhs.append(std::move(rhs)); }

template <uint8_t LSize, uint8_t RSize>
inline
FString<LSize + RSize> operator+(FString<LSize>&& lhs, const char (&rhs)[RSize]) { return lhs.append(rhs); }

template <uint8_t Size>
inline
bool operator==(const FString<Size>& lhs, const FString<Size>& rhs)
{
    return lhs.compare(rhs) == 0;
}

template <uint8_t Size>
inline
bool operator==(const FString<Size>& lhs, const char* rhs)
{
    return lhs.compare(rhs) == 0;
}

template <uint8_t Size>
inline
int operator<=>(const FString<Size>& lhs, const FString<Size>& rhs)
{
    return lhs.compare(rhs);
}

template <uint8_t Size>
inline
int operator<=>(const FString<Size>& lhs, const char* rhs)
{
    return lhs.compare(rhs);
}
