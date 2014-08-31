#ifndef BINARIZATIONFILTER_H_
#define BINARIZATIONFILTER_H_

#include <array>

#include "vocutil/vocutil.h"

using namespace vocutil;

class BFilter : public PictureFilter {
    
    static const Color MAX_Y_LEVEL = 0xFF;

    Color th_rigidty_;

    array<Color, MAX_Y_LEVEL + 1> y_histogram_;

public:

    BFilter(Color th_rigidty = 75) : th_rigidty_(th_rigidty) {
    }
    
    virtual void Filter(Pixel* pixels_in, Size width, Pixel* pixels_out) override {
        y_histogram_.fill(0);
        Size slide_size = width * width;
        for (Index pixel_index = 0; pixel_index < slide_size; pixel_index++) {
            Pixel pixel = pixels_in[pixel_index];
            y_histogram_[pixel] = y_histogram_[pixel] + 1;
        }
        
        Threshold threshold = CalcThreshold(width);
        for (Index pixel_index = 0; pixel_index < slide_size; pixel_index++) {
            if (threshold.IsBody(pixels_in[pixel_index])) {
                pixels_out[pixel_index] = MAX_Y_LEVEL;
            } else {
                pixels_out[pixel_index] = 0;
            }
        }
    }
    
private:
    struct Threshold {
        const bool dark_body;
        const Color threshold;
        
        bool IsBody(Pixel pixel) const {
            return (dark_body && pixel <= threshold) || (!dark_body && pixel > threshold);
        }
    };
    
    Threshold CalcThreshold(Size width) {
        Color threshold = MAX_Y_LEVEL / 2;
        Color delta_th;
        Color m1, m2;
        Size m1_pixel_count, m2_pixel_count;
        do {
            m1 = 0;
            m1_pixel_count = 0;
            for (Color y_level = 0; y_level < threshold; y_level++) {
                Size pixel_count = y_histogram_[y_level];
                m1 += pixel_count * y_level;
                m1_pixel_count += pixel_count;
            }
            if (m1_pixel_count) {
                m1 /= m1_pixel_count;
            }
            m2 = 0;
            m2_pixel_count = 0;
            for (Color y_level = threshold; y_level <= MAX_Y_LEVEL; y_level++) {
                Size pixel_count = y_histogram_[y_level];
                m2 += pixel_count * y_level;
                m2_pixel_count += pixel_count;
            }
            if (m2_pixel_count) {
                m2 /= m2_pixel_count;
            }
            Color new_th = (m1 + m2) / 2;
            delta_th = new_th - threshold;
            threshold = new_th;
        } while (-1 > delta_th || delta_th > 1);
        
        Size slide_size = width * width;
        bool dark_body = m1_pixel_count <= slide_size / 2;
        if (dark_body) {
            Threshold result = {dark_body, threshold * th_rigidty_ / 100};
            return result;
        } else {
            Threshold result = {dark_body, threshold * (200 - th_rigidty_) / 100};
            return result;
        }
    }

};

#endif /*BINARIZATIONFILTER_H_*/
