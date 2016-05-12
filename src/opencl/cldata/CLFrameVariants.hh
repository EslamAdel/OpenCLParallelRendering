#ifndef CLFRAME_VARIANTS_HH
#define CLFRAME_VARIANTS_HH

#include <QVariant>

#include "clData.h"


Q_DECLARE_METATYPE( clparen::clData::CLFrame< float >* )
Q_DECLARE_METATYPE( clparen::clData::CLImage2D< float >* )

namespace clparen {
namespace clData {

typedef QVariant CLFrameVariant ;

}
}

#endif // CLFRAME_VARIANTS_HH
