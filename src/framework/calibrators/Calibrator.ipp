#ifndef CALIBRATOR_IPP
#define CALIBRATOR_IPP


namespace clparen {
namespace Calibrator {

template class Calibrator< uchar ,uint8_t >;
template class Calibrator< uchar ,uint16_t >;
template class Calibrator< uchar ,uint32_t >;
template class Calibrator< uchar ,half  >;
template class Calibrator< uchar ,float > ;
}
}

#endif // CALIBRATOR_IPP
