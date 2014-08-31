#ifndef DEFANGLECATCHER_H_
#define DEFANGLECATCHER_H_

#include <cmath>
#include <vector>

#include "vocutil/common-types.h"
#include "vocutil/point.h"
#include "vocutil/object-catcher.h"

using namespace std;

using namespace vocutil;

class DefaultObjectCatcher : public ObjectCatcher {
    
    const Angle PI_2 = asin(1.0);
    const Angle PI_4 = atan(1.0);
    
    Size capacity_;
    unique_ptr<Point2D[]> object_points_;
    
public:
    DefaultObjectCatcher() : capacity_(0) {
    }
    
    ObjectParams Catch(Pixel* pixels, Size width) override {
        Size size = width * width;
        if (capacity_ < size) {
            capacity_ = size;
            object_points_.reset(new Point2D[size]);
        }
        
        Point2D object_points_acc;
        Size object_point_count = 0;
        for (Index pixel_index = 0; pixel_index < size; pixel_index++) {
            Coordinate x = pixel_index % width, y = pixel_index / width;
            if (pixels[pixel_index]) {
                Point2D object_point(x, y);
                object_points_acc = object_points_acc + object_point;
                object_points_[object_point_count] = object_point;
                object_point_count++;
            }
        }
        
        if (object_point_count == 0) {
            return {Point2D(), 0.0};
        }
        
        Point2D object_position = object_points_acc / object_point_count;
        Moment m11 = 0, m20_02 = 0;
        for (Index object_point_index = 0; object_point_index < object_point_count; object_point_index++) {
            Point2D rel_point = object_points_[object_point_index] - object_position;
            m11 += rel_point.x() * rel_point.y();
            m20_02 += rel_point.x() * rel_point.x() - rel_point.y() * rel_point.y();
        }
        
        if (m20_02 != 0) {
            Angle angle = 0.5 * atan(double(2 * m11) / double(m20_02));
            ObjectParams result = {object_position, m20_02 > 0 ? angle - copysign(PI_2, m11) : angle};
            return result;
        } else {
            ObjectParams result = {object_position, copysign(PI_4, m11)};
            return result;
        }
    }
};

#endif /*DEFANGLECATCHER_H_*/
