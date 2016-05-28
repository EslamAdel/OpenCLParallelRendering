#ifndef SORTFIRSTRENDERER_IPP
#define SORTFIRSTRENDERER_IPP

namespace clparen
{
namespace Parallel
{

template class SortFirstRenderer< uint8_t , uint8_t >;
template class SortFirstRenderer< uint8_t , uint16_t >;
template class SortFirstRenderer< uint8_t , uint32_t >;
template class SortFirstRenderer< uint8_t , half  >;
template class SortFirstRenderer< uint8_t , float > ;
}
}

#endif // SORTFIRSTRENDERER_IPP
