#ifndef DOUBLEBUFFER_H_
#define DOUBLEBUFFER_H_

#include <memory>

#include <jni.h>

#include "common-types.h"

using namespace std;

namespace vocutil {

    class DoubleBuffer {
    public:
        enum BufferType {
            FRONT, BACK
        };

    private:
        Size size_;
        Size capacity_;
        unique_ptr<Pixel[]> front_;
        unique_ptr<Pixel[]> back_;

    public:
        DoubleBuffer() : size_(0), capacity_(0) {
        }

        DoubleBuffer(Size size) : DoubleBuffer() {
            Alloc(size);
        }

        Size size() const {
            return size_;
        }
        
        Size capacity() const {
            return capacity_;
        }

        Pixel* front() const {
            return front_.get();
        }

        Pixel* back() const {
            return back_.get();
        }

        void Alloc(Size size) {
            size_ = size;
            if (capacity_ < size) {
                capacity_ = size;
                front_.reset(new Pixel[size]);
                back_.reset(new Pixel[size]);
            }
        }

        void Write(BufferType type, JNIEnv* env, jbyteArray byte_array, jsize offset, jsize size) {
            Alloc(size);
            env->GetByteArrayRegion(byte_array, offset, size, Get(type));
        }

        void Read(BufferType type, JNIEnv* env, jbyteArray byte_array, jsize offset, jsize size) {
            Alloc(size);
            env->SetByteArrayRegion(byte_array, offset, size, Get(type));
        }

    private:
        Pixel* Get(BufferType type) const {
            switch (type) {
                case FRONT: return front_.get();
                case BACK: return back_.get();
            }
        }
    };
}

#endif /*DOUBLEBUFFER_H_*/
