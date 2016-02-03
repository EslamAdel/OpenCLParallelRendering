#ifndef RENDERING_CLIMAGE_IPP
#define RENDERING_CLIMAGE_IPP

#include <Headers.hh>

template class CLFrame< uint8_t >;
template class CLFrame< uint16_t >;
template class CLFrame< uint32_t >;
template class CLFrame< uint64_t >;
template class CLFrame< half >;
template class CLFrame< float >;
template class CLFrame< double >;

#endif // RENDERING_CLIMAGE_IPP

