#ifndef CLIMAGE2DARRAY_IPP
#define CLIMAGE2DARRAY_IPP

#include <Headers.hh>

namespace clparen {
namespace CLData {


template class CLImage2DArray< uint8_t >;
template class CLImage2DArray< uint16_t >;
template class CLImage2DArray< uint32_t >;
template class CLImage2DArray< half >;
template class CLImage2DArray< float >;
}
}
#endif // CLIMAGE2DARRAY_IPP

