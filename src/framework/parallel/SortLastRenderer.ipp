#ifndef SORTLASTRENDERER_IPP
#define SORTLASTRENDERER_IPP

namespace clparen {
namespace Parallel {

template class SortLastRenderer< uint8_t , uint8_t >;
//template class SortLastRenderer< uint8_t , uint16_t >;
//template class SortLastRenderer< uint8_t , uint32_t >;
//template class SortLastRenderer< uint8_t , half  >;
template class SortLastRenderer< uint8_t , float > ;

}
}


#endif // SORTLASTRENDERER_IPP
