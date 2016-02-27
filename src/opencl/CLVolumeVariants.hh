#ifndef CLVOLUME_VARIANTS_HH
#define CLVOLUME_VARIANTS_HH

#include <QVariant>

#include "CLVolume.h"
#include "Volume.h"

Q_DECLARE_METATYPE( CLVolume< uchar >* )
Q_DECLARE_METATYPE( Volume< uchar >* )

typedef QVariant CLVolumeVariant ;
typedef QVariant VolumeVariant ;

#endif // CLVOLUME_VARIANTS_HH
