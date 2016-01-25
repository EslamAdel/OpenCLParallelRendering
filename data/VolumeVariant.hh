#ifndef VOLUMEVARIANT_HH
#define VOLUMEVARIANT_HH
#include "boost/variant.hpp"
#include "Volume.h"

typedef boost::variant< Volume8, Volume16, Volume32, Volume64,
                        VolumeF, VolumeD > VolumeVariant;



#endif // VOLUMEVARIANT_HH

