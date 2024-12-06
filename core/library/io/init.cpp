#include "library.hpp"
#include "common/constext.hpp"



namespace library {
CoreLibrary io("io", [](lua_State* L) -> int {
    return 1;
});
}
