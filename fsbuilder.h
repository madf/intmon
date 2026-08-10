#pragma once

#include "fstring.h"

#include <limits>
#include <type_traits>
#include <cstdint>

template <typename I>
struct FSIterator
{
    operator bool() const { return static_cast<const I&>(*this); }
    char operator*() const { return *static_cast<const I&>(*this); }
    void next() { static_cast<const I&>(*this).next(); }
};

enum class Padding : uint8_t { Yes, No };
inline constexpr Padding ForcePadding = Padding::Yes;

template <typename E>
struct FSExpr
{
    public:
        inline static constexpr uint8_t Size = E::Size;

        auto exec() const { return static_cast<const E&>(*this).exec(); }
        void putAt(char* pos) const { return static_cast<const E&>(*this).putAt(pos); }
        auto iter() const { return static_cast<const E&>(*this).iter(); }
        auto iter(Padding p) const { return static_cast<const E&>(*this).iter(p); }
};

template <uint8_t S>
class FSLit : public FSExpr<FSLit<S>>
{
    public:
        inline static constexpr uint8_t Size = S - 1;

        struct Iterator : FSIterator<Iterator>
        {
            explicit Iterator(const char* d) : data(d) {}
            const char* data;
            uint8_t pos = 0;

            operator bool() const { return pos < Size; }
            char operator*() const { return data[pos]; }
            void next() { ++pos; }
        };

        FSLit(const char (&data)[S]) : m_data(data) {}

        FString<S> exec() const { return FString<S>(m_data); }
        void putAt(char* pos) const { std::strncpy(pos, m_data, S); }
        Iterator iter() const { return Iterator(m_data); }
        Iterator iter(Padding /*p*/) const { return Iterator(m_data); }
    private:
        const char* m_data;
};

template <typename T>
consteval T pow10(uint8_t s)
{
    if (s == 0)
        return 1;
    if (s == 1)
        return 10;
    if (s == 2)
        return 100;
    if (s == 3)
        return 1000;
    T res = 1;
    while (s-- > 0)
        res *= 10;
    return res;
}

template <typename T, uint8_t S = std::numeric_limits<T>::digits10 + 1>
class FSNum : public FSExpr<FSNum<T, S>>
{
    static_assert(std::is_unsigned_v<T>, "FSNum can only format unsigned numbers.");
    public:
        inline static constexpr uint8_t Size = S;
        inline static constexpr T Pow = pow10<T>(S - 1);

        struct Iterator : FSIterator<Iterator>
        {
            explicit Iterator(T v, Padding p) : data(v)
            {
                if (p == Padding::No)
                    while (d > data) d /= 10;
            }
            T data;
            T d = Pow;

            operator bool() const { return d > 0; }
            char operator*() const { return static_cast<char>('0' + (data / d) % 10); }
            void next() { d /= 10; }
        };

        FSNum(T v, char fill = '0') : m_data(v), m_fill(fill) {}

        FString<S> exec() const
        {
            FString<S> res;
            putAt(&res[0]);
            return res;
        }
        void putAt(char* pos) const
        {
            T d = pow10<T>(S - 1);
            while (d > 0)
            {
                const auto dig = (m_data / d) % 10;
                *pos++ = dig == 0 ? m_fill : static_cast<char>('0' + dig);
                d /= 10;
            }
        }
        Iterator iter() const { return Iterator(m_data, Padding::No); }
        Iterator iter(Padding p) const { return Iterator(m_data, p); }
    private:
        T m_data;
        char m_fill;
};

template <typename E1, typename E2>
class FSApp : public FSExpr<FSApp<E1, E2>>
{
    public:
        inline static constexpr uint8_t Size = E1::Size + E2::Size;

        struct Iterator : FSIterator<Iterator>
        {
            Iterator(const E1::Iterator& l, const E2::Iterator& r) : lhs(l), rhs(r) {}
            E1::Iterator lhs;
            E2::Iterator rhs;

            operator bool() const { return lhs || rhs; }
            char operator*() const { return lhs ? *lhs : *rhs; }
            void next() { if (lhs) lhs.next(); else rhs.next(); }
        };

        FSApp(const E1& l, const E2& r) : m_lhs(l), m_rhs(r) {}

        FString<Size> exec() const
        {
            FString<Size> res;
            putAt(&res[0]);
            return res;
        }
        void putAt(char* pos) const
        {
            m_lhs.putAt(pos);
            m_rhs.putAt(&pos[E1::Size]);
        }
        Iterator iter() const { return Iterator(m_lhs.iter(), m_rhs.iter()); }
        Iterator iter(Padding p) const { return Iterator(m_lhs.iter(p), m_rhs.iter(p)); }
    private:
        const E1 m_lhs;
        const E2 m_rhs;
};

template <typename E1, typename E2>
inline
FSApp<E1, E2> operator+(const FSExpr<E1>& lhs, const FSExpr<E2>& rhs)
{
    return FSApp<E1, E2>(*static_cast<const E1*>(&lhs), *static_cast<const E2*>(&rhs));
}
