#ifndef RENDERING_CLIMAGE_IPP
#define RENDERING_CLIMAGE_IPP

#include <Headers.hh>

namespace clparen {
namespace CLData {

template class CLFrame< uint8_t >;
template class CLFrame< uint16_t >;
template class CLFrame< uint32_t >;
template class CLFrame< half >;
template class CLFrame< float >;

}
}

#endif // RENDERING_CLIMAGE_IPP

