#ifndef DEFPICTUREFILTER_H_
#define DEFPICTUREFILTER_H_

#include "vocutil/vocutil.h"

using namespace vocutil;

class DefaultPictureFilter : public RadialPictureFilter {
public:
    DefaultPictureFilter(Size radius) : RadialPictureFilter(radius) {
    }

    virtual Pixel FilterPixel(const FilterContext& context, Index index, Point2D point0,
            Pixel pixel, Color m_color, Color sigma) override {
        return (pixel - m_color) * (pixel - m_color) <= sigma ? pixel : m_color;
    }
};

#endif /*DEFPICTUREFILTER_H_*/
