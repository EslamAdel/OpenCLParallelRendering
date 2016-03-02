#ifndef CLFRAME_VARIANTS_HH
#define CLFRAME_VARIANTS_HH

#include <QVariant>

#include "CLFrame.h"
#include "CLImage2D.h"


Q_DECLARE_METATYPE( CLFrame< uint >* )
Q_DECLARE_METATYPE( CLImage2D< float >* )

typedef QVariant CLFrameVariant ;

#endif // CLFRAME_VARIANTS_HH
