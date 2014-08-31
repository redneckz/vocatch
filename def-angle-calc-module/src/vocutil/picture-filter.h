#ifndef PICTUREFILTER_H_
#define PICTUREFILTER_H_

#include <memory>

#include "common-types.h"
#include "point.h"

using namespace std;

namespace vocutil {
    
    class PictureFilter {
    public:
        virtual void Filter(Pixel* pixels_in, Size width, Pixel* pixels_out) = 0;

        virtual ~PictureFilter() {
        }

    };

    class AbstractPictureFilter : public PictureFilter {
    public:
        struct FilterContext {
            const Pixel* pixels_in;
            const Size width;
            const Pixel* pixels_out;
            
            Pixel In(Coordinate x, Coordinate y) const {
                return pixels_in[y * width + x];
            }
            
            Pixel In(Point2D point) const {
                return pixels_in[point.y() * width + point.x()];
            }
            
            Size size() const {
                return width * width;
            }
        };
        
        AbstractPictureFilter() {
        }

        virtual void Filter(Pixel* pixels_in, Size width, Pixel* pixels_out) override {
            FilterContext context = {pixels_in, width, pixels_out};
            Start(context);
            Size slide_size = width * width;
            for (Index pixel_index = 0; pixel_index < slide_size; pixel_index++) {
                pixels_out[pixel_index] = FilterPixel(context,
                        pixel_index, Point2D(pixel_index % width, pixel_index / width),
                        pixels_in[pixel_index]);
            }
            End(context);
        }

    protected:
        virtual void Start(const FilterContext& context) {
        }
        
        virtual Pixel FilterPixel(const FilterContext& context, Index index, Point2D point, Pixel pixel) = 0;
        
        virtual void End(const FilterContext& context) {
        }

    public:
        virtual ~AbstractPictureFilter() {
        }

    };

    class RadialPictureFilter : public AbstractPictureFilter {
    private:
        Size radius_;
        
        Size width_;
        unique_ptr<Color[]> columns_;
        unique_ptr<Color[]> columns2_;
        Color prev_m_color_, prev_m_color2_;
        
    public:
        RadialPictureFilter(Size radius) : AbstractPictureFilter(), radius_(radius) {
        }

        virtual void Start(const FilterContext& context) override {
            AbstractPictureFilter::Start(context);
            if (width_ != context.width) {
                width_ = context.width;
                columns_.reset((Color*) new Color[context.width]);
                columns2_.reset((Color*) new Color[context.width]);
            }
        }

        virtual Pixel FilterPixel(const FilterContext& context, Index index, Point2D point0, Pixel pixel) override {
            Coordinate min_y = point0.y() - radius_;
            min_y = min_y >= 0 ? min_y : 0;
            Coordinate max_y = point0.y() + radius_;
            max_y = max_y < context.width ? max_y : context.width - 1;
            Size n = max_y - min_y + 1;
            Color m_color, m_color2;
            if (point0.x() == 0) {
                n *= radius_ + 1;
                m_color = 0;
                m_color2 = 0;
                for (long x = 0; x <= radius_; x++) {
                    RecalcXColumn(context, x, point0.y(), min_y, max_y);
                    m_color += columns_[x];
                    m_color2 += columns2_[x];
                }
            } else {
                Coordinate min_x = point0.x() - radius_;
                min_x = min_x >= 0 ? min_x : 0;
                Coordinate max_x = point0.x() + radius_;
                max_x = max_x < context.width ? max_x : context.width - 1;
                n *= max_x - min_x + 1;
                m_color = prev_m_color_;
                m_color2 = prev_m_color2_;
                if (min_x > 0) {
                    m_color -= columns_[min_x - 1];
                    m_color2 -= columns2_[min_x - 1];
                }
                if (min_x <= (context.width - (2 * radius_ + 1))) {
                    RecalcXColumn(context, max_x, point0.y(), min_y, max_y);
                    m_color += columns_[max_x];
                    m_color2 += columns2_[max_x];
                }
            }
            prev_m_color_ = m_color;
            prev_m_color2_ = m_color;
            m_color -= pixel;
            m_color2 -= pixel * pixel;
            m_color /= n - 1;
            m_color2 /= n - 1;
            Color sigma = m_color2 - m_color * m_color;
            FilterPixel(context, index, point0, pixel, m_color, sigma);
        }

        virtual Pixel FilterPixel(const FilterContext& context, Index index, Point2D point0,
                Pixel pixel, Color m_color, Color sigma) = 0;

    private:
        void RecalcXColumn(const FilterContext& context, Coordinate x, Coordinate y0,
                Coordinate min_y, Coordinate max_y) {
            if (y0 == 0) {
                columns_[x] = 0;
                columns2_[x] = 0;
                for (Coordinate y = min_y; y <= max_y; y++) {
                    IncXColumn(context, x, y);
                }
            } else {
                if (min_y > 0) {
                    Color color = context.In(x, min_y - 1);
                    columns_[x] -= color;
                    columns2_[x] -= color * color;
                }
                if (min_y <= (context.width - (2 * radius_ + 1))) {
                    IncXColumn(context, x, max_y);
                }
            }
        }

        void IncXColumn(const FilterContext& context, Coordinate x, Coordinate y) {
            Color color = context.In(x, y);
            columns_[x] += color;
            columns2_[x] += color * color;
        }
    };

}

#endif /*PICTUREFILTER_H_*/
