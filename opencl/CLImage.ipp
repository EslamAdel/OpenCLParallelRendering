#ifndef RENDERING_CLIMAGE_IPP
#define RENDERING_CLIMAGE_IPP

#include <Headers.hh>

template class CLImage< uint8_t >;
template class CLImage< uint16_t >;
template class CLImage< uint32_t >;
template class CLImage< uint64_t >;
template class CLImage< half >;
template class CLImage< float >;
template class CLImage< double >;

#endif // RENDERING_CLIMAGE_IPP

