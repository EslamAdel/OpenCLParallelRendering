#ifndef CLCOMPOSITOR_ACCUMULATE_IPP
#define CLCOMPOSITOR_ACCUMULATE_IPP

namespace clparen {
namespace Compositor {

template class CLCompositorAccumulate< uint8_t >;
template class CLCompositorAccumulate< uint16_t >;
template class CLCompositorAccumulate< uint32_t >;
template class CLCompositorAccumulate< half  >;
template class CLCompositorAccumulate< float > ;

}
}

#endif // CLCOMPOSITOR_ACCUMULATE_IPP

