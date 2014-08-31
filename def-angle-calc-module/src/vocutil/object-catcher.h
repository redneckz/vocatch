#ifndef ANGLECATCHER_H_
#define ANGLECATCHER_H_

#include <jni.h>

#include "common-types.h"
#include "point.h"

namespace vocutil {

    class ObjectCatcher {
    public:
        struct ObjectParams {
            const Point2D position;
            const Angle angle;
        };

        virtual ObjectParams Catch(Pixel* pixels, Size width) = 0;

        virtual ~ObjectCatcher() {
        }

    };
}

#endif /*ANGLECATCHER_H_*/
