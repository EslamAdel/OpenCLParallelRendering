#ifndef CLGPUCONTEXT_IPP
#define CLGPUCONTEXT_IPP

#include <Headers.hh>

template class CLContext< uint8_t >;
template class CLContext< uint16_t >;
template class CLContext< uint32_t >;
template class CLContext< uint64_t >;
template class CLContext< half >;
template class CLContext< float >;
template class CLContext< double >;

#endif // CLGPUCONTEXT_IPP

