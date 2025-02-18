#include "abstract/adaptor/pluge_factory.h"

namespace SDS {

    NCPluge* PlugeFactory::getNCPluge() {
        return new NCPluge();
    }

    SDS::HDFPluge* PlugeFactory::getHDFPluge() {
        return new HDFPluge();
    }
}