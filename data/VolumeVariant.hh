#ifndef VOLUMEVARIANT_HH
#define VOLUMEVARIANT_HH
#include "boost/variant.hpp"
#include "Volume.h"


/**
 * @brief VolumeVariant
 * Since QObject or classes inheriting from QObject do not allow using
 * templates, So using boost.variant will provide the desired polymorphism
 * instead of templates.
 */
typedef boost::variant< Volume8, Volume16, Volume32, Volume64,
                        VolumeF, VolumeD > VolumeVariant;



#endif // VOLUMEVARIANT_HH

