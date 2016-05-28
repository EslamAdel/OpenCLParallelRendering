#ifndef CLRENDERER_IPP
#define CLRENDERER_IPP

namespace clparen {
namespace Renderer {

template class CLRenderer< uint8_t , uint8_t >;
template class CLRenderer< uint8_t , uint16_t >;
template class CLRenderer< uint8_t , uint32_t >;
template class CLRenderer< uint8_t , half  >;
template class CLRenderer< uint8_t , float > ;

}
}
#endif // CLRENDERER_IPP
