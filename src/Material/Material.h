#pragma once

#include <Util/Array.h>
#include <Util/Status.h>
#include <Util/Precision.h>

namespace Material
{
    using Id = unsigned int;
    using Count = unsigned int;

    // Material types
    enum Type{ LINEAR_ELASTIC, TYPE_COUNT };
    struct TypeData{
        Array<int> num_params;
    };
    extern TypeData type_data;

    Status defineTypes();
}

