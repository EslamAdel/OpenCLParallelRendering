#ifndef IMAGEVARIANT_HH
#define IMAGEVARIANT_HH
#include "boost/variant.hpp"
#include "Image.h"

typedef boost::variant< Image8, Image16, Image32, Image64,
                        ImageF, ImageD > ImageVariant;


#endif // IMAGEVARIANT_HH

