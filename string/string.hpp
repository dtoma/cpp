#pragma once

#include <iterator>
#include <memory>
#include <string>

/**
 * Implement bit-fiddling helper functions for the SSO.
 */
namespace
{

static std::size_t const high_bit_mask = static_cast<std::size_t>(1) << (sizeof(std::size_t) * CHAR_BIT - 1);
static std::size_t const sec_high_bit_mask = static_cast<std::size_t>(1) << (sizeof(std::size_t) * CHAR_BIT - 2);

template <typename T>
unsigned char *uchar_cast(T *p)
{
    return reinterpret_cast<unsigned char *>(p);
}

template <typename T>
unsigned char const *uchar_cast(T const *p)
{
    return reinterpret_cast<unsigned char const *>(p);
}

template <typename T>
unsigned char &most_sig_byte(T &obj)
{
    return *(reinterpret_cast<unsigned char *>(&obj) + sizeof(obj) - 1);
}

template <int N>
bool lsb(unsigned char byte)
{
    return byte & (1u << N);
}

template <int N>
bool msb(unsigned char byte)
{
    return byte & (1u << (CHAR_BIT - N - 1));
}

template <int N>
void set_lsb(unsigned char &byte, bool bit)
{
    if (bit)
    {
        byte |= 1u << N;
    }
    else
    {
        byte &= ~(1u << N);
    }
}

template <int N>
void set_msb(unsigned char &byte, bool bit)
{
    if (bit)
    {
        byte |= 1u << (CHAR_BIT - N - 1);
    }
    else
    {
        byte &= ~(1u << (CHAR_BIT - N - 1));
    }
}

} // namespace

/**
 * String class.
 * 
 * Features:
 * - SSO
 * - iterators
 * 
 * Non-features:
 * - templated value type/type traits
 * - templated allocator
 * 
 * This simple class should be usable by the STL algorithms,
 * be somewhat performant thanks to SSO, compile quickly,
 * and be easy to debug since it doesn't have template magic.
 * 
 * Inspiration: https://github.com/elliotgoodrich/SSO-23/blob/master/include/string.hpp
 * 
 * TODO:
 * - move constructors
 *   + rule of 5: https://en.cppreference.com/w/cpp/language/rule_of_three
 *   + assertions: https://howardhinnant.github.io/classdecl.html
 * - comparison operators
 * 
 */
class String final
{
    /**
     * SSO union.
     */
    union Data {
        struct NonSSO
        {
            char *ptr;
            std::size_t size;
            std::size_t capacity;
        } non_sso;
        struct SSO
        {
            char ptr[sizeof(NonSSO) / sizeof(char) - 1];
            std::make_unsigned_t<char> size;
        } sso;
    } _data;

    /**
     * SSO helper member functions.
     */
    std::pair<std::size_t, std::size_t> read_non_sso_data() const
    {
        auto size = this->_data.non_sso.size;
        auto capacity = this->_data.non_sso.capacity;

        auto &size_hsb = most_sig_byte(size);
        auto &cap_hsb = most_sig_byte(capacity);

        // Remember to negate the high bits
        auto const cap_high_bit = lsb<0>(cap_hsb);
        auto const size_high_bit = !lsb<1>(cap_hsb);
        auto const cap_sec_high_bit = msb<0>(size_hsb);

        set_msb<0>(size_hsb, size_high_bit);

        cap_hsb >>= 2;
        set_msb<0>(cap_hsb, cap_high_bit);
        set_msb<1>(cap_hsb, cap_sec_high_bit);

        return std::make_pair(size, capacity);
    }

    void set_sso_size(unsigned char size) noexcept
    {
        _data.sso.size = static_cast<value_type>(sso_capacity - size) << 2;
    }

    std::size_t sso_size() const
    {
        return sso_capacity - ((this->_data.sso.size >> 2) & 63u);
    }

public:
    /**
     * Types
     */
    using value_type = char;
    using traits_type = std::char_traits<value_type>;

    using size_type = std::size_t;

    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using const_pointer = value_type const *;
    using reference = value_type &;
    using const_reference = value_type const &;

    using iterator_category = std::random_access_iterator_tag;

    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static std::size_t const sso_capacity = //
        sizeof(typename Data::NonSSO) / sizeof(value_type) - 1;

    /**
     * Constructors & destructors.
     */
    String() : String("", 0){};

    String(char const *str, std::size_t size)
    {
        if (size <= sso_capacity)
        {
            traits_type::copy(this->_data.sso.ptr, str, size); // TODO: move?
            traits_type::assign(this->_data.sso.ptr[size], '\0');
            this->set_sso_size(size);
            return;
        }
        this->_data.non_sso.ptr = new value_type[size + 1];
        traits_type::copy(this->_data.non_sso.ptr, str, size); // TODO: move?
        traits_type::assign(this->_data.non_sso.ptr[size], '\0');

        auto capacity = size;

        auto &size_hsb = most_sig_byte(size);
        auto const size_high_bit = msb<0>(size_hsb);

        auto &cap_hsb = most_sig_byte(capacity);
        auto const cap_high_bit = msb<0>(cap_hsb);
        auto const cap_sec_high_bit = msb<1>(cap_hsb);

        set_msb<0>(size_hsb, cap_sec_high_bit);

        cap_hsb <<= 2;
        set_lsb<0>(cap_hsb, cap_high_bit);
        set_lsb<1>(cap_hsb, !size_high_bit);

        _data.non_sso.size = size;
        _data.non_sso.capacity = capacity;
    }

    String(char const *str)
        : String(str, std::strlen(str))
    {
    }

    String(String const &other)
    {
        if (other.sso())
        {
            this->_data.sso = other._data.sso;
        }
        new (this) String(other.data(), other.size());
    };

    String &operator=(String const &other)
    {
        auto tmp = other;
        std::swap(tmp, *this);
        return *this;
    };

    ~String()
    {
        if (!this->sso() && this->_data.non_sso.ptr != nullptr)
        {
            delete[] this->_data.non_sso.ptr;
        }
    };

    /**
     * Public interface.
     */
    char const *data() const noexcept
    {
        if (this->sso())
        {
            return this->_data.sso.ptr;
        }
        return this->_data.non_sso.ptr;
    }

    char *data() noexcept
    {
        if (this->sso())
        {
            return this->_data.sso.ptr;
        }
        return this->_data.non_sso.ptr;
    }

    std::size_t size() const noexcept
    {
        if (this->sso())
        {
            return this->sso_size();
        }
        return this->read_non_sso_data().first;
    }

    std::size_t capacity() const noexcept
    {
        if (sso())
        {
            return sizeof(_data) - 1;
        }
        return read_non_sso_data().second;
    }

    bool sso() const noexcept
    {
        auto last_bit = lsb<0>(this->_data.sso.size);
        auto second_last_bit = lsb<1>(this->_data.sso.size);
        return !last_bit && !second_last_bit;
    }

    /**
     * Iterator interface.
     */
    iterator begin()
    {
        return this->data();
    }

    iterator end()
    {
        return this->data() + this->size();
    }

    const_iterator begin() const
    {
        return this->data();
    }

    const_iterator end() const
    {
        return this->data() + this->size();
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(this->end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(this->begin());
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(this->end());
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(this->begin());
    }

    reference operator[](size_type pos)
    {
        return this->data()[pos];
    }

    const_reference operator[](size_type pos) const
    {
        return this->data()[pos];
    }
};

static_assert(std::is_destructible<String>{});
static_assert(std::is_default_constructible<String>{});
static_assert(std::is_copy_constructible<String>{});
static_assert(std::is_copy_assignable<String>{});
