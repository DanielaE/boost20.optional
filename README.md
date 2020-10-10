# boost20.optional
An educational C++20 implementation of Boost.Optional that also is-a C++20 std::optional.

The primary motivation for this implementation is the exploration of these C++20 features:
  * *concepts*
  * *restricted templates* instead of SFINAE and `std::enable_if` to control overload sets
  * *modules* and implementation techniques regarding their various flavours
  * *three-way comparison*

This implementation of Boost.Optional offers these major features:
  * single header
  * conformant to the specification of `Boost.Optional`
  * conformant to the C++20 specification of `std::optional`
  * the namespace is configurable, by default it is `boost`
  * the underlying optional base class is configurable, by default it is `std::optional`
  * can be used as a *legacy header* `#include <optional/optional.hpp>`
  * can be compiled into a *header module* and used as `import <optional/optional.hpp>;`
  * can be compiled into a *named module* `boost.optional` and used as `import boost.optional;`
    This is what people should aim for!
  * the module name is configurable
  
So far, MSVC 16.8-pre3 is capable of compiling all module flavours and the assorted examples. Clang trunk and
gcc 10 accept the code at least as `#include`, I couldn't yet figure out how to compile it as modules on Compiler Explorer.

## How to use it
All you actually need is in directory `optional`:
  * `optional.hpp` is the full implementation of
    * `optional<T>`
    * `optional<T &>`
    * the related free functions and deduction guides
    
    as specified by C++20 and Boost
  * `optional.cpp` is the minimal module interface unit source, a stub translation unit required by the syntactic rules of C++20.

    It is both
      * *universal*, because it may turn every single-header-only library into a module given sufficient preparation.
      * *minimal*, because you cannot get away with fewer *logical lines* and *preprocessing tokens* to do so.
  
Directory `VisualStudio` contains a solution and projects to build 
  * a named module (with *invisible* details)
  * a header module (with *invisible* details)
  * an example using `optional` as a legacy include (with unavoidably *visible* details)
  * an example using `optional` as an imported header module
  * an example using `optional` as an imported named module
