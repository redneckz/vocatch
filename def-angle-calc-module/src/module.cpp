#include <windows.h>
#include <jni.h>

#include "vocutil/vocutil.h"
#include "def-picture-filter.h"
#include "binarization-filter.h"
#include "def-object-catcher.h"

using namespace std;
using namespace vocutil;

extern "C" {
    
    enum FilterDebugLevel : jbyte {
        NO_DEBUG = 0, PFILTER_DEBUG = 1, BFILTER_DEBUG = 2
    };

    JNIEXPORT void JNICALL Java_com_qbit_vocatch_modules_impl_DefaultCalcModule_work(
            JNIEnv* env, jobject self, jobject my_slide, jshort filter_radius, jbyte th_rigidity,
            FilterDebugLevel filter_debug_level) {
        jclass slide_class = env->GetObjectClass(my_slide);
        jfieldID slide_buffer_fid = env->GetFieldID(slide_class, "parent", "Lcom/qbit/vocatch/modules/impl/DefaultSlideBuffer;");
        jobject slide_buffer = env->GetObjectField(my_slide, slide_buffer_fid);
        jfieldID slide_offset_fid = env->GetFieldID(slide_class, "offset", "I");
        jint slide_offset = env->GetIntField(my_slide, slide_offset_fid);
        jfieldID slide_size_fid = env->GetFieldID(slide_class, "size", "I");
        jint slide_size = env->GetIntField(my_slide, slide_size_fid);
        jfieldID slide_angle_fid = env->GetFieldID(slide_class, "angle", "D");
        jfieldID slide_x_fid = env->GetFieldID(slide_class, "x", "I");
        jfieldID slide_y_fid = env->GetFieldID(slide_class, "y", "I");
        jclass slide_buffer_class = env->GetObjectClass(slide_buffer);
        jfieldID slide_width_fid = env->GetFieldID(slide_buffer_class, "slideWidth", "I");
        jint slide_width = env->GetIntField(slide_buffer, slide_width_fid);
        jfieldID pixels_fid = env->GetFieldID(slide_buffer_class, "pixels", "[B");
        jbyteArray pixels = (jbyteArray) (env->GetObjectField(slide_buffer, pixels_fid));
        
        static DoubleBuffer double_buffer;
        double_buffer.Write(DoubleBuffer::FRONT, env, pixels, slide_offset, slide_size);
        
        static DefaultPictureFilter p_filter(filter_radius);
        p_filter.Filter(double_buffer.front(), slide_width, double_buffer.back());
        
        if (filter_debug_level == PFILTER_DEBUG) {
            double_buffer.Read(DoubleBuffer::BACK, env, pixels, slide_offset, slide_size);
        }
        
        static BFilter b_filter(th_rigidity);
        b_filter.Filter(double_buffer.back(), slide_width, double_buffer.front());
        
        if (filter_debug_level == BFILTER_DEBUG) {
            double_buffer.Read(DoubleBuffer::FRONT, env, pixels, slide_offset, slide_size);
        }
        
        static DefaultObjectCatcher catcher;
        ObjectCatcher::ObjectParams object_params = catcher.Catch(double_buffer.front(), slide_width);
        
        env->SetIntField(my_slide, slide_x_fid, object_params.position.x());
        env->SetIntField(my_slide, slide_y_fid, object_params.position.y());
        env->SetDoubleField(my_slide, slide_angle_fid, object_params.angle);
    }

}
