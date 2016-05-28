#ifndef CLFRAME_VARIANTS_HH
#define CLFRAME_VARIANTS_HH

#include <QVariant>



Q_DECLARE_METATYPE( clparen::CLData::CLFrame< uint8_t >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< uint16_t >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< uint32_t >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< half >* )
Q_DECLARE_METATYPE( clparen::CLData::CLFrame< float >* )



namespace clparen {
namespace CLData {

typedef QVariant CLFrameVariant ;

}
}

#endif // CLFRAME_VARIANTS_HH
