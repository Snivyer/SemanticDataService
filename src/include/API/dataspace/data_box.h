#pragma once

#include "manager/metadata/cnt_meta.h"
#include "manager/metadata/sto_meta.h"

namespace SDS
{

    class DataBox {
    public:
        DataBox();
        ~DataBox();

        void init(ContentDesc &cntDesc);
        void addSpaceIndex(size_t start, size_t count);

    };



}