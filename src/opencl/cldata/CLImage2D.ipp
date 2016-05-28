#ifndef CLIMAGE2D_IPP
#define CLIMAGE2D_IPP

#include <Headers.hh>

namespace clparen {
namespace CLData {

template class CLImage2D< uint8_t >;
template class CLImage2D< uint16_t >;
template class CLImage2D< uint32_t >;
template class CLImage2D< half >;
template class CLImage2D< float >;

}
}
#endif // CLIMAGE2D_IPP
