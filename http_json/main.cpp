#include <iostream>
#include <string>

/* Metaprogramming library, enables us to iterate over structure members. */

#include <boost/hana.hpp>
namespace hana = boost::hana;

/* Create and manipulate JSON objects. */

#include <nlohmann/json.hpp>
using json = nlohmann::json;

/* This function recursively visits a structure, converting its members to JSON values. */

template <typename T>
std::enable_if_t<hana::Struct<T>::value, json> to_json(T const &object)
{
    json j;

    hana::for_each(
        hana::keys(object),
        [&](auto name) {
            auto member = hana::at_key(object, name);
            auto const *name_str = hana::to<char const *>(name);
            auto constexpr is_struct = hana::trait<hana::Struct>(hana::typeid_(member));

            if constexpr (is_struct)
            {
                j[name_str] = to_json(member);
            }
            else
            {
                j[name_str] = member;
            }
        });

    return j;
}

/* This function recursively visits a structure, setting its members from JSON values. */

template <typename T>
std::enable_if_t<hana::Struct<T>::value, T> from_json(json const &j)
{
    T result;

    hana::for_each(
        hana::keys(result),
        [&](auto name) {
            auto &member = hana::at_key(result, name);
            auto const *name_str = hana::to<char const *>(name);
            auto is_struct = hana::trait<hana::Struct>(hana::typeid_(member));
            using member_t = std::remove_reference_t<decltype(member)>;

            if constexpr (is_struct)
            {
                member = from_json<member_t>(j[name_str]);
            }
            else
            {
                member = j[name_str];
            }
        });

    return result;
}

/* We define two structures, one includes the other. */

struct Car
{
    BOOST_HANA_DEFINE_STRUCT(Car,
                             (std::string, brand),
                             (std::string, model));
};

struct Person
{
    BOOST_HANA_DEFINE_STRUCT(Person,
                             (std::string, name),
                             (Car, car));
};

int main()
{
    Car c{"Audi", "A4"};
    Person p{"Foo", c};

    json expected = {
        {"name", "Foo"},
        {"car", {{"brand", "Audi"}, {"model", "A4"}}},
    };

    /* Serialize */

    json j = to_json(p);

    assert(j == expected);

    std::cout << j << std::endl; /* {"car":{"brand":"Audi","model":"A4"},"name":"Foo"} */

    /* Deserialize */

    auto c2 = from_json<Car>(json{
        {"brand", "Audi"},
        {"model", "A4"}});

    assert(c2.brand == c.brand);
    assert(c2.model == c.model);

    auto p2 = from_json<Person>(expected);

    assert(p2.name == p.name);
    assert(p2.car.brand == c.brand);
    assert(p2.car.model == c.model);
}
