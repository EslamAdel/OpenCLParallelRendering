#ifndef CLFRAME_VARIANTS_HH
#define CLFRAME_VARIANTS_HH

#include <QVariant>

#include "clData.h"


Q_DECLARE_METATYPE( clpar::clData::CLFrame< float >* )
Q_DECLARE_METATYPE( clpar::clData::CLImage2D< float >* )

namespace clpar {
namespace clData {

typedef QVariant CLFrameVariant ;

}
}

#endif // CLFRAME_VARIANTS_HH
