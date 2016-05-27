#ifndef CLVOLUME_VARIANTS_HH
#define CLVOLUME_VARIANTS_HH

#include <QVariant>
#include "CLData.hh"
#include "Volume.h"


Q_DECLARE_METATYPE( clparen::CLData::CLVolume< uchar >* )
Q_DECLARE_METATYPE( Volume< uchar >* )


namespace clparen {
namespace CLData {

typedef QVariant CLVolumeVariant ;

}
}

typedef QVariant VolumeVariant ;

#endif // CLVOLUME_VARIANTS_HH
