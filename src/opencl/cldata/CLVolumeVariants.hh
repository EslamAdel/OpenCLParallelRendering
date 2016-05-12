#ifndef CLVOLUME_VARIANTS_HH
#define CLVOLUME_VARIANTS_HH

#include <QVariant>
#include "clData.h"
#include "Volume.h"


Q_DECLARE_METATYPE( clparen::clData::CLVolume< uchar >* )
Q_DECLARE_METATYPE( Volume< uchar >* )


namespace clparen {
namespace clData {

typedef QVariant CLVolumeVariant ;

}
}

typedef QVariant VolumeVariant ;

#endif // CLVOLUME_VARIANTS_HH
