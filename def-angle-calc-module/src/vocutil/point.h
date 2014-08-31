#ifndef POINT_H_
#define POINT_H_

#include "common-types.h"

namespace vocutil {

    class Point2D {
        Coordinate x_, y_;

    public:
        Point2D() : x_(0), y_(0) {
        }

        Point2D(Coordinate x, Coordinate y) : x_(x), y_(y) {
        }
        
        Point2D(const Point2D& point) : x_(point.x_), y_(point.y_) {
        }
        
        Coordinate x() const {
            return x_;
        }
        
        Coordinate y() const {
            return y_;
        }

        Point2D operator+(const Point2D& point) const {
            return Point2D(x_ + point.x_, y_ + point.y_);
        }

        Point2D operator-(const Point2D& point) const {
            return Point2D(x_ - point.x_, y_ - point.y_);
        }

        Point2D operator*(const Point2D& point) const {
            return Point2D(x_ * point.x_, y_ * point.y_);
        }

        Point2D operator*(Coordinate n) const {
            return Point2D(n * x_, n * y_);
        }
        
        Point2D operator/(Coordinate n) const {
            return Point2D(n / x_, n / y_);
        }

        bool operator==(const Point2D& point) const {
            return x_ == point.x_ && y_ == point.y_;
        }

        bool operator!=(const Point2D& point) const {
            return x_ != point.x_ || y_ != point.y_;
        }
    };

}

#endif /*POINT_H_*/
