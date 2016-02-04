#ifndef IMAGEVARIANT_HH
#define IMAGEVARIANT_HH
#include "boost/variant.hpp"
#include "Image.h"

/**
 * @brief ImageVariant
 * Since QObject or classes inheriting from QObject do not allow using
 * templates, So using boost.variant will provide the desired polymorphism
 *  instead of templates.
 */
typedef boost::variant< Image8, Image16, Image32, Image64,
                        ImageF, ImageD > ImageVariant;


#endif // IMAGEVARIANT_HH

