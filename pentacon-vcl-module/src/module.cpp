#include <iostream>
#include <rvcl.h>
#include <jni.h>
#include <windows.h>

using namespace std;

class RawBuffer {
    unsigned long size;
    unsigned char* rawBufferPtr;

public:

    RawBuffer() {
        size = 0;
        rawBufferPtr = NULL;
    }

    unsigned long getSize() {
        return size;
    }

    unsigned char* update(unsigned long newSize) {
        if (newSize != size) {
            free();
            size = newSize;
            rawBufferPtr = new unsigned char[size];
        }
        return rawBufferPtr;
    }
private:

    void free() {
        if (rawBufferPtr != NULL) {
            delete[] rawBufferPtr;
        }
        size = 0;
        rawBufferPtr = NULL;
    }
public:

    ~RawBuffer() {
        free();
    }

};

BOOL authOk = FALSE;

BOOL authorize();

jlong getStartTime();

extern "C" {

    JNIEXPORT jlong JNICALL Java_com_qbit_vocatch_modules_impl_PentaconVCLModule_work(
            JNIEnv* env, jobject self, jobject mySlideBuffer, jint offset, jint count, jbyte vfIndex) {
        jclass slideBufferClass = env->GetObjectClass(mySlideBuffer);
        jfieldID slideCountFID = env->GetFieldID(slideBufferClass, "slideCount", "I");
        jint slideBufferSlideCount = env->GetIntField(mySlideBuffer, slideCountFID);
        jfieldID slideWidthFID = env->GetFieldID(slideBufferClass, "slideWidth", "I");
        jint slideWidth = env->GetIntField(mySlideBuffer, slideWidthFID);
        jfieldID pixelsFID = env->GetFieldID(slideBufferClass, "pixels", "[B");
        jbyteArray pixels = (jbyteArray) (env->GetObjectField(mySlideBuffer, pixelsFID));
        if (!authorize()) {
            return -1;
        }
        static BOOL deviceInitOk = FALSE;
        static SRvclDeviceStatus deviceStatus;
        static SRvclFrame frame;
        static OVERLAPPED over;
        static unsigned char* rawBufferPtr;
        if (deviceInitOk == FALSE) {
            int deviceCount = 0;
            BOOL ok = RvclGetDevicesCount(&deviceCount);
            if (ok == FALSE || deviceCount <= 0) {
                return -1;
            }
            ok = RvclSetVideoFormat(0, 1 << vfIndex);
            if (ok == FALSE) {
                return -1;
            }
            deviceStatus.dwSize = sizeof (deviceStatus);
            deviceStatus.dwMask = RVCL_STATUS_RESOLUTION;
            ok = RvclGetDeviceStatus(0, &deviceStatus);
            if (ok == FALSE) {
                return -1;
            }
            frame.dwSize = sizeof (frame);
            frame.dwFlags = RVCL_GRAB_FIELD_ODD | RVCL_GRAB_NOSIGNAL;
            frame.sizeResolution = deviceStatus.sizeMaxResolution;
            frame.dwColorFormat = RVCL_CF_GRAYSCALE;
            frame.dwVideoSource = 0;
            static RawBuffer rawBuffer;
            rawBufferPtr = rawBuffer.update(
                    deviceStatus.sizeMaxResolution.cx * deviceStatus.sizeMaxResolution.cy);
            frame.pBufferPtr = rawBufferPtr;
            frame.dwBufferSize = rawBuffer.getSize();
            over.Internal = over.InternalHigh = over.Offset = over.OffsetHigh = 0;
            over.hEvent = CreateEvent(NULL, true, false, "gfEvent");
            deviceInitOk = TRUE;
        }
        long maxX = deviceStatus.sizeMaxResolution.cx < slideWidth ? deviceStatus.sizeMaxResolution.cx : slideWidth;
        long maxY = deviceStatus.sizeMaxResolution.cy < slideWidth ? deviceStatus.sizeMaxResolution.cy : slideWidth;
        long x0 = (deviceStatus.sizeMaxResolution.cx - maxX) / 2;
        long y0 = (deviceStatus.sizeMaxResolution.cy - maxY) / 2;
        jlong startTime = 0;
        for (long slideIndex = offset;
                count > 0 && slideIndex < slideBufferSlideCount;
                count--, slideIndex++) {
            BOOL ok = RvclGrabFrame(0, &frame, &over);
            if (ok == FALSE) {
                if (GetLastError() != ERROR_IO_PENDING) {
                    return -1;
                }
            }
            DWORD bytesWritten = 0;
            ok = RvclGetGrabFrameResult(0, &over, &bytesWritten, TRUE);
            if (ok == FALSE) {
                if (GetLastError() != ERROR_IO_PENDING) {
                    return -1;
                }
            }
            if (slideIndex == 0) {
                startTime = getStartTime();
            }
            long tmp = slideIndex * slideWidth * slideWidth;
            for (long y = 0; y < maxY; y++) {
                env->SetByteArrayRegion(pixels,
                        tmp + y * slideWidth, maxX,
                        (jbyte*) (rawBufferPtr + (y0 + y) * deviceStatus.sizeMaxResolution.cx + x0));
            }
        }
        return startTime;
    }
}

SRvclInitialize RVCL_INITIALIZE_DATA;

void fillRvclInitializeData();

BOOL authorize() {
    if (authOk == FALSE) {
        fillRvclInitializeData();
        authOk = RvclInitializeEx(&RVCL_INITIALIZE_DATA);
    }
    return authOk;
}

void fillRvclInitializeData() {
    RVCL_INITIALIZE_DATA.dwSize = sizeof (RVCL_INITIALIZE_DATA);
    RVCL_INITIALIZE_DATA.pdwCustomerPassword[0] = 0xC9A10891;
    RVCL_INITIALIZE_DATA.pdwCustomerPassword[1] = 0x60D8D182;
    RVCL_INITIALIZE_DATA.pdwCustomerPassword[2] = 0xE119754C;
    RVCL_INITIALIZE_DATA.pdwCustomerPassword[3] = 0xA211377;
}

jlong getStartTime() {
    SYSTEMTIME startTime;
    GetSystemTime(&startTime);
    return (jlong) (startTime.wSecond * 1000 + startTime.wMilliseconds);
}
