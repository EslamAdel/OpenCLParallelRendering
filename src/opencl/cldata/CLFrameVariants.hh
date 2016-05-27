#ifndef CLFRAME_VARIANTS_HH
#define CLFRAME_VARIANTS_HH

#include <QVariant>

#include "CLData.hh"


Q_DECLARE_METATYPE( clparen::CLData::CLFrame< float >* )
Q_DECLARE_METATYPE( clparen::CLData::CLImage2D< float >* )

namespace clparen {
namespace CLData {

typedef QVariant CLFrameVariant ;

}
}

#endif // CLFRAME_VARIANTS_HH
