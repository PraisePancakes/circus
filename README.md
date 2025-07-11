# Circus : A C++ Text Serialization Framework

## Design Motives
This project is designed to seperate serialization into two distinguished, easy steps, with a flexible api for developers.

 ## <a name="arch"></a> Archives
In Circus, archives can be seperated into two basic categories. 
 
* Output (Serialization)
    The output archive is the ```circus::serializer``` object which takes any [std::ostream](https://cplusplus.com/reference/ostream/ostream/) object as its argument. The output archive is in charge of streaming your source data objects into whichever [std::ostream](https://cplusplus.com/reference/ostream/ostream/) object you passed. This will stream your source data into a circus formatted file.

* Input (Deserialization)
    The input archive on the contrary is the ```circus::deserializer``` object which takes any [std::istream](https://cplusplus.com/reference/istream/istream/) object as its argument. The input archive is in charge of streaming your 
    [std::istream](https://cplusplus.com/reference/istream/istream/) object into a ```c++ circus::circ_object ``` this object holds all the retrievable data of any circus formatted file. 

<ins>**Serialization API**</ins>

In Circus, serialization follows an api design similar to [Cereal](https://uscilab.github.io/cereal/), you must first instantiate the circus::serializer object that takes [any valid output archive](#arch). This will be your archive. 
use the CIRCUS_ENTRY() macro to create a key-value pair with the key being the identifier passed and the value being its respective value.

```c++
namespace private_subjects {
struct C {
    int x = 0;
    int y = 0;

    C() {};
    template <typename Ar>
    void serialize(Ar &ar) {
        ar(CIRCUS_ENTRY(x), y);
    }
    ~C() {};
};

struct T {
    int x = 2;
    int y = 4;
    const std::string v = "hi";
    C c;
    T(int x, int y) : x(x), y(y) {};
    template <typename Ar>
    void serialize(Ar &ar) {
        ar(CIRCUS_ENTRY(x), y, v, c);
    }
    ~T() {};
};
}  // namespace private_subjects

int main() {
    private_subjects::T t(5555, 5555);
    int x = 3;
    std::vector<int> vec{0, 1, 2, 3, 4};

    circus::serializer out_archive(std::cout);

    out_archive(CIRCUS_ENTRY(t), CIRCUS_ENTRY(x), CIRCUS_ENTRY(vec));
};

```

<ins>**Deserialization API**</ins>

Circus deserialization is very intuitive and trivial, 
simply instantiate a ``` circus::deserializer ``` object and pass in [any valid input archive](#arch) 

```c++

int main() {
    std::ifstream ifs{"examples/mycircformat.circ"};
    circus::deserializer dearch(ifs);
    //get implicitely which checks if good otherwise returns default.
    int v = dearch["root"]["x"];
    std::cout << v << std::endl;

    //get explicitely which checks if good otherwise throws std::bad_variant_access.
    auto v = dearch["root"]["x"].value<int>();

    return 0;
}

```

## Circus Format

**Circus (.circ)** is a minimal, readable serialization format designed for structured data representation. It supports nested objects, arrays, and a variety of primitive types with a clean and compact syntax.

### Circ format example

```
$root : {
    $x : 3,
    $y : "hi",
    $z : {
        $x : 5555,
        $y : 1001010
    },
    $arr : [1, 2, 3]
}
```
**Declarations**

A circus grammar is as shown :
```
Declaration  ::= "$" Identifier ":" Primary
Primary      ::= Integer
              |  Boolean
              |  String
              |  Object
              |  Array

Identifier   ::= /* a valid identifier: [a-zA-Z_][a-zA-Z0-9_]* */

Integer      ::= /* a sequence of digits, e.g., 123 */
Boolean      ::= "true" | "false"
String       ::= '"' /* any character except '"' or with escape */ '"'
Object       ::= "{" { Declaration ("," Declaration)* } "}"
Array        ::= "[" [ Primary ("," Primary)* ] "]"
```

each file will be serialized with the ```$root``` entry denoting the outermost 'root' directory of all serialized objects. Thus it is important that you access (due to change soon) your data beginning with the "root" directory.

```c++
int value = dearch["root"]["my_int"];
```

# Build
This project is built using CMAKE version 3.14, with GCC 13.2.0 x86_64-w64-mingw32
using the ISO C++20 Standard.

To build, from the root folder, input these commands if not done already.
```bash
  mkdir build 
  cd build
  cmake -G "<Preferred-Generator>" ..
  cmake --build .
  ./CIRCUS
```

# Testing
This project contains tests via Doctest, to run the tests, from the build folder input :
```bash
    ctest --rerun-failed --output-on-failure
```

# Want more insight?
[check out the developer docs here](https://praisepancakes.github.io/circus/)


