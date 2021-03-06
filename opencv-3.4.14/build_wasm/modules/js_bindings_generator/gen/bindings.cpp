/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

/*M///////////////////////////////////////////////////////////////////////////////////////
// Author: Sajjad Taheri, University of California, Irvine. sajjadt[at]uci[dot]edu
//
//                             LICENSE AGREEMENT
// Copyright (c) 2015 The Regents of the University of California (Regents)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the University nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//M*/

#include <emscripten/bind.h>

#include "opencv2/core.hpp"
#include "opencv2/core/async.hpp"
#include "opencv2/core/base.hpp"
#include "opencv2/core/bindings_utils.hpp"
#include "opencv2/core/check.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/optim.hpp"
#include "opencv2/core/ovx.hpp"
#include "opencv2/core/persistence.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/utils/logger.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/video/tracking.hpp"
#include "opencv2/dnn/dict.hpp"
#include "opencv2/dnn/dnn.hpp"
#include "opencv2/dnn/utils/inference_engine.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/objdetect.hpp"
#include "../../../modules/core/src/parallel_impl.hpp"

#ifdef TEST_WASM_INTRIN
#include "../../../modules/core/include/opencv2/core/hal/intrin.hpp"
#include "../../../modules/core/include/opencv2/core/utils/trace.hpp"
#include "../../../modules/ts/include/opencv2/ts/ts_gtest.h"
namespace cv {
namespace hal {
#include "../../../modules/core/test/test_intrin_utils.hpp"
}
}
#endif

using namespace emscripten;
using namespace cv;
#ifdef HAVE_OPENCV_DNN
using namespace cv::dnn;
#endif

namespace binding_utils
{
    template<typename T>
    emscripten::val matData(const cv::Mat& mat)
    {
        return emscripten::val(emscripten::memory_view<T>((mat.total()*mat.elemSize())/sizeof(T),
                               (T*)mat.data));
    }

    template<typename T>
    emscripten::val matPtr(const cv::Mat& mat, int i)
    {
        return emscripten::val(emscripten::memory_view<T>(mat.step1(0), mat.ptr<T>(i)));
    }

    template<typename T>
    emscripten::val matPtr(const cv::Mat& mat, int i, int j)
    {
        return emscripten::val(emscripten::memory_view<T>(mat.step1(1), mat.ptr<T>(i,j)));
    }

    cv::Mat* createMat(int rows, int cols, int type, intptr_t data, size_t step)
    {
        return new cv::Mat(rows, cols, type, reinterpret_cast<void*>(data), step);
    }

    static emscripten::val getMatSize(const cv::Mat& mat)
    {
        emscripten::val size = emscripten::val::array();
        for (int i = 0; i < mat.dims; i++) {
            size.call<void>("push", mat.size[i]);
        }
        return size;
    }

    static emscripten::val getMatStep(const cv::Mat& mat)
    {
        emscripten::val step = emscripten::val::array();
        for (int i = 0; i < mat.dims; i++) {
            step.call<void>("push", mat.step[i]);
        }
        return step;
    }

    static Mat matEye(int rows, int cols, int type)
    {
        return Mat(cv::Mat::eye(rows, cols, type));
    }

    static Mat matEye(Size size, int type)
    {
        return Mat(cv::Mat::eye(size, type));
    }

    void convertTo(const Mat& obj, Mat& m, int rtype, double alpha, double beta)
    {
        obj.convertTo(m, rtype, alpha, beta);
    }

    void convertTo(const Mat& obj, Mat& m, int rtype)
    {
        obj.convertTo(m, rtype);
    }

    void convertTo(const Mat& obj, Mat& m, int rtype, double alpha)
    {
        obj.convertTo(m, rtype, alpha);
    }

    Size matSize(const cv::Mat& mat)
    {
        return mat.size();
    }

    cv::Mat matZeros(int arg0, int arg1, int arg2)
    {
        return cv::Mat::zeros(arg0, arg1, arg2);
    }

    cv::Mat matZeros(cv::Size arg0, int arg1)
    {
        return cv::Mat::zeros(arg0,arg1);
    }

    cv::Mat matOnes(int arg0, int arg1, int arg2)
    {
        return cv::Mat::ones(arg0, arg1, arg2);
    }

    cv::Mat matOnes(cv::Size arg0, int arg1)
    {
        return cv::Mat::ones(arg0, arg1);
    }

    double matDot(const cv::Mat& obj, const Mat& mat)
    {
        return  obj.dot(mat);
    }

    Mat matMul(const cv::Mat& obj, const Mat& mat, double scale)
    {
        return  Mat(obj.mul(mat, scale));
    }

    Mat matT(const cv::Mat& obj)
    {
        return  Mat(obj.t());
    }

    Mat matInv(const cv::Mat& obj, int type)
    {
        return  Mat(obj.inv(type));
    }

    void matCopyTo(const cv::Mat& obj, cv::Mat& mat)
    {
        return obj.copyTo(mat);
    }

    void matCopyTo(const cv::Mat& obj, cv::Mat& mat, const cv::Mat& mask)
    {
        return obj.copyTo(mat, mask);
    }

    Mat matDiag(const cv::Mat& obj, int d)
    {
        return obj.diag(d);
    }

    Mat matDiag(const cv::Mat& obj)
    {
        return obj.diag();
    }

    void matSetTo(cv::Mat& obj, const cv::Scalar& s)
    {
        obj.setTo(s);
    }

    void matSetTo(cv::Mat& obj, const cv::Scalar& s, const cv::Mat& mask)
    {
        obj.setTo(s, mask);
    }

    emscripten::val rotatedRectPoints(const cv::RotatedRect& obj)
    {
        cv::Point2f points[4];
        obj.points(points);
        emscripten::val pointsArray = emscripten::val::array();
        for (int i = 0; i < 4; i++) {
            pointsArray.call<void>("push", points[i]);
        }
        return pointsArray;
    }

    Rect rotatedRectBoundingRect(const cv::RotatedRect& obj)
    {
        return obj.boundingRect();
    }

    Rect2f rotatedRectBoundingRect2f(const cv::RotatedRect& obj)
    {
        return obj.boundingRect2f();
    }

    int cvMatDepth(int flags)
    {
        return CV_MAT_DEPTH(flags);
    }

    class MinMaxLoc
    {
    public:
        double minVal;
        double maxVal;
        Point minLoc;
        Point maxLoc;
    };

    MinMaxLoc minMaxLoc(const cv::Mat& src, const cv::Mat& mask)
    {
        MinMaxLoc result;
        cv::minMaxLoc(src, &result.minVal, &result.maxVal, &result.minLoc, &result.maxLoc, mask);
        return result;
    }

    MinMaxLoc minMaxLoc_1(const cv::Mat& src)
    {
        MinMaxLoc result;
        cv::minMaxLoc(src, &result.minVal, &result.maxVal, &result.minLoc, &result.maxLoc);
        return result;
    }

    class Circle
    {
    public:
        Point2f center;
        float radius;
    };

#ifdef HAVE_OPENCV_IMGPROC
    Circle minEnclosingCircle(const cv::Mat& points)
    {
        Circle circle;
        cv::minEnclosingCircle(points, circle.center, circle.radius);
        return circle;
    }

    int floodFill_withRect_helper(cv::Mat& arg1, cv::Mat& arg2, Point arg3, Scalar arg4, emscripten::val arg5, Scalar arg6 = Scalar(), Scalar arg7 = Scalar(), int arg8 = 4)
    {
        cv::Rect rect;

        int rc = cv::floodFill(arg1, arg2, arg3, arg4, &rect, arg6, arg7, arg8);

        arg5.set("x", emscripten::val(rect.x));
        arg5.set("y", emscripten::val(rect.y));
        arg5.set("width", emscripten::val(rect.width));
        arg5.set("height", emscripten::val(rect.height));

        return rc;
    }

    int floodFill_wrapper(cv::Mat& arg1, cv::Mat& arg2, Point arg3, Scalar arg4, emscripten::val arg5, Scalar arg6, Scalar arg7, int arg8) {
        return floodFill_withRect_helper(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }

    int floodFill_wrapper_1(cv::Mat& arg1, cv::Mat& arg2, Point arg3, Scalar arg4, emscripten::val arg5, Scalar arg6, Scalar arg7) {
        return floodFill_withRect_helper(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }

    int floodFill_wrapper_2(cv::Mat& arg1, cv::Mat& arg2, Point arg3, Scalar arg4, emscripten::val arg5, Scalar arg6) {
        return floodFill_withRect_helper(arg1, arg2, arg3, arg4, arg5, arg6);
    }

    int floodFill_wrapper_3(cv::Mat& arg1, cv::Mat& arg2, Point arg3, Scalar arg4, emscripten::val arg5) {
        return floodFill_withRect_helper(arg1, arg2, arg3, arg4, arg5);
    }

    int floodFill_wrapper_4(cv::Mat& arg1, cv::Mat& arg2, Point arg3, Scalar arg4) {
        return cv::floodFill(arg1, arg2, arg3, arg4);
    }
#endif

#ifdef HAVE_OPENCV_VIDEO
    emscripten::val CamShiftWrapper(const cv::Mat& arg1, Rect& arg2, TermCriteria arg3)
    {
        RotatedRect rotatedRect = cv::CamShift(arg1, arg2, arg3);
        emscripten::val result = emscripten::val::array();
        result.call<void>("push", rotatedRect);
        result.call<void>("push", arg2);
        return result;
    }

    emscripten::val meanShiftWrapper(const cv::Mat& arg1, Rect& arg2, TermCriteria arg3)
    {
        int n = cv::meanShift(arg1, arg2, arg3);
        emscripten::val result = emscripten::val::array();
        result.call<void>("push", n);
        result.call<void>("push", arg2);
        return result;
    }
#endif  // HAVE_OPENCV_VIDEO

    std::string getExceptionMsg(const cv::Exception& e) {
        return e.msg;
    }

    void setExceptionMsg(cv::Exception& e, std::string msg) {
        e.msg = msg;
        return;
    }

    cv::Exception exceptionFromPtr(intptr_t ptr) {
        return *reinterpret_cast<cv::Exception*>(ptr);
    }

    std::string getBuildInformation() {
        return cv::getBuildInformation();
    }

#ifdef TEST_WASM_INTRIN
    void test_hal_intrin_uint8() {
        cv::hal::test_hal_intrin_uint8();
    }
    void test_hal_intrin_int8() {
        cv::hal::test_hal_intrin_int8();
    }
    void test_hal_intrin_uint16() {
        cv::hal::test_hal_intrin_uint16();
    }
    void test_hal_intrin_int16() {
        cv::hal::test_hal_intrin_int16();
    }
    void test_hal_intrin_uint32() {
        cv::hal::test_hal_intrin_uint32();
    }
    void test_hal_intrin_int32() {
        cv::hal::test_hal_intrin_int32();
    }
    void test_hal_intrin_uint64() {
        cv::hal::test_hal_intrin_uint64();
    }
    void test_hal_intrin_int64() {
        cv::hal::test_hal_intrin_int64();
    }
    void test_hal_intrin_float32() {
        cv::hal::test_hal_intrin_float32();
    }
    void test_hal_intrin_float64() {
        cv::hal::test_hal_intrin_float64();
    }
    void test_hal_intrin_all() {
        cv::hal::test_hal_intrin_uint8();
        cv::hal::test_hal_intrin_int8();
        cv::hal::test_hal_intrin_uint16();
        cv::hal::test_hal_intrin_int16();
        cv::hal::test_hal_intrin_uint32();
        cv::hal::test_hal_intrin_int32();
        cv::hal::test_hal_intrin_uint64();
        cv::hal::test_hal_intrin_int64();
        cv::hal::test_hal_intrin_float32();
        cv::hal::test_hal_intrin_float64();
    }
#endif
}

EMSCRIPTEN_BINDINGS(binding_utils)
{
    register_vector<int>("IntVector");
    register_vector<float>("FloatVector");
    register_vector<double>("DoubleVector");
    register_vector<cv::Point>("PointVector");
    register_vector<cv::Mat>("MatVector");
    register_vector<cv::Rect>("RectVector");
    register_vector<cv::KeyPoint>("KeyPointVector");
    register_vector<cv::DMatch>("DMatchVector");
    register_vector<std::vector<cv::DMatch>>("DMatchVectorVector");


    emscripten::class_<cv::Mat>("Mat")
        .constructor<>()
        .constructor<const Mat&>()
        .constructor<Size, int>()
        .constructor<int, int, int>()
        .constructor<int, int, int, const Scalar&>()
        .constructor(&binding_utils::createMat, allow_raw_pointers())

        .class_function("eye", select_overload<Mat(Size, int)>(&binding_utils::matEye))
        .class_function("eye", select_overload<Mat(int, int, int)>(&binding_utils::matEye))
        .class_function("ones", select_overload<Mat(Size, int)>(&binding_utils::matOnes))
        .class_function("ones", select_overload<Mat(int, int, int)>(&binding_utils::matOnes))
        .class_function("zeros", select_overload<Mat(Size, int)>(&binding_utils::matZeros))
        .class_function("zeros", select_overload<Mat(int, int, int)>(&binding_utils::matZeros))

        .property("rows", &cv::Mat::rows)
        .property("cols", &cv::Mat::cols)
        .property("matSize", &binding_utils::getMatSize)
        .property("step", &binding_utils::getMatStep)
        .property("data", &binding_utils::matData<unsigned char>)
        .property("data8S", &binding_utils::matData<char>)
        .property("data16U", &binding_utils::matData<unsigned short>)
        .property("data16S", &binding_utils::matData<short>)
        .property("data32S", &binding_utils::matData<int>)
        .property("data32F", &binding_utils::matData<float>)
        .property("data64F", &binding_utils::matData<double>)

        .function("elemSize", select_overload<size_t()const>(&cv::Mat::elemSize))
        .function("elemSize1", select_overload<size_t()const>(&cv::Mat::elemSize1))
        .function("channels", select_overload<int()const>(&cv::Mat::channels))
        .function("convertTo", select_overload<void(const Mat&, Mat&, int, double, double)>(&binding_utils::convertTo))
        .function("convertTo", select_overload<void(const Mat&, Mat&, int)>(&binding_utils::convertTo))
        .function("convertTo", select_overload<void(const Mat&, Mat&, int, double)>(&binding_utils::convertTo))
        .function("total", select_overload<size_t()const>(&cv::Mat::total))
        .function("row", select_overload<Mat(int)const>(&cv::Mat::row))
        .function("create", select_overload<void(int, int, int)>(&cv::Mat::create))
        .function("create", select_overload<void(Size, int)>(&cv::Mat::create))
        .function("rowRange", select_overload<Mat(int, int)const>(&cv::Mat::rowRange))
        .function("rowRange", select_overload<Mat(const Range&)const>(&cv::Mat::rowRange))
        .function("copyTo", select_overload<void(const Mat&, Mat&)>(&binding_utils::matCopyTo))
        .function("copyTo", select_overload<void(const Mat&, Mat&, const Mat&)>(&binding_utils::matCopyTo))
        .function("type", select_overload<int()const>(&cv::Mat::type))
        .function("empty", select_overload<bool()const>(&cv::Mat::empty))
        .function("colRange", select_overload<Mat(int, int)const>(&cv::Mat::colRange))
        .function("colRange", select_overload<Mat(const Range&)const>(&cv::Mat::colRange))
        .function("step1", select_overload<size_t(int)const>(&cv::Mat::step1))
        .function("clone", select_overload<Mat()const>(&cv::Mat::clone))
        .function("depth", select_overload<int()const>(&cv::Mat::depth))
        .function("col", select_overload<Mat(int)const>(&cv::Mat::col))
        .function("dot", select_overload<double(const Mat&, const Mat&)>(&binding_utils::matDot))
        .function("mul", select_overload<Mat(const Mat&, const Mat&, double)>(&binding_utils::matMul))
        .function("inv", select_overload<Mat(const Mat&, int)>(&binding_utils::matInv))
        .function("t", select_overload<Mat(const Mat&)>(&binding_utils::matT))
        .function("roi", select_overload<Mat(const Rect&)const>(&cv::Mat::operator()))
        .function("diag", select_overload<Mat(const Mat&, int)>(&binding_utils::matDiag))
        .function("diag", select_overload<Mat(const Mat&)>(&binding_utils::matDiag))
        .function("isContinuous", select_overload<bool()const>(&cv::Mat::isContinuous))
        .function("setTo", select_overload<void(Mat&, const Scalar&)>(&binding_utils::matSetTo))
        .function("setTo", select_overload<void(Mat&, const Scalar&, const Mat&)>(&binding_utils::matSetTo))
        .function("size", select_overload<Size(const Mat&)>(&binding_utils::matSize))

        .function("ptr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<unsigned char>))
        .function("ptr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<unsigned char>))
        .function("ucharPtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<unsigned char>))
        .function("ucharPtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<unsigned char>))
        .function("charPtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<char>))
        .function("charPtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<char>))
        .function("shortPtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<short>))
        .function("shortPtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<short>))
        .function("ushortPtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<unsigned short>))
        .function("ushortPtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<unsigned short>))
        .function("intPtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<int>))
        .function("intPtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<int>))
        .function("floatPtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<float>))
        .function("floatPtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<float>))
        .function("doublePtr", select_overload<val(const Mat&, int)>(&binding_utils::matPtr<double>))
        .function("doublePtr", select_overload<val(const Mat&, int, int)>(&binding_utils::matPtr<double>))

        .function("charAt", select_overload<char&(int)>(&cv::Mat::at<char>))
        .function("charAt", select_overload<char&(int, int)>(&cv::Mat::at<char>))
        .function("charAt", select_overload<char&(int, int, int)>(&cv::Mat::at<char>))
        .function("ucharAt", select_overload<unsigned char&(int)>(&cv::Mat::at<unsigned char>))
        .function("ucharAt", select_overload<unsigned char&(int, int)>(&cv::Mat::at<unsigned char>))
        .function("ucharAt", select_overload<unsigned char&(int, int, int)>(&cv::Mat::at<unsigned char>))
        .function("shortAt", select_overload<short&(int)>(&cv::Mat::at<short>))
        .function("shortAt", select_overload<short&(int, int)>(&cv::Mat::at<short>))
        .function("shortAt", select_overload<short&(int, int, int)>(&cv::Mat::at<short>))
        .function("ushortAt", select_overload<unsigned short&(int)>(&cv::Mat::at<unsigned short>))
        .function("ushortAt", select_overload<unsigned short&(int, int)>(&cv::Mat::at<unsigned short>))
        .function("ushortAt", select_overload<unsigned short&(int, int, int)>(&cv::Mat::at<unsigned short>))
        .function("intAt", select_overload<int&(int)>(&cv::Mat::at<int>) )
        .function("intAt", select_overload<int&(int, int)>(&cv::Mat::at<int>) )
        .function("intAt", select_overload<int&(int, int, int)>(&cv::Mat::at<int>) )
        .function("floatAt", select_overload<float&(int)>(&cv::Mat::at<float>))
        .function("floatAt", select_overload<float&(int, int)>(&cv::Mat::at<float>))
        .function("floatAt", select_overload<float&(int, int, int)>(&cv::Mat::at<float>))
        .function("doubleAt", select_overload<double&(int, int, int)>(&cv::Mat::at<double>))
        .function("doubleAt", select_overload<double&(int)>(&cv::Mat::at<double>))
        .function("doubleAt", select_overload<double&(int, int)>(&cv::Mat::at<double>));

    emscripten::value_object<cv::Range>("Range")
        .field("start", &cv::Range::start)
        .field("end", &cv::Range::end);

    emscripten::value_object<cv::TermCriteria>("TermCriteria")
        .field("type", &cv::TermCriteria::type)
        .field("maxCount", &cv::TermCriteria::maxCount)
        .field("epsilon", &cv::TermCriteria::epsilon);

#define EMSCRIPTEN_CV_SIZE(type) \
    emscripten::value_object<type>("#type") \
        .field("width", &type::width) \
        .field("height", &type::height);

    EMSCRIPTEN_CV_SIZE(Size)
    EMSCRIPTEN_CV_SIZE(Size2f)

#define EMSCRIPTEN_CV_POINT(type) \
    emscripten::value_object<type>("#type") \
        .field("x", &type::x) \
        .field("y", &type::y); \

    EMSCRIPTEN_CV_POINT(Point)
    EMSCRIPTEN_CV_POINT(Point2f)

#define EMSCRIPTEN_CV_RECT(type, name) \
    emscripten::value_object<cv::Rect_<type>> (name) \
        .field("x", &cv::Rect_<type>::x) \
        .field("y", &cv::Rect_<type>::y) \
        .field("width", &cv::Rect_<type>::width) \
        .field("height", &cv::Rect_<type>::height);

    EMSCRIPTEN_CV_RECT(int, "Rect")
    EMSCRIPTEN_CV_RECT(float, "Rect2f")

    emscripten::value_object<cv::RotatedRect>("RotatedRect")
        .field("center", &cv::RotatedRect::center)
        .field("size", &cv::RotatedRect::size)
        .field("angle", &cv::RotatedRect::angle);

    function("rotatedRectPoints", select_overload<emscripten::val(const cv::RotatedRect&)>(&binding_utils::rotatedRectPoints));
    function("rotatedRectBoundingRect", select_overload<Rect(const cv::RotatedRect&)>(&binding_utils::rotatedRectBoundingRect));
    function("rotatedRectBoundingRect2f", select_overload<Rect2f(const cv::RotatedRect&)>(&binding_utils::rotatedRectBoundingRect2f));

    emscripten::value_object<cv::KeyPoint>("KeyPoint")
        .field("angle", &cv::KeyPoint::angle)
        .field("class_id", &cv::KeyPoint::class_id)
        .field("octave", &cv::KeyPoint::octave)
        .field("pt", &cv::KeyPoint::pt)
        .field("response", &cv::KeyPoint::response)
        .field("size", &cv::KeyPoint::size);

    emscripten::value_object<cv::DMatch>("DMatch")
        .field("queryIdx", &cv::DMatch::queryIdx)
        .field("trainIdx", &cv::DMatch::trainIdx)
        .field("imgIdx", &cv::DMatch::imgIdx)
        .field("distance", &cv::DMatch::distance);

    emscripten::value_array<cv::Scalar_<double>> ("Scalar")
        .element(emscripten::index<0>())
        .element(emscripten::index<1>())
        .element(emscripten::index<2>())
        .element(emscripten::index<3>());

    emscripten::value_object<binding_utils::MinMaxLoc>("MinMaxLoc")
        .field("minVal", &binding_utils::MinMaxLoc::minVal)
        .field("maxVal", &binding_utils::MinMaxLoc::maxVal)
        .field("minLoc", &binding_utils::MinMaxLoc::minLoc)
        .field("maxLoc", &binding_utils::MinMaxLoc::maxLoc);

    emscripten::value_object<binding_utils::Circle>("Circle")
        .field("center", &binding_utils::Circle::center)
        .field("radius", &binding_utils::Circle::radius);

    emscripten::value_object<cv::Moments >("Moments")
        .field("m00", &cv::Moments::m00)
        .field("m10", &cv::Moments::m10)
        .field("m01", &cv::Moments::m01)
        .field("m20", &cv::Moments::m20)
        .field("m11", &cv::Moments::m11)
        .field("m02", &cv::Moments::m02)
        .field("m30", &cv::Moments::m30)
        .field("m21", &cv::Moments::m21)
        .field("m12", &cv::Moments::m12)
        .field("m03", &cv::Moments::m03)
        .field("mu20", &cv::Moments::mu20)
        .field("mu11", &cv::Moments::mu11)
        .field("mu02", &cv::Moments::mu02)
        .field("mu30", &cv::Moments::mu30)
        .field("mu21", &cv::Moments::mu21)
        .field("mu12", &cv::Moments::mu12)
        .field("mu03", &cv::Moments::mu03)
        .field("nu20", &cv::Moments::nu20)
        .field("nu11", &cv::Moments::nu11)
        .field("nu02", &cv::Moments::nu02)
        .field("nu30", &cv::Moments::nu30)
        .field("nu21", &cv::Moments::nu21)
        .field("nu12", &cv::Moments::nu12)
        .field("nu03", &cv::Moments::nu03);

    emscripten::value_object<cv::Exception>("Exception")
        .field("code", &cv::Exception::code)
        .field("msg", &binding_utils::getExceptionMsg, &binding_utils::setExceptionMsg);

    function("exceptionFromPtr", &binding_utils::exceptionFromPtr, allow_raw_pointers());

#ifdef HAVE_OPENCV_IMGPROC
    function("minEnclosingCircle", select_overload<binding_utils::Circle(const cv::Mat&)>(&binding_utils::minEnclosingCircle));

    function("floodFill", select_overload<int(cv::Mat&, cv::Mat&, Point, Scalar, emscripten::val, Scalar, Scalar, int)>(&binding_utils::floodFill_wrapper));

    function("floodFill", select_overload<int(cv::Mat&, cv::Mat&, Point, Scalar, emscripten::val, Scalar, Scalar)>(&binding_utils::floodFill_wrapper_1));

    function("floodFill", select_overload<int(cv::Mat&, cv::Mat&, Point, Scalar, emscripten::val, Scalar)>(&binding_utils::floodFill_wrapper_2));

    function("floodFill", select_overload<int(cv::Mat&, cv::Mat&, Point, Scalar, emscripten::val)>(&binding_utils::floodFill_wrapper_3));

    function("floodFill", select_overload<int(cv::Mat&, cv::Mat&, Point, Scalar)>(&binding_utils::floodFill_wrapper_4));
#endif

    function("minMaxLoc", select_overload<binding_utils::MinMaxLoc(const cv::Mat&, const cv::Mat&)>(&binding_utils::minMaxLoc));

    function("minMaxLoc", select_overload<binding_utils::MinMaxLoc(const cv::Mat&)>(&binding_utils::minMaxLoc_1));

#ifdef HAVE_OPENCV_IMGPROC
    function("morphologyDefaultBorderValue", &cv::morphologyDefaultBorderValue);
#endif

    function("CV_MAT_DEPTH", &binding_utils::cvMatDepth);

#ifdef HAVE_OPENCV_VIDEO
    function("CamShift", select_overload<emscripten::val(const cv::Mat&, Rect&, TermCriteria)>(&binding_utils::CamShiftWrapper));

    function("meanShift", select_overload<emscripten::val(const cv::Mat&, Rect&, TermCriteria)>(&binding_utils::meanShiftWrapper));
#endif

    function("getBuildInformation", &binding_utils::getBuildInformation);

#ifdef HAVE_PTHREADS_PF
    function("parallel_pthreads_set_threads_num", &cv::parallel_pthreads_set_threads_num);
    function("parallel_pthreads_get_threads_num", &cv::parallel_pthreads_get_threads_num);
#endif

#ifdef TEST_WASM_INTRIN
    function("test_hal_intrin_uint8", &binding_utils::test_hal_intrin_uint8);
    function("test_hal_intrin_int8", &binding_utils::test_hal_intrin_int8);
    function("test_hal_intrin_uint16", &binding_utils::test_hal_intrin_uint16);
    function("test_hal_intrin_int16", &binding_utils::test_hal_intrin_int16);
    function("test_hal_intrin_uint32", &binding_utils::test_hal_intrin_uint32);
    function("test_hal_intrin_int32", &binding_utils::test_hal_intrin_int32);
    function("test_hal_intrin_uint64", &binding_utils::test_hal_intrin_uint64);
    function("test_hal_intrin_int64", &binding_utils::test_hal_intrin_int64);
    function("test_hal_intrin_float32", &binding_utils::test_hal_intrin_float32);
    function("test_hal_intrin_float64", &binding_utils::test_hal_intrin_float64);
    function("test_hal_intrin_all", &binding_utils::test_hal_intrin_all);
#endif

    constant("CV_8UC1", CV_8UC1);
    constant("CV_8UC2", CV_8UC2);
    constant("CV_8UC3", CV_8UC3);
    constant("CV_8UC4", CV_8UC4);

    constant("CV_8SC1", CV_8SC1);
    constant("CV_8SC2", CV_8SC2);
    constant("CV_8SC3", CV_8SC3);
    constant("CV_8SC4", CV_8SC4);

    constant("CV_16UC1", CV_16UC1);
    constant("CV_16UC2", CV_16UC2);
    constant("CV_16UC3", CV_16UC3);
    constant("CV_16UC4", CV_16UC4);

    constant("CV_16SC1", CV_16SC1);
    constant("CV_16SC2", CV_16SC2);
    constant("CV_16SC3", CV_16SC3);
    constant("CV_16SC4", CV_16SC4);

    constant("CV_32SC1", CV_32SC1);
    constant("CV_32SC2", CV_32SC2);
    constant("CV_32SC3", CV_32SC3);
    constant("CV_32SC4", CV_32SC4);

    constant("CV_32FC1", CV_32FC1);
    constant("CV_32FC2", CV_32FC2);
    constant("CV_32FC3", CV_32FC3);
    constant("CV_32FC4", CV_32FC4);

    constant("CV_64FC1", CV_64FC1);
    constant("CV_64FC2", CV_64FC2);
    constant("CV_64FC3", CV_64FC3);
    constant("CV_64FC4", CV_64FC4);

    constant("CV_8U", CV_8U);
    constant("CV_8S", CV_8S);
    constant("CV_16U", CV_16U);
    constant("CV_16S", CV_16S);
    constant("CV_32S",  CV_32S);
    constant("CV_32F", CV_32F);
    constant("CV_64F", CV_64F);

    constant("INT_MIN", INT_MIN);
    constant("INT_MAX", INT_MAX);
}
namespace Wrappers {
    void Canny_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, bool arg6) {
        return cv::Canny(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void Canny_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5) {
        return cv::Canny(arg1, arg2, arg3, arg4, arg5);
    }
    
    void Canny_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4) {
        return cv::Canny(arg1, arg2, arg3, arg4);
    }
    
    void Canny_wrapper1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4, double arg5, bool arg6) {
        return cv::Canny(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void Canny_wrapper1_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4, double arg5) {
        return cv::Canny(arg1, arg2, arg3, arg4, arg5);
    }
    
    void GaussianBlur_wrapper(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, double arg4, double arg5, int arg6) {
        return cv::GaussianBlur(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void GaussianBlur_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, double arg4, double arg5) {
        return cv::GaussianBlur(arg1, arg2, arg3, arg4, arg5);
    }
    
    void GaussianBlur_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, double arg4) {
        return cv::GaussianBlur(arg1, arg2, arg3, arg4);
    }
    
    void HoughCircles_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, double arg6, double arg7, int arg8, int arg9) {
        return cv::HoughCircles(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void HoughCircles_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, double arg6, double arg7, int arg8) {
        return cv::HoughCircles(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void HoughCircles_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, double arg6, double arg7) {
        return cv::HoughCircles(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void HoughCircles_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, double arg6) {
        return cv::HoughCircles(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void HoughCircles_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5) {
        return cv::HoughCircles(arg1, arg2, arg3, arg4, arg5);
    }
    
    void HoughLines_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, double arg6, double arg7, double arg8, double arg9) {
        return cv::HoughLines(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void HoughLines_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, double arg6, double arg7, double arg8) {
        return cv::HoughLines(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void HoughLines_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, double arg6, double arg7) {
        return cv::HoughLines(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void HoughLines_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, double arg6) {
        return cv::HoughLines(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void HoughLines_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5) {
        return cv::HoughLines(arg1, arg2, arg3, arg4, arg5);
    }
    
    void HoughLinesP_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, double arg6, double arg7) {
        return cv::HoughLinesP(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void HoughLinesP_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, double arg6) {
        return cv::HoughLinesP(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void HoughLinesP_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5) {
        return cv::HoughLinesP(arg1, arg2, arg3, arg4, arg5);
    }
    
    void Laplacian_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, double arg5, double arg6, int arg7) {
        return cv::Laplacian(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void Laplacian_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, double arg5, double arg6) {
        return cv::Laplacian(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void Laplacian_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, double arg5) {
        return cv::Laplacian(arg1, arg2, arg3, arg4, arg5);
    }
    
    void Laplacian_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::Laplacian(arg1, arg2, arg3, arg4);
    }
    
    void Laplacian_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::Laplacian(arg1, arg2, arg3);
    }
    
    void Rodrigues_wrapper(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3) {
        return cv::Rodrigues(arg1, arg2, arg3);
    }
    
    void Rodrigues_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::Rodrigues(arg1, arg2);
    }
    
    void Scharr_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, double arg6, double arg7, int arg8) {
        return cv::Scharr(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void Scharr_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, double arg6, double arg7) {
        return cv::Scharr(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void Scharr_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, double arg6) {
        return cv::Scharr(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void Scharr_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5) {
        return cv::Scharr(arg1, arg2, arg3, arg4, arg5);
    }
    
    void Sobel_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, int arg6, double arg7, double arg8, int arg9) {
        return cv::Sobel(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void Sobel_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, int arg6, double arg7, double arg8) {
        return cv::Sobel(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void Sobel_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, int arg6, double arg7) {
        return cv::Sobel(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void Sobel_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, int arg6) {
        return cv::Sobel(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void Sobel_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5) {
        return cv::Sobel(arg1, arg2, arg3, arg4, arg5);
    }
    
    void absdiff_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::absdiff(arg1, arg2, arg3);
    }
    
    void adaptiveThreshold_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, int arg4, int arg5, int arg6, double arg7) {
        return cv::adaptiveThreshold(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void add_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4, int arg5) {
        return cv::add(arg1, arg2, arg3, arg4, arg5);
    }
    
    void add_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::add(arg1, arg2, arg3, arg4);
    }
    
    void add_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::add(arg1, arg2, arg3);
    }
    
    void addWeighted_wrapper(const cv::Mat& arg1, double arg2, const cv::Mat& arg3, double arg4, double arg5, cv::Mat& arg6, int arg7) {
        return cv::addWeighted(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void addWeighted_wrapper_1(const cv::Mat& arg1, double arg2, const cv::Mat& arg3, double arg4, double arg5, cv::Mat& arg6) {
        return cv::addWeighted(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void approxPolyDP_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, bool arg4) {
        return cv::approxPolyDP(arg1, arg2, arg3, arg4);
    }
    
    double arcLength_wrapper(const cv::Mat& arg1, bool arg2) {
        return cv::arcLength(arg1, arg2);
    }
    
    void bilateralFilter_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, int arg6) {
        return cv::bilateralFilter(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void bilateralFilter_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5) {
        return cv::bilateralFilter(arg1, arg2, arg3, arg4, arg5);
    }
    
    void bitwise_and_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::bitwise_and(arg1, arg2, arg3, arg4);
    }
    
    void bitwise_and_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::bitwise_and(arg1, arg2, arg3);
    }
    
    void bitwise_not_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::bitwise_not(arg1, arg2, arg3);
    }
    
    void bitwise_not_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::bitwise_not(arg1, arg2);
    }
    
    void bitwise_or_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::bitwise_or(arg1, arg2, arg3, arg4);
    }
    
    void bitwise_or_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::bitwise_or(arg1, arg2, arg3);
    }
    
    void bitwise_xor_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::bitwise_xor(arg1, arg2, arg3, arg4);
    }
    
    void bitwise_xor_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::bitwise_xor(arg1, arg2, arg3);
    }
    
    void blur_wrapper(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, Point arg4, int arg5) {
        return cv::blur(arg1, arg2, arg3, arg4, arg5);
    }
    
    void blur_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, Point arg4) {
        return cv::blur(arg1, arg2, arg3, arg4);
    }
    
    void blur_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, Size arg3) {
        return cv::blur(arg1, arg2, arg3);
    }
    
    Rect boundingRect_wrapper(const cv::Mat& arg1) {
        return cv::boundingRect(arg1);
    }
    
    void boxFilter_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, Size arg4, Point arg5, bool arg6, int arg7) {
        return cv::boxFilter(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void boxFilter_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, Size arg4, Point arg5, bool arg6) {
        return cv::boxFilter(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void boxFilter_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, Size arg4, Point arg5) {
        return cv::boxFilter(arg1, arg2, arg3, arg4, arg5);
    }
    
    void boxFilter_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, Size arg4) {
        return cv::boxFilter(arg1, arg2, arg3, arg4);
    }
    
    void calcBackProject_wrapper(const std::vector<cv::Mat>& arg1, const emscripten::val& arg2, const cv::Mat& arg3, cv::Mat& arg4, const emscripten::val& arg5, double arg6) {
        return cv::calcBackProject(arg1, emscripten::vecFromJSArray<int>(arg2), arg3, arg4, emscripten::vecFromJSArray<float>(arg5), arg6);
    }
    
    void calcHist_wrapper(const std::vector<cv::Mat>& arg1, const emscripten::val& arg2, const cv::Mat& arg3, cv::Mat& arg4, const emscripten::val& arg5, const emscripten::val& arg6, bool arg7) {
        return cv::calcHist(arg1, emscripten::vecFromJSArray<int>(arg2), arg3, arg4, emscripten::vecFromJSArray<int>(arg5), emscripten::vecFromJSArray<float>(arg6), arg7);
    }
    
    void calcHist_wrapper_1(const std::vector<cv::Mat>& arg1, const emscripten::val& arg2, const cv::Mat& arg3, cv::Mat& arg4, const emscripten::val& arg5, const emscripten::val& arg6) {
        return cv::calcHist(arg1, emscripten::vecFromJSArray<int>(arg2), arg3, arg4, emscripten::vecFromJSArray<int>(arg5), emscripten::vecFromJSArray<float>(arg6));
    }
    
    void calcOpticalFlowFarneback_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4, int arg5, int arg6, int arg7, int arg8, double arg9, int arg10) {
        return cv::calcOpticalFlowFarneback(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void calcOpticalFlowPyrLK_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, Size arg7, int arg8, TermCriteria arg9, int arg10, double arg11) {
        return cv::calcOpticalFlowPyrLK(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
    }
    
    void calcOpticalFlowPyrLK_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, Size arg7, int arg8, TermCriteria arg9, int arg10) {
        return cv::calcOpticalFlowPyrLK(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void calcOpticalFlowPyrLK_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, Size arg7, int arg8, TermCriteria arg9) {
        return cv::calcOpticalFlowPyrLK(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void calcOpticalFlowPyrLK_wrapper_3(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, Size arg7, int arg8) {
        return cv::calcOpticalFlowPyrLK(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void calcOpticalFlowPyrLK_wrapper_4(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, Size arg7) {
        return cv::calcOpticalFlowPyrLK(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void calcOpticalFlowPyrLK_wrapper_5(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6) {
        return cv::calcOpticalFlowPyrLK(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    double calibrateCameraExtended_wrapper(const std::vector<cv::Mat>& arg1, const std::vector<cv::Mat>& arg2, Size arg3, cv::Mat& arg4, cv::Mat& arg5, std::vector<cv::Mat>& arg6, std::vector<cv::Mat>& arg7, cv::Mat& arg8, cv::Mat& arg9, cv::Mat& arg10, int arg11, TermCriteria arg12) {
        return cv::calibrateCamera(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
    }
    
    double calibrateCameraExtended_wrapper_1(const std::vector<cv::Mat>& arg1, const std::vector<cv::Mat>& arg2, Size arg3, cv::Mat& arg4, cv::Mat& arg5, std::vector<cv::Mat>& arg6, std::vector<cv::Mat>& arg7, cv::Mat& arg8, cv::Mat& arg9, cv::Mat& arg10, int arg11) {
        return cv::calibrateCamera(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
    }
    
    double calibrateCameraExtended_wrapper_2(const std::vector<cv::Mat>& arg1, const std::vector<cv::Mat>& arg2, Size arg3, cv::Mat& arg4, cv::Mat& arg5, std::vector<cv::Mat>& arg6, std::vector<cv::Mat>& arg7, cv::Mat& arg8, cv::Mat& arg9, cv::Mat& arg10) {
        return cv::calibrateCamera(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void cartToPolar_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4, bool arg5) {
        return cv::cartToPolar(arg1, arg2, arg3, arg4, arg5);
    }
    
    void cartToPolar_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4) {
        return cv::cartToPolar(arg1, arg2, arg3, arg4);
    }
    
    void circle_wrapper(cv::Mat& arg1, Point arg2, int arg3, const Scalar& arg4, int arg5, int arg6, int arg7) {
        return cv::circle(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void circle_wrapper_1(cv::Mat& arg1, Point arg2, int arg3, const Scalar& arg4, int arg5, int arg6) {
        return cv::circle(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void circle_wrapper_2(cv::Mat& arg1, Point arg2, int arg3, const Scalar& arg4, int arg5) {
        return cv::circle(arg1, arg2, arg3, arg4, arg5);
    }
    
    void circle_wrapper_3(cv::Mat& arg1, Point arg2, int arg3, const Scalar& arg4) {
        return cv::circle(arg1, arg2, arg3, arg4);
    }
    
    void compare_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4) {
        return cv::compare(arg1, arg2, arg3, arg4);
    }
    
    double compareHist_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, int arg3) {
        return cv::compareHist(arg1, arg2, arg3);
    }
    
    int connectedComponents_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::connectedComponents(arg1, arg2, arg3, arg4);
    }
    
    int connectedComponents_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::connectedComponents(arg1, arg2, arg3);
    }
    
    int connectedComponents_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::connectedComponents(arg1, arg2);
    }
    
    int connectedComponentsWithStats_wrapper(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4, int arg5, int arg6) {
        return cv::connectedComponentsWithStats(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    int connectedComponentsWithStats_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4, int arg5) {
        return cv::connectedComponentsWithStats(arg1, arg2, arg3, arg4, arg5);
    }
    
    int connectedComponentsWithStats_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4) {
        return cv::connectedComponentsWithStats(arg1, arg2, arg3, arg4);
    }
    
    double contourArea_wrapper(const cv::Mat& arg1, bool arg2) {
        return cv::contourArea(arg1, arg2);
    }
    
    double contourArea_wrapper_1(const cv::Mat& arg1) {
        return cv::contourArea(arg1);
    }
    
    void convertScaleAbs_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4) {
        return cv::convertScaleAbs(arg1, arg2, arg3, arg4);
    }
    
    void convertScaleAbs_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, double arg3) {
        return cv::convertScaleAbs(arg1, arg2, arg3);
    }
    
    void convertScaleAbs_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::convertScaleAbs(arg1, arg2);
    }
    
    void convexHull_wrapper(const cv::Mat& arg1, cv::Mat& arg2, bool arg3, bool arg4) {
        return cv::convexHull(arg1, arg2, arg3, arg4);
    }
    
    void convexHull_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, bool arg3) {
        return cv::convexHull(arg1, arg2, arg3);
    }
    
    void convexHull_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::convexHull(arg1, arg2);
    }
    
    void convexityDefects_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::convexityDefects(arg1, arg2, arg3);
    }
    
    void copyMakeBorder_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, int arg6, int arg7, const Scalar& arg8) {
        return cv::copyMakeBorder(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void copyMakeBorder_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5, int arg6, int arg7) {
        return cv::copyMakeBorder(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void cornerHarris_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, double arg5, int arg6) {
        return cv::cornerHarris(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void cornerHarris_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, double arg5) {
        return cv::cornerHarris(arg1, arg2, arg3, arg4, arg5);
    }
    
    void cornerMinEigenVal_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5) {
        return cv::cornerMinEigenVal(arg1, arg2, arg3, arg4, arg5);
    }
    
    void cornerMinEigenVal_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::cornerMinEigenVal(arg1, arg2, arg3, arg4);
    }
    
    void cornerMinEigenVal_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::cornerMinEigenVal(arg1, arg2, arg3);
    }
    
    int countNonZero_wrapper(const cv::Mat& arg1) {
        return cv::countNonZero(arg1);
    }
    
    void cvtColor_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::cvtColor(arg1, arg2, arg3, arg4);
    }
    
    void cvtColor_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::cvtColor(arg1, arg2, arg3);
    }
    
    void demosaicing_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::demosaicing(arg1, arg2, arg3, arg4);
    }
    
    void demosaicing_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::demosaicing(arg1, arg2, arg3);
    }
    
    double determinant_wrapper(const cv::Mat& arg1) {
        return cv::determinant(arg1);
    }
    
    void dft_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::dft(arg1, arg2, arg3, arg4);
    }
    
    void dft_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::dft(arg1, arg2, arg3);
    }
    
    void dft_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::dft(arg1, arg2);
    }
    
    void dilate_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4, int arg5, int arg6, const Scalar& arg7) {
        return cv::dilate(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void dilate_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4, int arg5, int arg6) {
        return cv::dilate(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void dilate_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4, int arg5) {
        return cv::dilate(arg1, arg2, arg3, arg4, arg5);
    }
    
    void dilate_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4) {
        return cv::dilate(arg1, arg2, arg3, arg4);
    }
    
    void dilate_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::dilate(arg1, arg2, arg3);
    }
    
    void distanceTransform_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5) {
        return cv::distanceTransform(arg1, arg2, arg3, arg4, arg5);
    }
    
    void distanceTransform_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::distanceTransform(arg1, arg2, arg3, arg4);
    }
    
    void distanceTransformWithLabels_wrapper(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, int arg4, int arg5, int arg6) {
        return cv::distanceTransform(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void distanceTransformWithLabels_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, int arg4, int arg5) {
        return cv::distanceTransform(arg1, arg2, arg3, arg4, arg5);
    }
    
    void divide_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4, int arg5) {
        return cv::divide(arg1, arg2, arg3, arg4, arg5);
    }
    
    void divide_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4) {
        return cv::divide(arg1, arg2, arg3, arg4);
    }
    
    void divide_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::divide(arg1, arg2, arg3);
    }
    
    void divide_wrapper1(double arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4) {
        return cv::divide(arg1, arg2, arg3, arg4);
    }
    
    void divide_wrapper1_1(double arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::divide(arg1, arg2, arg3);
    }
    
    void drawContours_wrapper(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, int arg3, const Scalar& arg4, int arg5, int arg6, const cv::Mat& arg7, int arg8, Point arg9) {
        return cv::drawContours(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void drawContours_wrapper_1(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, int arg3, const Scalar& arg4, int arg5, int arg6, const cv::Mat& arg7, int arg8) {
        return cv::drawContours(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void drawContours_wrapper_2(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, int arg3, const Scalar& arg4, int arg5, int arg6, const cv::Mat& arg7) {
        return cv::drawContours(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void drawContours_wrapper_3(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, int arg3, const Scalar& arg4, int arg5, int arg6) {
        return cv::drawContours(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void drawContours_wrapper_4(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, int arg3, const Scalar& arg4, int arg5) {
        return cv::drawContours(arg1, arg2, arg3, arg4, arg5);
    }
    
    void drawContours_wrapper_5(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, int arg3, const Scalar& arg4) {
        return cv::drawContours(arg1, arg2, arg3, arg4);
    }
    
    void drawFrameAxes_wrapper(cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, const cv::Mat& arg5, float arg6, int arg7) {
        return cv::drawFrameAxes(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void drawFrameAxes_wrapper_1(cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, const cv::Mat& arg5, float arg6) {
        return cv::drawFrameAxes(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void drawKeypoints_wrapper(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, cv::Mat& arg3, const Scalar& arg4, int arg5) {
        return cv::drawKeypoints(arg1, arg2, arg3, arg4, arg5);
    }
    
    void drawKeypoints_wrapper_1(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, cv::Mat& arg3, const Scalar& arg4) {
        return cv::drawKeypoints(arg1, arg2, arg3, arg4);
    }
    
    void drawKeypoints_wrapper_2(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, cv::Mat& arg3) {
        return cv::drawKeypoints(arg1, arg2, arg3);
    }
    
    void drawMatches_wrapper(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<DMatch>& arg5, cv::Mat& arg6, const Scalar& arg7, const Scalar& arg8, const emscripten::val& arg9, int arg10) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, emscripten::vecFromJSArray<char>(arg9), arg10);
    }
    
    void drawMatches_wrapper_1(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<DMatch>& arg5, cv::Mat& arg6, const Scalar& arg7, const Scalar& arg8, const emscripten::val& arg9) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, emscripten::vecFromJSArray<char>(arg9));
    }
    
    void drawMatches_wrapper_2(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<DMatch>& arg5, cv::Mat& arg6, const Scalar& arg7, const Scalar& arg8) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void drawMatches_wrapper_3(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<DMatch>& arg5, cv::Mat& arg6, const Scalar& arg7) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void drawMatches_wrapper_4(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<DMatch>& arg5, cv::Mat& arg6) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void drawMatchesKnn_wrapper(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<std::vector<DMatch>>& arg5, cv::Mat& arg6, const Scalar& arg7, const Scalar& arg8, const std::vector<std::vector<char>>& arg9, int arg10) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void drawMatchesKnn_wrapper_1(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<std::vector<DMatch>>& arg5, cv::Mat& arg6, const Scalar& arg7, const Scalar& arg8, const std::vector<std::vector<char>>& arg9) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void drawMatchesKnn_wrapper_2(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<std::vector<DMatch>>& arg5, cv::Mat& arg6, const Scalar& arg7, const Scalar& arg8) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void drawMatchesKnn_wrapper_3(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<std::vector<DMatch>>& arg5, cv::Mat& arg6, const Scalar& arg7) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void drawMatchesKnn_wrapper_4(const cv::Mat& arg1, const std::vector<KeyPoint>& arg2, const cv::Mat& arg3, const std::vector<KeyPoint>& arg4, const std::vector<std::vector<DMatch>>& arg5, cv::Mat& arg6) {
        return cv::drawMatches(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    bool eigen_wrapper(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3) {
        return cv::eigen(arg1, arg2, arg3);
    }
    
    bool eigen_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::eigen(arg1, arg2);
    }
    
    void ellipse_wrapper(cv::Mat& arg1, Point arg2, Size arg3, double arg4, double arg5, double arg6, const Scalar& arg7, int arg8, int arg9, int arg10) {
        return cv::ellipse(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void ellipse_wrapper_1(cv::Mat& arg1, Point arg2, Size arg3, double arg4, double arg5, double arg6, const Scalar& arg7, int arg8, int arg9) {
        return cv::ellipse(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void ellipse_wrapper_2(cv::Mat& arg1, Point arg2, Size arg3, double arg4, double arg5, double arg6, const Scalar& arg7, int arg8) {
        return cv::ellipse(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void ellipse_wrapper_3(cv::Mat& arg1, Point arg2, Size arg3, double arg4, double arg5, double arg6, const Scalar& arg7) {
        return cv::ellipse(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void ellipse_wrapper1(cv::Mat& arg1, const RotatedRect& arg2, const Scalar& arg3, int arg4, int arg5) {
        return cv::ellipse(arg1, arg2, arg3, arg4, arg5);
    }
    
    void ellipse_wrapper1_1(cv::Mat& arg1, const RotatedRect& arg2, const Scalar& arg3, int arg4) {
        return cv::ellipse(arg1, arg2, arg3, arg4);
    }
    
    void ellipse_wrapper1_2(cv::Mat& arg1, const RotatedRect& arg2, const Scalar& arg3) {
        return cv::ellipse(arg1, arg2, arg3);
    }
    
    void ellipse2Poly_wrapper(Point arg1, Size arg2, int arg3, int arg4, int arg5, int arg6, std::vector<Point>& arg7) {
        return cv::ellipse2Poly(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void equalizeHist_wrapper(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::equalizeHist(arg1, arg2);
    }
    
    void erode_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4, int arg5, int arg6, const Scalar& arg7) {
        return cv::erode(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void erode_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4, int arg5, int arg6) {
        return cv::erode(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void erode_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4, int arg5) {
        return cv::erode(arg1, arg2, arg3, arg4, arg5);
    }
    
    void erode_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Point arg4) {
        return cv::erode(arg1, arg2, arg3, arg4);
    }
    
    void erode_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::erode(arg1, arg2, arg3);
    }
    
    Mat estimateAffine2D_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4, double arg5, size_t arg6, double arg7, size_t arg8) {
        return cv::estimateAffine2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    Mat estimateAffine2D_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4, double arg5, size_t arg6, double arg7) {
        return cv::estimateAffine2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    Mat estimateAffine2D_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4, double arg5, size_t arg6) {
        return cv::estimateAffine2D(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Mat estimateAffine2D_wrapper_3(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4, double arg5) {
        return cv::estimateAffine2D(arg1, arg2, arg3, arg4, arg5);
    }
    
    Mat estimateAffine2D_wrapper_4(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4) {
        return cv::estimateAffine2D(arg1, arg2, arg3, arg4);
    }
    
    Mat estimateAffine2D_wrapper_5(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::estimateAffine2D(arg1, arg2, arg3);
    }
    
    Mat estimateAffine2D_wrapper_6(const cv::Mat& arg1, const cv::Mat& arg2) {
        return cv::estimateAffine2D(arg1, arg2);
    }
    
    void exp_wrapper(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::exp(arg1, arg2);
    }
    
    void fillConvexPoly_wrapper(cv::Mat& arg1, const cv::Mat& arg2, const Scalar& arg3, int arg4, int arg5) {
        return cv::fillConvexPoly(arg1, arg2, arg3, arg4, arg5);
    }
    
    void fillConvexPoly_wrapper_1(cv::Mat& arg1, const cv::Mat& arg2, const Scalar& arg3, int arg4) {
        return cv::fillConvexPoly(arg1, arg2, arg3, arg4);
    }
    
    void fillConvexPoly_wrapper_2(cv::Mat& arg1, const cv::Mat& arg2, const Scalar& arg3) {
        return cv::fillConvexPoly(arg1, arg2, arg3);
    }
    
    void fillPoly_wrapper(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, const Scalar& arg3, int arg4, int arg5, Point arg6) {
        return cv::fillPoly(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void fillPoly_wrapper_1(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, const Scalar& arg3, int arg4, int arg5) {
        return cv::fillPoly(arg1, arg2, arg3, arg4, arg5);
    }
    
    void fillPoly_wrapper_2(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, const Scalar& arg3, int arg4) {
        return cv::fillPoly(arg1, arg2, arg3, arg4);
    }
    
    void fillPoly_wrapper_3(cv::Mat& arg1, const std::vector<cv::Mat>& arg2, const Scalar& arg3) {
        return cv::fillPoly(arg1, arg2, arg3);
    }
    
    void filter2D_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5, double arg6, int arg7) {
        return cv::filter2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void filter2D_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5, double arg6) {
        return cv::filter2D(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void filter2D_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5) {
        return cv::filter2D(arg1, arg2, arg3, arg4, arg5);
    }
    
    void filter2D_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4) {
        return cv::filter2D(arg1, arg2, arg3, arg4);
    }
    
    void findContours_wrapper(cv::Mat& arg1, std::vector<cv::Mat>& arg2, cv::Mat& arg3, int arg4, int arg5, Point arg6) {
        return cv::findContours(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void findContours_wrapper_1(cv::Mat& arg1, std::vector<cv::Mat>& arg2, cv::Mat& arg3, int arg4, int arg5) {
        return cv::findContours(arg1, arg2, arg3, arg4, arg5);
    }
    
    Mat findHomography_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, int arg3, double arg4, cv::Mat& arg5, const int arg6, const double arg7) {
        return cv::findHomography(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    Mat findHomography_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, int arg3, double arg4, cv::Mat& arg5, const int arg6) {
        return cv::findHomography(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Mat findHomography_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, int arg3, double arg4, cv::Mat& arg5) {
        return cv::findHomography(arg1, arg2, arg3, arg4, arg5);
    }
    
    Mat findHomography_wrapper_3(const cv::Mat& arg1, const cv::Mat& arg2, int arg3, double arg4) {
        return cv::findHomography(arg1, arg2, arg3, arg4);
    }
    
    Mat findHomography_wrapper_4(const cv::Mat& arg1, const cv::Mat& arg2, int arg3) {
        return cv::findHomography(arg1, arg2, arg3);
    }
    
    Mat findHomography_wrapper_5(const cv::Mat& arg1, const cv::Mat& arg2) {
        return cv::findHomography(arg1, arg2);
    }
    
    double findTransformECC_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4, TermCriteria arg5, const cv::Mat& arg6, int arg7) {
        return cv::findTransformECC(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    RotatedRect fitEllipse_wrapper(const cv::Mat& arg1) {
        return cv::fitEllipse(arg1);
    }
    
    void fitLine_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, double arg6) {
        return cv::fitLine(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void flip_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::flip(arg1, arg2, arg3);
    }
    
    void gemm_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, double arg3, const cv::Mat& arg4, double arg5, cv::Mat& arg6, int arg7) {
        return cv::gemm(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void gemm_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, double arg3, const cv::Mat& arg4, double arg5, cv::Mat& arg6) {
        return cv::gemm(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Mat getAffineTransform_wrapper(const cv::Mat& arg1, const cv::Mat& arg2) {
        return cv::getAffineTransform(arg1, arg2);
    }
    
    Mat getDefaultNewCameraMatrix_wrapper(const cv::Mat& arg1, Size arg2, bool arg3) {
        return cv::getDefaultNewCameraMatrix(arg1, arg2, arg3);
    }
    
    Mat getDefaultNewCameraMatrix_wrapper_1(const cv::Mat& arg1, Size arg2) {
        return cv::getDefaultNewCameraMatrix(arg1, arg2);
    }
    
    Mat getDefaultNewCameraMatrix_wrapper_2(const cv::Mat& arg1) {
        return cv::getDefaultNewCameraMatrix(arg1);
    }
    
    int getLogLevel_wrapper() {
        return cv::getLogLevel();
    }
    
    int getOptimalDFTSize_wrapper(int arg1) {
        return cv::getOptimalDFTSize(arg1);
    }
    
    Mat getPerspectiveTransform_wrapper(const cv::Mat& arg1, const cv::Mat& arg2) {
        return cv::getPerspectiveTransform(arg1, arg2);
    }
    
    Mat getRotationMatrix2D_wrapper(Point2f arg1, double arg2, double arg3) {
        return cv::getRotationMatrix2D(arg1, arg2, arg3);
    }
    
    Mat getStructuringElement_wrapper(int arg1, Size arg2, Point arg3) {
        return cv::getStructuringElement(arg1, arg2, arg3);
    }
    
    Mat getStructuringElement_wrapper_1(int arg1, Size arg2) {
        return cv::getStructuringElement(arg1, arg2);
    }
    
    void goodFeaturesToTrack_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6, int arg7, bool arg8, double arg9) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void goodFeaturesToTrack_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6, int arg7, bool arg8) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void goodFeaturesToTrack_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6, int arg7) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void goodFeaturesToTrack_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void goodFeaturesToTrack_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5);
    }
    
    void goodFeaturesToTrack_wrapper1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6, int arg7, int arg8, bool arg9, double arg10) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void goodFeaturesToTrack_wrapper1_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6, int arg7, int arg8, bool arg9) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void goodFeaturesToTrack_wrapper1_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, double arg4, double arg5, const cv::Mat& arg6, int arg7, int arg8) {
        return cv::goodFeaturesToTrack(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void grabCut_wrapper(const cv::Mat& arg1, cv::Mat& arg2, Rect arg3, cv::Mat& arg4, cv::Mat& arg5, int arg6, int arg7) {
        return cv::grabCut(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void grabCut_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, Rect arg3, cv::Mat& arg4, cv::Mat& arg5, int arg6) {
        return cv::grabCut(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void groupRectangles_wrapper(std::vector<Rect>& arg1, std::vector<int>& arg2, int arg3, double arg4) {
        return cv::groupRectangles(arg1, arg2, arg3, arg4);
    }
    
    void groupRectangles_wrapper_1(std::vector<Rect>& arg1, std::vector<int>& arg2, int arg3) {
        return cv::groupRectangles(arg1, arg2, arg3);
    }
    
    void hconcat_wrapper(const std::vector<cv::Mat>& arg1, cv::Mat& arg2) {
        return cv::hconcat(arg1, arg2);
    }
    
    void inRange_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, cv::Mat& arg4) {
        return cv::inRange(arg1, arg2, arg3, arg4);
    }
    
    void initUndistortRectifyMap_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, Size arg5, int arg6, cv::Mat& arg7, cv::Mat& arg8) {
        return cv::initUndistortRectifyMap(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void integral_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::integral(arg1, arg2, arg3);
    }
    
    void integral_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::integral(arg1, arg2);
    }
    
    void integral2_wrapper(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, int arg4, int arg5) {
        return cv::integral(arg1, arg2, arg3, arg4, arg5);
    }
    
    void integral2_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, int arg4) {
        return cv::integral(arg1, arg2, arg3, arg4);
    }
    
    void integral2_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3) {
        return cv::integral(arg1, arg2, arg3);
    }
    
    double invert_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::invert(arg1, arg2, arg3);
    }
    
    double invert_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::invert(arg1, arg2);
    }
    
    bool isContourConvex_wrapper(const cv::Mat& arg1) {
        return cv::isContourConvex(arg1);
    }
    
    double kmeans_wrapper(const cv::Mat& arg1, int arg2, cv::Mat& arg3, TermCriteria arg4, int arg5, int arg6, cv::Mat& arg7) {
        return cv::kmeans(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    double kmeans_wrapper_1(const cv::Mat& arg1, int arg2, cv::Mat& arg3, TermCriteria arg4, int arg5, int arg6) {
        return cv::kmeans(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void line_wrapper(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4, int arg5, int arg6, int arg7) {
        return cv::line(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void line_wrapper_1(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4, int arg5, int arg6) {
        return cv::line(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void line_wrapper_2(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4, int arg5) {
        return cv::line(arg1, arg2, arg3, arg4, arg5);
    }
    
    void line_wrapper_3(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4) {
        return cv::line(arg1, arg2, arg3, arg4);
    }
    
    void log_wrapper(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::log(arg1, arg2);
    }
    
    void magnitude_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::magnitude(arg1, arg2, arg3);
    }
    
    double matchShapes_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, int arg3, double arg4) {
        return cv::matchShapes(arg1, arg2, arg3, arg4);
    }
    
    void matchTemplate_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4, const cv::Mat& arg5) {
        return cv::matchTemplate(arg1, arg2, arg3, arg4, arg5);
    }
    
    void matchTemplate_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4) {
        return cv::matchTemplate(arg1, arg2, arg3, arg4);
    }
    
    void max_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::max(arg1, arg2, arg3);
    }
    
    Scalar mean_wrapper(const cv::Mat& arg1, const cv::Mat& arg2) {
        return cv::mean(arg1, arg2);
    }
    
    Scalar mean_wrapper_1(const cv::Mat& arg1) {
        return cv::mean(arg1);
    }
    
    void meanStdDev_wrapper(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::meanStdDev(arg1, arg2, arg3, arg4);
    }
    
    void meanStdDev_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3) {
        return cv::meanStdDev(arg1, arg2, arg3);
    }
    
    void medianBlur_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::medianBlur(arg1, arg2, arg3);
    }
    
    void merge_wrapper(const std::vector<cv::Mat>& arg1, cv::Mat& arg2) {
        return cv::merge(arg1, arg2);
    }
    
    void min_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::min(arg1, arg2, arg3);
    }
    
    RotatedRect minAreaRect_wrapper(const cv::Mat& arg1) {
        return cv::minAreaRect(arg1);
    }
    
    void mixChannels_wrapper(const std::vector<cv::Mat>& arg1, std::vector<cv::Mat>& arg2, const emscripten::val& arg3) {
        return cv::mixChannels(arg1, arg2, emscripten::vecFromJSArray<int>(arg3));
    }
    
    Moments moments_wrapper(const cv::Mat& arg1, bool arg2) {
        return cv::moments(arg1, arg2);
    }
    
    Moments moments_wrapper_1(const cv::Mat& arg1) {
        return cv::moments(arg1);
    }
    
    void morphologyEx_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5, int arg6, int arg7, const Scalar& arg8) {
        return cv::morphologyEx(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void morphologyEx_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5, int arg6, int arg7) {
        return cv::morphologyEx(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void morphologyEx_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5, int arg6) {
        return cv::morphologyEx(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void morphologyEx_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, Point arg5) {
        return cv::morphologyEx(arg1, arg2, arg3, arg4, arg5);
    }
    
    void morphologyEx_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4) {
        return cv::morphologyEx(arg1, arg2, arg3, arg4);
    }
    
    void multiply_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4, int arg5) {
        return cv::multiply(arg1, arg2, arg3, arg4, arg5);
    }
    
    void multiply_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, double arg4) {
        return cv::multiply(arg1, arg2, arg3, arg4);
    }
    
    void multiply_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::multiply(arg1, arg2, arg3);
    }
    
    double norm_wrapper(const cv::Mat& arg1, int arg2, const cv::Mat& arg3) {
        return cv::norm(arg1, arg2, arg3);
    }
    
    double norm_wrapper_1(const cv::Mat& arg1, int arg2) {
        return cv::norm(arg1, arg2);
    }
    
    double norm_wrapper_2(const cv::Mat& arg1) {
        return cv::norm(arg1);
    }
    
    double norm_wrapper1(const cv::Mat& arg1, const cv::Mat& arg2, int arg3, const cv::Mat& arg4) {
        return cv::norm(arg1, arg2, arg3, arg4);
    }
    
    double norm_wrapper1_1(const cv::Mat& arg1, const cv::Mat& arg2, int arg3) {
        return cv::norm(arg1, arg2, arg3);
    }
    
    double norm_wrapper1_2(const cv::Mat& arg1, const cv::Mat& arg2) {
        return cv::norm(arg1, arg2);
    }
    
    void normalize_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, int arg6, const cv::Mat& arg7) {
        return cv::normalize(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void normalize_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5, int arg6) {
        return cv::normalize(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void normalize_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5) {
        return cv::normalize(arg1, arg2, arg3, arg4, arg5);
    }
    
    void normalize_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4) {
        return cv::normalize(arg1, arg2, arg3, arg4);
    }
    
    void normalize_wrapper_4(const cv::Mat& arg1, cv::Mat& arg2, double arg3) {
        return cv::normalize(arg1, arg2, arg3);
    }
    
    void normalize_wrapper_5(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::normalize(arg1, arg2);
    }
    
    void perspectiveTransform_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::perspectiveTransform(arg1, arg2, arg3);
    }
    
    double pointPolygonTest_wrapper(const cv::Mat& arg1, Point2f arg2, bool arg3) {
        return cv::pointPolygonTest(arg1, arg2, arg3);
    }
    
    void polarToCart_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4, bool arg5) {
        return cv::polarToCart(arg1, arg2, arg3, arg4, arg5);
    }
    
    void polarToCart_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, cv::Mat& arg4) {
        return cv::polarToCart(arg1, arg2, arg3, arg4);
    }
    
    void pow_wrapper(const cv::Mat& arg1, double arg2, cv::Mat& arg3) {
        return cv::pow(arg1, arg2, arg3);
    }
    
    void putText_wrapper(cv::Mat& arg1, const std::string& arg2, Point arg3, int arg4, double arg5, Scalar arg6, int arg7, int arg8, bool arg9) {
        return cv::putText(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void putText_wrapper_1(cv::Mat& arg1, const std::string& arg2, Point arg3, int arg4, double arg5, Scalar arg6, int arg7, int arg8) {
        return cv::putText(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void putText_wrapper_2(cv::Mat& arg1, const std::string& arg2, Point arg3, int arg4, double arg5, Scalar arg6, int arg7) {
        return cv::putText(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void putText_wrapper_3(cv::Mat& arg1, const std::string& arg2, Point arg3, int arg4, double arg5, Scalar arg6) {
        return cv::putText(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void pyrDown_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const Size& arg3, int arg4) {
        return cv::pyrDown(arg1, arg2, arg3, arg4);
    }
    
    void pyrDown_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const Size& arg3) {
        return cv::pyrDown(arg1, arg2, arg3);
    }
    
    void pyrDown_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::pyrDown(arg1, arg2);
    }
    
    void pyrUp_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const Size& arg3, int arg4) {
        return cv::pyrUp(arg1, arg2, arg3, arg4);
    }
    
    void pyrUp_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const Size& arg3) {
        return cv::pyrUp(arg1, arg2, arg3);
    }
    
    void pyrUp_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::pyrUp(arg1, arg2);
    }
    
    void randn_wrapper(cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::randn(arg1, arg2, arg3);
    }
    
    void randu_wrapper(cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::randu(arg1, arg2, arg3);
    }
    
    void rectangle_wrapper(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4, int arg5, int arg6, int arg7) {
        return cv::rectangle(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void rectangle_wrapper_1(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4, int arg5, int arg6) {
        return cv::rectangle(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void rectangle_wrapper_2(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4, int arg5) {
        return cv::rectangle(arg1, arg2, arg3, arg4, arg5);
    }
    
    void rectangle_wrapper_3(cv::Mat& arg1, Point arg2, Point arg3, const Scalar& arg4) {
        return cv::rectangle(arg1, arg2, arg3, arg4);
    }
    
    void reduce_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4, int arg5) {
        return cv::reduce(arg1, arg2, arg3, arg4, arg5);
    }
    
    void reduce_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, int arg4) {
        return cv::reduce(arg1, arg2, arg3, arg4);
    }
    
    void remap_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, int arg5, int arg6, const Scalar& arg7) {
        return cv::remap(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void remap_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, int arg5, int arg6) {
        return cv::remap(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void remap_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, int arg5) {
        return cv::remap(arg1, arg2, arg3, arg4, arg5);
    }
    
    void repeat_wrapper(const cv::Mat& arg1, int arg2, int arg3, cv::Mat& arg4) {
        return cv::repeat(arg1, arg2, arg3, arg4);
    }
    
    void resize_wrapper(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, double arg4, double arg5, int arg6) {
        return cv::resize(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void resize_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, double arg4, double arg5) {
        return cv::resize(arg1, arg2, arg3, arg4, arg5);
    }
    
    void resize_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, double arg4) {
        return cv::resize(arg1, arg2, arg3, arg4);
    }
    
    void resize_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, Size arg3) {
        return cv::resize(arg1, arg2, arg3);
    }
    
    void rotate_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::rotate(arg1, arg2, arg3);
    }
    
    void sepFilter2D_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, const cv::Mat& arg5, Point arg6, double arg7, int arg8) {
        return cv::sepFilter2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void sepFilter2D_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, const cv::Mat& arg5, Point arg6, double arg7) {
        return cv::sepFilter2D(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void sepFilter2D_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, const cv::Mat& arg5, Point arg6) {
        return cv::sepFilter2D(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void sepFilter2D_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, int arg3, const cv::Mat& arg4, const cv::Mat& arg5) {
        return cv::sepFilter2D(arg1, arg2, arg3, arg4, arg5);
    }
    
    void setIdentity_wrapper(cv::Mat& arg1, const Scalar& arg2) {
        return cv::setIdentity(arg1, arg2);
    }
    
    void setIdentity_wrapper_1(cv::Mat& arg1) {
        return cv::setIdentity(arg1);
    }
    
    int setLogLevel_wrapper(int arg1) {
        return cv::setLogLevel(arg1);
    }
    
    void setRNGSeed_wrapper(int arg1) {
        return cv::setRNGSeed(arg1);
    }
    
    bool solve_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, int arg4) {
        return cv::solve(arg1, arg2, arg3, arg4);
    }
    
    bool solve_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::solve(arg1, arg2, arg3);
    }
    
    bool solvePnP_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7, int arg8) {
        return cv::solvePnP(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    bool solvePnP_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7) {
        return cv::solvePnP(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    bool solvePnP_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6) {
        return cv::solvePnP(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    bool solvePnPRansac_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7, int arg8, float arg9, double arg10, cv::Mat& arg11, int arg12) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12);
    }
    
    bool solvePnPRansac_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7, int arg8, float arg9, double arg10, cv::Mat& arg11) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11);
    }
    
    bool solvePnPRansac_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7, int arg8, float arg9, double arg10) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    bool solvePnPRansac_wrapper_3(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7, int arg8, float arg9) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    bool solvePnPRansac_wrapper_4(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7, int arg8) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    bool solvePnPRansac_wrapper_5(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, bool arg7) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    bool solvePnPRansac_wrapper_6(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6) {
        return cv::solvePnPRansac(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void solvePnPRefineLM_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6, TermCriteria arg7) {
        return cv::solvePnPRefineLM(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void solvePnPRefineLM_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, cv::Mat& arg5, cv::Mat& arg6) {
        return cv::solvePnPRefineLM(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    double solvePoly_wrapper(const cv::Mat& arg1, cv::Mat& arg2, int arg3) {
        return cv::solvePoly(arg1, arg2, arg3);
    }
    
    double solvePoly_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::solvePoly(arg1, arg2);
    }
    
    void split_wrapper(const cv::Mat& arg1, std::vector<cv::Mat>& arg2) {
        return cv::split(arg1, arg2);
    }
    
    void sqrt_wrapper(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::sqrt(arg1, arg2);
    }
    
    void subtract_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4, int arg5) {
        return cv::subtract(arg1, arg2, arg3, arg4, arg5);
    }
    
    void subtract_wrapper_1(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::subtract(arg1, arg2, arg3, arg4);
    }
    
    void subtract_wrapper_2(const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return cv::subtract(arg1, arg2, arg3);
    }
    
    double threshold_wrapper(const cv::Mat& arg1, cv::Mat& arg2, double arg3, double arg4, int arg5) {
        return cv::threshold(arg1, arg2, arg3, arg4, arg5);
    }
    
    Scalar trace_wrapper(const cv::Mat& arg1) {
        return cv::trace(arg1);
    }
    
    void transform_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3) {
        return cv::transform(arg1, arg2, arg3);
    }
    
    void transpose_wrapper(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::transpose(arg1, arg2);
    }
    
    void undistort_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, const cv::Mat& arg5) {
        return cv::undistort(arg1, arg2, arg3, arg4, arg5);
    }
    
    void undistort_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4) {
        return cv::undistort(arg1, arg2, arg3, arg4);
    }
    
    void vconcat_wrapper(const std::vector<cv::Mat>& arg1, cv::Mat& arg2) {
        return cv::vconcat(arg1, arg2);
    }
    
    void warpAffine_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4, int arg5, int arg6, const Scalar& arg7) {
        return cv::warpAffine(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void warpAffine_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4, int arg5, int arg6) {
        return cv::warpAffine(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void warpAffine_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4, int arg5) {
        return cv::warpAffine(arg1, arg2, arg3, arg4, arg5);
    }
    
    void warpAffine_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4) {
        return cv::warpAffine(arg1, arg2, arg3, arg4);
    }
    
    void warpPerspective_wrapper(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4, int arg5, int arg6, const Scalar& arg7) {
        return cv::warpPerspective(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void warpPerspective_wrapper_1(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4, int arg5, int arg6) {
        return cv::warpPerspective(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void warpPerspective_wrapper_2(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4, int arg5) {
        return cv::warpPerspective(arg1, arg2, arg3, arg4, arg5);
    }
    
    void warpPerspective_wrapper_3(const cv::Mat& arg1, cv::Mat& arg2, const cv::Mat& arg3, Size arg4) {
        return cv::warpPerspective(arg1, arg2, arg3, arg4);
    }
    
    void warpPolar_wrapper(const cv::Mat& arg1, cv::Mat& arg2, Size arg3, Point2f arg4, double arg5, int arg6) {
        return cv::warpPolar(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void watershed_wrapper(const cv::Mat& arg1, cv::Mat& arg2) {
        return cv::watershed(arg1, arg2);
    }
    
    Mat blobFromImage_wrapper(const cv::Mat& arg1, double arg2, const Size& arg3, const Scalar& arg4, bool arg5, bool arg6, int arg7) {
        return cv::dnn::blobFromImage(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    Mat blobFromImage_wrapper_1(const cv::Mat& arg1, double arg2, const Size& arg3, const Scalar& arg4, bool arg5, bool arg6) {
        return cv::dnn::blobFromImage(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Mat blobFromImage_wrapper_2(const cv::Mat& arg1, double arg2, const Size& arg3, const Scalar& arg4, bool arg5) {
        return cv::dnn::blobFromImage(arg1, arg2, arg3, arg4, arg5);
    }
    
    Mat blobFromImage_wrapper_3(const cv::Mat& arg1, double arg2, const Size& arg3, const Scalar& arg4) {
        return cv::dnn::blobFromImage(arg1, arg2, arg3, arg4);
    }
    
    Mat blobFromImage_wrapper_4(const cv::Mat& arg1, double arg2, const Size& arg3) {
        return cv::dnn::blobFromImage(arg1, arg2, arg3);
    }
    
    Mat blobFromImage_wrapper_5(const cv::Mat& arg1, double arg2) {
        return cv::dnn::blobFromImage(arg1, arg2);
    }
    
    Mat blobFromImage_wrapper_6(const cv::Mat& arg1) {
        return cv::dnn::blobFromImage(arg1);
    }
    
    Net readNet_wrapper(const std::string& arg1, const std::string& arg2, const std::string& arg3) {
        return cv::dnn::readNet(arg1, arg2, arg3);
    }
    
    Net readNet_wrapper_1(const std::string& arg1, const std::string& arg2) {
        return cv::dnn::readNet(arg1, arg2);
    }
    
    Net readNet_wrapper_2(const std::string& arg1) {
        return cv::dnn::readNet(arg1);
    }
    
    Net readNet_wrapper1(const std::string& arg1, const emscripten::val& arg2, const emscripten::val& arg3) {
        return cv::dnn::readNet(arg1, emscripten::vecFromJSArray<uchar>(arg2), emscripten::vecFromJSArray<uchar>(arg3));
    }
    
    Net readNet_wrapper1_1(const std::string& arg1, const emscripten::val& arg2) {
        return cv::dnn::readNet(arg1, emscripten::vecFromJSArray<uchar>(arg2));
    }
    
    Net readNetFromCaffe_wrapper(const std::string& arg1, const std::string& arg2) {
        return cv::dnn::readNetFromCaffe(arg1, arg2);
    }
    
    Net readNetFromCaffe_wrapper_1(const std::string& arg1) {
        return cv::dnn::readNetFromCaffe(arg1);
    }
    
    Net readNetFromCaffe_wrapper1(const emscripten::val& arg1, const emscripten::val& arg2) {
        return cv::dnn::readNetFromCaffe(emscripten::vecFromJSArray<uchar>(arg1), emscripten::vecFromJSArray<uchar>(arg2));
    }
    
    Net readNetFromCaffe_wrapper1_1(const emscripten::val& arg1) {
        return cv::dnn::readNetFromCaffe(emscripten::vecFromJSArray<uchar>(arg1));
    }
    
    Net readNetFromDarknet_wrapper(const std::string& arg1, const std::string& arg2) {
        return cv::dnn::readNetFromDarknet(arg1, arg2);
    }
    
    Net readNetFromDarknet_wrapper_1(const std::string& arg1) {
        return cv::dnn::readNetFromDarknet(arg1);
    }
    
    Net readNetFromDarknet_wrapper1(const emscripten::val& arg1, const emscripten::val& arg2) {
        return cv::dnn::readNetFromDarknet(emscripten::vecFromJSArray<uchar>(arg1), emscripten::vecFromJSArray<uchar>(arg2));
    }
    
    Net readNetFromDarknet_wrapper1_1(const emscripten::val& arg1) {
        return cv::dnn::readNetFromDarknet(emscripten::vecFromJSArray<uchar>(arg1));
    }
    
    Net readNetFromONNX_wrapper(const std::string& arg1) {
        return cv::dnn::readNetFromONNX(arg1);
    }
    
    Net readNetFromONNX_wrapper1(const emscripten::val& arg1) {
        return cv::dnn::readNetFromONNX(emscripten::vecFromJSArray<uchar>(arg1));
    }
    
    Net readNetFromTensorflow_wrapper(const std::string& arg1, const std::string& arg2) {
        return cv::dnn::readNetFromTensorflow(arg1, arg2);
    }
    
    Net readNetFromTensorflow_wrapper_1(const std::string& arg1) {
        return cv::dnn::readNetFromTensorflow(arg1);
    }
    
    Net readNetFromTensorflow_wrapper1(const emscripten::val& arg1, const emscripten::val& arg2) {
        return cv::dnn::readNetFromTensorflow(emscripten::vecFromJSArray<uchar>(arg1), emscripten::vecFromJSArray<uchar>(arg2));
    }
    
    Net readNetFromTensorflow_wrapper1_1(const emscripten::val& arg1) {
        return cv::dnn::readNetFromTensorflow(emscripten::vecFromJSArray<uchar>(arg1));
    }
    
    Net readNetFromTorch_wrapper(const std::string& arg1, bool arg2, bool arg3) {
        return cv::dnn::readNetFromTorch(arg1, arg2, arg3);
    }
    
    Net readNetFromTorch_wrapper_1(const std::string& arg1, bool arg2) {
        return cv::dnn::readNetFromTorch(arg1, arg2);
    }
    
    Net readNetFromTorch_wrapper_2(const std::string& arg1) {
        return cv::dnn::readNetFromTorch(arg1);
    }
    
    void initUndistortRectifyMap_wrapper(const cv::Mat& arg1, const cv::Mat& arg2, const cv::Mat& arg3, const cv::Mat& arg4, const Size& arg5, int arg6, cv::Mat& arg7, cv::Mat& arg8) {
        return cv::fisheye::initUndistortRectifyMap(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper(int arg1, int arg2, int arg3, float arg4, int arg5, int arg6, int arg7) {
        return cv::AKAZE::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_1(int arg1, int arg2, int arg3, float arg4, int arg5, int arg6) {
        return cv::AKAZE::create(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_2(int arg1, int arg2, int arg3, float arg4, int arg5) {
        return cv::AKAZE::create(arg1, arg2, arg3, arg4, arg5);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_3(int arg1, int arg2, int arg3, float arg4) {
        return cv::AKAZE::create(arg1, arg2, arg3, arg4);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_4(int arg1, int arg2, int arg3) {
        return cv::AKAZE::create(arg1, arg2, arg3);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_5(int arg1, int arg2) {
        return cv::AKAZE::create(arg1, arg2);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_6(int arg1) {
        return cv::AKAZE::create(arg1);
    }
    
    Ptr<AKAZE> AKAZE_create_wrapper_7() {
        return cv::AKAZE::create();
    }
    
    std::string AKAZE_getDefaultName_wrapper(cv::AKAZE& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void AKAZE_setDescriptorChannels_wrapper(cv::AKAZE& arg0 , int arg1) {
        return arg0.setDescriptorChannels(arg1);
    }
    
    void AKAZE_setDescriptorSize_wrapper(cv::AKAZE& arg0 , int arg1) {
        return arg0.setDescriptorSize(arg1);
    }
    
    void AKAZE_setDescriptorType_wrapper(cv::AKAZE& arg0 , int arg1) {
        return arg0.setDescriptorType(arg1);
    }
    
    void AKAZE_setDiffusivity_wrapper(cv::AKAZE& arg0 , int arg1) {
        return arg0.setDiffusivity(arg1);
    }
    
    void AKAZE_setNOctaveLayers_wrapper(cv::AKAZE& arg0 , int arg1) {
        return arg0.setNOctaveLayers(arg1);
    }
    
    void AKAZE_setNOctaves_wrapper(cv::AKAZE& arg0 , int arg1) {
        return arg0.setNOctaves(arg1);
    }
    
    void AKAZE_setThreshold_wrapper(cv::AKAZE& arg0 , double arg1) {
        return arg0.setThreshold(arg1);
    }
    
    Ptr<AgastFeatureDetector> AgastFeatureDetector_create_wrapper(int arg1, bool arg2, int arg3) {
        return cv::AgastFeatureDetector::create(arg1, arg2, arg3);
    }
    
    Ptr<AgastFeatureDetector> AgastFeatureDetector_create_wrapper_1(int arg1, bool arg2) {
        return cv::AgastFeatureDetector::create(arg1, arg2);
    }
    
    Ptr<AgastFeatureDetector> AgastFeatureDetector_create_wrapper_2(int arg1) {
        return cv::AgastFeatureDetector::create(arg1);
    }
    
    Ptr<AgastFeatureDetector> AgastFeatureDetector_create_wrapper_3() {
        return cv::AgastFeatureDetector::create();
    }
    
    std::string AgastFeatureDetector_getDefaultName_wrapper(cv::AgastFeatureDetector& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void AgastFeatureDetector_setNonmaxSuppression_wrapper(cv::AgastFeatureDetector& arg0 , bool arg1) {
        return arg0.setNonmaxSuppression(arg1);
    }
    
    void AgastFeatureDetector_setThreshold_wrapper(cv::AgastFeatureDetector& arg0 , int arg1) {
        return arg0.setThreshold(arg1);
    }
    
    void AgastFeatureDetector_setType_wrapper(cv::AgastFeatureDetector& arg0 , int arg1) {
        return arg0.setType(arg1);
    }
    
    Ptr<BFMatcher> BFMatcher_create_wrapper(int arg1, bool arg2) {
        return cv::BFMatcher::create(arg1, arg2);
    }
    
    Ptr<BFMatcher> BFMatcher_create_wrapper_1(int arg1) {
        return cv::BFMatcher::create(arg1);
    }
    
    Ptr<BFMatcher> BFMatcher_create_wrapper_2() {
        return cv::BFMatcher::create();
    }
    
    Ptr<BRISK> BRISK_create_wrapper(int arg1, int arg2, float arg3) {
        return cv::BRISK::create(arg1, arg2, arg3);
    }
    
    Ptr<BRISK> BRISK_create_wrapper_1(int arg1, int arg2) {
        return cv::BRISK::create(arg1, arg2);
    }
    
    Ptr<BRISK> BRISK_create_wrapper_2(int arg1) {
        return cv::BRISK::create(arg1);
    }
    
    Ptr<BRISK> BRISK_create_wrapper_3() {
        return cv::BRISK::create();
    }
    
    Ptr<BRISK> BRISK_create_wrapper1(const emscripten::val& arg1, const emscripten::val& arg2, float arg3, float arg4, const emscripten::val& arg5) {
        return cv::BRISK::create(emscripten::vecFromJSArray<float>(arg1), emscripten::vecFromJSArray<int>(arg2), arg3, arg4, emscripten::vecFromJSArray<int>(arg5));
    }
    
    Ptr<BRISK> BRISK_create_wrapper1_1(const emscripten::val& arg1, const emscripten::val& arg2, float arg3, float arg4) {
        return cv::BRISK::create(emscripten::vecFromJSArray<float>(arg1), emscripten::vecFromJSArray<int>(arg2), arg3, arg4);
    }
    
    Ptr<BRISK> BRISK_create_wrapper2(int arg1, int arg2, const emscripten::val& arg3, const emscripten::val& arg4, float arg5, float arg6, const emscripten::val& arg7) {
        return cv::BRISK::create(arg1, arg2, emscripten::vecFromJSArray<float>(arg3), emscripten::vecFromJSArray<int>(arg4), arg5, arg6, emscripten::vecFromJSArray<int>(arg7));
    }
    
    Ptr<BRISK> BRISK_create_wrapper2_1(int arg1, int arg2, const emscripten::val& arg3, const emscripten::val& arg4, float arg5, float arg6) {
        return cv::BRISK::create(arg1, arg2, emscripten::vecFromJSArray<float>(arg3), emscripten::vecFromJSArray<int>(arg4), arg5, arg6);
    }
    
    std::string BRISK_getDefaultName_wrapper(cv::BRISK& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void BackgroundSubtractor_apply_wrapper(cv::BackgroundSubtractor& arg0 , const cv::Mat& arg1, cv::Mat& arg2, double arg3) {
        return arg0.apply(arg1, arg2, arg3);
    }
    
    void BackgroundSubtractor_apply_wrapper_1(cv::BackgroundSubtractor& arg0 , const cv::Mat& arg1, cv::Mat& arg2) {
        return arg0.apply(arg1, arg2);
    }
    
    void BackgroundSubtractor_getBackgroundImage_wrapper(cv::BackgroundSubtractor& arg0 , cv::Mat& arg1) {
        return arg0.getBackgroundImage(arg1);
    }
    
    void BackgroundSubtractorMOG2_apply_wrapper(cv::BackgroundSubtractorMOG2& arg0 , const cv::Mat& arg1, cv::Mat& arg2, double arg3) {
        return arg0.apply(arg1, arg2, arg3);
    }
    
    void BackgroundSubtractorMOG2_apply_wrapper_1(cv::BackgroundSubtractorMOG2& arg0 , const cv::Mat& arg1, cv::Mat& arg2) {
        return arg0.apply(arg1, arg2);
    }
    
    Ptr<BackgroundSubtractorMOG2> _createBackgroundSubtractorMOG2_wrapper(int arg1, double arg2, bool arg3) {
        return cv::createBackgroundSubtractorMOG2(arg1, arg2, arg3);
    }
    
    Ptr<BackgroundSubtractorMOG2> _createBackgroundSubtractorMOG2_wrapper_1(int arg1, double arg2) {
        return cv::createBackgroundSubtractorMOG2(arg1, arg2);
    }
    
    Ptr<BackgroundSubtractorMOG2> _createBackgroundSubtractorMOG2_wrapper_2(int arg1) {
        return cv::createBackgroundSubtractorMOG2(arg1);
    }
    
    Ptr<BackgroundSubtractorMOG2> _createBackgroundSubtractorMOG2_wrapper_3() {
        return cv::createBackgroundSubtractorMOG2();
    }
    
    void CLAHE_apply_wrapper(cv::CLAHE& arg0 , const cv::Mat& arg1, cv::Mat& arg2) {
        return arg0.apply(arg1, arg2);
    }
    
    Ptr<CLAHE> _createCLAHE_wrapper(double arg1, Size arg2) {
        return cv::createCLAHE(arg1, arg2);
    }
    
    Ptr<CLAHE> _createCLAHE_wrapper_1(double arg1) {
        return cv::createCLAHE(arg1);
    }
    
    Ptr<CLAHE> _createCLAHE_wrapper_2() {
        return cv::createCLAHE();
    }
    
    void CLAHE_setClipLimit_wrapper(cv::CLAHE& arg0 , double arg1) {
        return arg0.setClipLimit(arg1);
    }
    
    void CLAHE_setTilesGridSize_wrapper(cv::CLAHE& arg0 , Size arg1) {
        return arg0.setTilesGridSize(arg1);
    }
    
    void CascadeClassifier_detectMultiScale_wrapper(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, double arg3, int arg4, int arg5, Size arg6, Size arg7) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void CascadeClassifier_detectMultiScale_wrapper_1(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, double arg3, int arg4, int arg5, Size arg6) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void CascadeClassifier_detectMultiScale_wrapper_2(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, double arg3, int arg4, int arg5) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5);
    }
    
    void CascadeClassifier_detectMultiScale_wrapper_3(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, double arg3, int arg4) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4);
    }
    
    void CascadeClassifier_detectMultiScale_wrapper_4(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, double arg3) {
        return arg0.detectMultiScale(arg1, arg2, arg3);
    }
    
    void CascadeClassifier_detectMultiScale_wrapper_5(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2) {
        return arg0.detectMultiScale(arg1, arg2);
    }
    
    void CascadeClassifier_detectMultiScale2_wrapper(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, double arg4, int arg5, int arg6, Size arg7, Size arg8) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void CascadeClassifier_detectMultiScale2_wrapper_1(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, double arg4, int arg5, int arg6, Size arg7) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void CascadeClassifier_detectMultiScale2_wrapper_2(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, double arg4, int arg5, int arg6) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void CascadeClassifier_detectMultiScale2_wrapper_3(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, double arg4, int arg5) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5);
    }
    
    void CascadeClassifier_detectMultiScale2_wrapper_4(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, double arg4) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4);
    }
    
    void CascadeClassifier_detectMultiScale2_wrapper_5(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3) {
        return arg0.detectMultiScale(arg1, arg2, arg3);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4, double arg5, int arg6, int arg7, Size arg8, Size arg9, bool arg10) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper_1(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4, double arg5, int arg6, int arg7, Size arg8, Size arg9) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper_2(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4, double arg5, int arg6, int arg7, Size arg8) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper_3(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4, double arg5, int arg6, int arg7) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper_4(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4, double arg5, int arg6) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper_5(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4, double arg5) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5);
    }
    
    void CascadeClassifier_detectMultiScale3_wrapper_6(cv::CascadeClassifier& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<int>& arg3, std::vector<double>& arg4) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4);
    }
    
    bool CascadeClassifier_load_wrapper(cv::CascadeClassifier& arg0 , const std::string& arg1) {
        return arg0.load(arg1);
    }
    
    void DescriptorMatcher_add_wrapper(cv::DescriptorMatcher& arg0 , const std::vector<cv::Mat>& arg1) {
        return arg0.add(arg1);
    }
    
    Ptr<DescriptorMatcher> DescriptorMatcher_create_wrapper(const std::string& arg1) {
        return cv::DescriptorMatcher::create(arg1);
    }
    
    void DescriptorMatcher_knnMatch_wrapper(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<std::vector<DMatch>>& arg3, int arg4, const cv::Mat& arg5, bool arg6) {
        return arg0.knnMatch(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void DescriptorMatcher_knnMatch_wrapper_1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<std::vector<DMatch>>& arg3, int arg4, const cv::Mat& arg5) {
        return arg0.knnMatch(arg1, arg2, arg3, arg4, arg5);
    }
    
    void DescriptorMatcher_knnMatch_wrapper_2(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<std::vector<DMatch>>& arg3, int arg4) {
        return arg0.knnMatch(arg1, arg2, arg3, arg4);
    }
    
    void DescriptorMatcher_knnMatch_wrapper1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<std::vector<DMatch>>& arg2, int arg3, const std::vector<cv::Mat>& arg4, bool arg5) {
        return arg0.knnMatch(arg1, arg2, arg3, arg4, arg5);
    }
    
    void DescriptorMatcher_knnMatch_wrapper1_1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<std::vector<DMatch>>& arg2, int arg3, const std::vector<cv::Mat>& arg4) {
        return arg0.knnMatch(arg1, arg2, arg3, arg4);
    }
    
    void DescriptorMatcher_knnMatch_wrapper1_2(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<std::vector<DMatch>>& arg2, int arg3) {
        return arg0.knnMatch(arg1, arg2, arg3);
    }
    
    void DescriptorMatcher_match_wrapper(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<DMatch>& arg3, const cv::Mat& arg4) {
        return arg0.match(arg1, arg2, arg3, arg4);
    }
    
    void DescriptorMatcher_match_wrapper_1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<DMatch>& arg3) {
        return arg0.match(arg1, arg2, arg3);
    }
    
    void DescriptorMatcher_match_wrapper1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<DMatch>& arg2, const std::vector<cv::Mat>& arg3) {
        return arg0.match(arg1, arg2, arg3);
    }
    
    void DescriptorMatcher_match_wrapper1_1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<DMatch>& arg2) {
        return arg0.match(arg1, arg2);
    }
    
    void DescriptorMatcher_radiusMatch_wrapper(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<std::vector<DMatch>>& arg3, float arg4, const cv::Mat& arg5, bool arg6) {
        return arg0.radiusMatch(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void DescriptorMatcher_radiusMatch_wrapper_1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<std::vector<DMatch>>& arg3, float arg4, const cv::Mat& arg5) {
        return arg0.radiusMatch(arg1, arg2, arg3, arg4, arg5);
    }
    
    void DescriptorMatcher_radiusMatch_wrapper_2(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<std::vector<DMatch>>& arg3, float arg4) {
        return arg0.radiusMatch(arg1, arg2, arg3, arg4);
    }
    
    void DescriptorMatcher_radiusMatch_wrapper1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<std::vector<DMatch>>& arg2, float arg3, const std::vector<cv::Mat>& arg4, bool arg5) {
        return arg0.radiusMatch(arg1, arg2, arg3, arg4, arg5);
    }
    
    void DescriptorMatcher_radiusMatch_wrapper1_1(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<std::vector<DMatch>>& arg2, float arg3, const std::vector<cv::Mat>& arg4) {
        return arg0.radiusMatch(arg1, arg2, arg3, arg4);
    }
    
    void DescriptorMatcher_radiusMatch_wrapper1_2(cv::DescriptorMatcher& arg0 , const cv::Mat& arg1, std::vector<std::vector<DMatch>>& arg2, float arg3) {
        return arg0.radiusMatch(arg1, arg2, arg3);
    }
    
    Ptr<FastFeatureDetector> FastFeatureDetector_create_wrapper(int arg1, bool arg2, int arg3) {
        return cv::FastFeatureDetector::create(arg1, arg2, arg3);
    }
    
    Ptr<FastFeatureDetector> FastFeatureDetector_create_wrapper_1(int arg1, bool arg2) {
        return cv::FastFeatureDetector::create(arg1, arg2);
    }
    
    Ptr<FastFeatureDetector> FastFeatureDetector_create_wrapper_2(int arg1) {
        return cv::FastFeatureDetector::create(arg1);
    }
    
    Ptr<FastFeatureDetector> FastFeatureDetector_create_wrapper_3() {
        return cv::FastFeatureDetector::create();
    }
    
    std::string FastFeatureDetector_getDefaultName_wrapper(cv::FastFeatureDetector& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void FastFeatureDetector_setNonmaxSuppression_wrapper(cv::FastFeatureDetector& arg0 , bool arg1) {
        return arg0.setNonmaxSuppression(arg1);
    }
    
    void FastFeatureDetector_setThreshold_wrapper(cv::FastFeatureDetector& arg0 , int arg1) {
        return arg0.setThreshold(arg1);
    }
    
    void FastFeatureDetector_setType_wrapper(cv::FastFeatureDetector& arg0 , int arg1) {
        return arg0.setType(arg1);
    }
    
    void Feature2D_compute_wrapper(cv::Feature2D& arg0 , const cv::Mat& arg1, std::vector<KeyPoint>& arg2, cv::Mat& arg3) {
        return arg0.compute(arg1, arg2, arg3);
    }
    
    void Feature2D_compute_wrapper1(cv::Feature2D& arg0 , const std::vector<cv::Mat>& arg1, std::vector<std::vector<KeyPoint>>& arg2, std::vector<cv::Mat>& arg3) {
        return arg0.compute(arg1, arg2, arg3);
    }
    
    void Feature2D_detect_wrapper(cv::Feature2D& arg0 , const cv::Mat& arg1, std::vector<KeyPoint>& arg2, const cv::Mat& arg3) {
        return arg0.detect(arg1, arg2, arg3);
    }
    
    void Feature2D_detect_wrapper_1(cv::Feature2D& arg0 , const cv::Mat& arg1, std::vector<KeyPoint>& arg2) {
        return arg0.detect(arg1, arg2);
    }
    
    void Feature2D_detect_wrapper1(cv::Feature2D& arg0 , const std::vector<cv::Mat>& arg1, std::vector<std::vector<KeyPoint>>& arg2, const std::vector<cv::Mat>& arg3) {
        return arg0.detect(arg1, arg2, arg3);
    }
    
    void Feature2D_detect_wrapper1_1(cv::Feature2D& arg0 , const std::vector<cv::Mat>& arg1, std::vector<std::vector<KeyPoint>>& arg2) {
        return arg0.detect(arg1, arg2);
    }
    
    void Feature2D_detectAndCompute_wrapper(cv::Feature2D& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<KeyPoint>& arg3, cv::Mat& arg4, bool arg5) {
        return arg0.detectAndCompute(arg1, arg2, arg3, arg4, arg5);
    }
    
    void Feature2D_detectAndCompute_wrapper_1(cv::Feature2D& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, std::vector<KeyPoint>& arg3, cv::Mat& arg4) {
        return arg0.detectAndCompute(arg1, arg2, arg3, arg4);
    }
    
    std::string Feature2D_getDefaultName_wrapper(cv::Feature2D& arg0 ) {
        return arg0.getDefaultName();
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper(int arg1, double arg2, double arg3, int arg4, bool arg5, double arg6) {
        return cv::GFTTDetector::create(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper_1(int arg1, double arg2, double arg3, int arg4, bool arg5) {
        return cv::GFTTDetector::create(arg1, arg2, arg3, arg4, arg5);
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper_2(int arg1, double arg2, double arg3, int arg4) {
        return cv::GFTTDetector::create(arg1, arg2, arg3, arg4);
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper_3(int arg1, double arg2, double arg3) {
        return cv::GFTTDetector::create(arg1, arg2, arg3);
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper_4(int arg1, double arg2) {
        return cv::GFTTDetector::create(arg1, arg2);
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper_5(int arg1) {
        return cv::GFTTDetector::create(arg1);
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper_6() {
        return cv::GFTTDetector::create();
    }
    
    Ptr<GFTTDetector> GFTTDetector_create_wrapper1(int arg1, double arg2, double arg3, int arg4, int arg5, bool arg6, double arg7) {
        return cv::GFTTDetector::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    std::string GFTTDetector_getDefaultName_wrapper(cv::GFTTDetector& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void GFTTDetector_setBlockSize_wrapper(cv::GFTTDetector& arg0 , int arg1) {
        return arg0.setBlockSize(arg1);
    }
    
    void GFTTDetector_setHarrisDetector_wrapper(cv::GFTTDetector& arg0 , bool arg1) {
        return arg0.setHarrisDetector(arg1);
    }
    
    void GFTTDetector_setK_wrapper(cv::GFTTDetector& arg0 , double arg1) {
        return arg0.setK(arg1);
    }
    
    void GFTTDetector_setMaxFeatures_wrapper(cv::GFTTDetector& arg0 , int arg1) {
        return arg0.setMaxFeatures(arg1);
    }
    
    void GFTTDetector_setMinDistance_wrapper(cv::GFTTDetector& arg0 , double arg1) {
        return arg0.setMinDistance(arg1);
    }
    
    void GFTTDetector_setQualityLevel_wrapper(cv::GFTTDetector& arg0 , double arg1) {
        return arg0.setQualityLevel(arg1);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3, double arg4, Size arg5, Size arg6, double arg7, double arg8, bool arg9) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper_1(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3, double arg4, Size arg5, Size arg6, double arg7, double arg8) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper_2(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3, double arg4, Size arg5, Size arg6, double arg7) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper_3(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3, double arg4, Size arg5, Size arg6) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper_4(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3, double arg4, Size arg5) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4, arg5);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper_5(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3, double arg4) {
        return arg0.detectMultiScale(arg1, arg2, arg3, arg4);
    }
    
    void HOGDescriptor_detectMultiScale_wrapper_6(cv::HOGDescriptor& arg0 , const cv::Mat& arg1, std::vector<Rect>& arg2, std::vector<double>& arg3) {
        return arg0.detectMultiScale(arg1, arg2, arg3);
    }
    
    bool HOGDescriptor_load_wrapper(cv::HOGDescriptor& arg0 , const std::string& arg1, const std::string& arg2) {
        return arg0.load(arg1, arg2);
    }
    
    bool HOGDescriptor_load_wrapper_1(cv::HOGDescriptor& arg0 , const std::string& arg1) {
        return arg0.load(arg1);
    }
    
    void HOGDescriptor_setSVMDetector_wrapper(cv::HOGDescriptor& arg0 , const cv::Mat& arg1) {
        return arg0.setSVMDetector(arg1);
    }
    
    Ptr<KAZE> KAZE_create_wrapper(bool arg1, bool arg2, float arg3, int arg4, int arg5, int arg6) {
        return cv::KAZE::create(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Ptr<KAZE> KAZE_create_wrapper_1(bool arg1, bool arg2, float arg3, int arg4, int arg5) {
        return cv::KAZE::create(arg1, arg2, arg3, arg4, arg5);
    }
    
    Ptr<KAZE> KAZE_create_wrapper_2(bool arg1, bool arg2, float arg3, int arg4) {
        return cv::KAZE::create(arg1, arg2, arg3, arg4);
    }
    
    Ptr<KAZE> KAZE_create_wrapper_3(bool arg1, bool arg2, float arg3) {
        return cv::KAZE::create(arg1, arg2, arg3);
    }
    
    Ptr<KAZE> KAZE_create_wrapper_4(bool arg1, bool arg2) {
        return cv::KAZE::create(arg1, arg2);
    }
    
    Ptr<KAZE> KAZE_create_wrapper_5(bool arg1) {
        return cv::KAZE::create(arg1);
    }
    
    Ptr<KAZE> KAZE_create_wrapper_6() {
        return cv::KAZE::create();
    }
    
    std::string KAZE_getDefaultName_wrapper(cv::KAZE& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void KAZE_setDiffusivity_wrapper(cv::KAZE& arg0 , int arg1) {
        return arg0.setDiffusivity(arg1);
    }
    
    void KAZE_setExtended_wrapper(cv::KAZE& arg0 , bool arg1) {
        return arg0.setExtended(arg1);
    }
    
    void KAZE_setNOctaveLayers_wrapper(cv::KAZE& arg0 , int arg1) {
        return arg0.setNOctaveLayers(arg1);
    }
    
    void KAZE_setNOctaves_wrapper(cv::KAZE& arg0 , int arg1) {
        return arg0.setNOctaves(arg1);
    }
    
    void KAZE_setThreshold_wrapper(cv::KAZE& arg0 , double arg1) {
        return arg0.setThreshold(arg1);
    }
    
    void KAZE_setUpright_wrapper(cv::KAZE& arg0 , bool arg1) {
        return arg0.setUpright(arg1);
    }
    
    Ptr<MSER> MSER_create_wrapper(int arg1, int arg2, int arg3, double arg4, double arg5, int arg6, double arg7, double arg8, int arg9) {
        return cv::MSER::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    Ptr<MSER> MSER_create_wrapper_1(int arg1, int arg2, int arg3, double arg4, double arg5, int arg6, double arg7, double arg8) {
        return cv::MSER::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    Ptr<MSER> MSER_create_wrapper_2(int arg1, int arg2, int arg3, double arg4, double arg5, int arg6, double arg7) {
        return cv::MSER::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    Ptr<MSER> MSER_create_wrapper_3(int arg1, int arg2, int arg3, double arg4, double arg5, int arg6) {
        return cv::MSER::create(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Ptr<MSER> MSER_create_wrapper_4(int arg1, int arg2, int arg3, double arg4, double arg5) {
        return cv::MSER::create(arg1, arg2, arg3, arg4, arg5);
    }
    
    Ptr<MSER> MSER_create_wrapper_5(int arg1, int arg2, int arg3, double arg4) {
        return cv::MSER::create(arg1, arg2, arg3, arg4);
    }
    
    Ptr<MSER> MSER_create_wrapper_6(int arg1, int arg2, int arg3) {
        return cv::MSER::create(arg1, arg2, arg3);
    }
    
    Ptr<MSER> MSER_create_wrapper_7(int arg1, int arg2) {
        return cv::MSER::create(arg1, arg2);
    }
    
    Ptr<MSER> MSER_create_wrapper_8(int arg1) {
        return cv::MSER::create(arg1);
    }
    
    Ptr<MSER> MSER_create_wrapper_9() {
        return cv::MSER::create();
    }
    
    void MSER_detectRegions_wrapper(cv::MSER& arg0 , const cv::Mat& arg1, std::vector<std::vector<Point>>& arg2, std::vector<Rect>& arg3) {
        return arg0.detectRegions(arg1, arg2, arg3);
    }
    
    std::string MSER_getDefaultName_wrapper(cv::MSER& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void MSER_setDelta_wrapper(cv::MSER& arg0 , int arg1) {
        return arg0.setDelta(arg1);
    }
    
    void MSER_setMaxArea_wrapper(cv::MSER& arg0 , int arg1) {
        return arg0.setMaxArea(arg1);
    }
    
    void MSER_setMinArea_wrapper(cv::MSER& arg0 , int arg1) {
        return arg0.setMinArea(arg1);
    }
    
    void MSER_setPass2Only_wrapper(cv::MSER& arg0 , bool arg1) {
        return arg0.setPass2Only(arg1);
    }
    
    Ptr<ORB> ORB_create_wrapper(int arg1, float arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9) {
        return cv::ORB::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
    
    Ptr<ORB> ORB_create_wrapper_1(int arg1, float arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8) {
        return cv::ORB::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }
    
    Ptr<ORB> ORB_create_wrapper_2(int arg1, float arg2, int arg3, int arg4, int arg5, int arg6, int arg7) {
        return cv::ORB::create(arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }
    
    Ptr<ORB> ORB_create_wrapper_3(int arg1, float arg2, int arg3, int arg4, int arg5, int arg6) {
        return cv::ORB::create(arg1, arg2, arg3, arg4, arg5, arg6);
    }
    
    Ptr<ORB> ORB_create_wrapper_4(int arg1, float arg2, int arg3, int arg4, int arg5) {
        return cv::ORB::create(arg1, arg2, arg3, arg4, arg5);
    }
    
    Ptr<ORB> ORB_create_wrapper_5(int arg1, float arg2, int arg3, int arg4) {
        return cv::ORB::create(arg1, arg2, arg3, arg4);
    }
    
    Ptr<ORB> ORB_create_wrapper_6(int arg1, float arg2, int arg3) {
        return cv::ORB::create(arg1, arg2, arg3);
    }
    
    Ptr<ORB> ORB_create_wrapper_7(int arg1, float arg2) {
        return cv::ORB::create(arg1, arg2);
    }
    
    Ptr<ORB> ORB_create_wrapper_8(int arg1) {
        return cv::ORB::create(arg1);
    }
    
    Ptr<ORB> ORB_create_wrapper_9() {
        return cv::ORB::create();
    }
    
    std::string ORB_getDefaultName_wrapper(cv::ORB& arg0 ) {
        return arg0.getDefaultName();
    }
    
    void ORB_setEdgeThreshold_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setEdgeThreshold(arg1);
    }
    
    void ORB_setFirstLevel_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setFirstLevel(arg1);
    }
    
    void ORB_setMaxFeatures_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setMaxFeatures(arg1);
    }
    
    void ORB_setNLevels_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setNLevels(arg1);
    }
    
    void ORB_setPatchSize_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setPatchSize(arg1);
    }
    
    void ORB_setScaleFactor_wrapper(cv::ORB& arg0 , double arg1) {
        return arg0.setScaleFactor(arg1);
    }
    
    void ORB_setScoreType_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setScoreType(arg1);
    }
    
    void ORB_setWTA_K_wrapper(cv::ORB& arg0 , int arg1) {
        return arg0.setWTA_K(arg1);
    }
    
    std::string QRCodeDetector_decode_wrapper(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return arg0.decode(arg1, arg2, arg3);
    }
    
    std::string QRCodeDetector_decode_wrapper_1(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, const cv::Mat& arg2) {
        return arg0.decode(arg1, arg2);
    }
    
    std::string QRCodeDetector_decodeCurved_wrapper(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, const cv::Mat& arg2, cv::Mat& arg3) {
        return arg0.decodeCurved(arg1, arg2, arg3);
    }
    
    std::string QRCodeDetector_decodeCurved_wrapper_1(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, const cv::Mat& arg2) {
        return arg0.decodeCurved(arg1, arg2);
    }
    
    bool QRCodeDetector_detect_wrapper(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, cv::Mat& arg2) {
        return arg0.detect(arg1, arg2);
    }
    
    std::string QRCodeDetector_detectAndDecode_wrapper(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, cv::Mat& arg2, cv::Mat& arg3) {
        return arg0.detectAndDecode(arg1, arg2, arg3);
    }
    
    std::string QRCodeDetector_detectAndDecode_wrapper_1(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, cv::Mat& arg2) {
        return arg0.detectAndDecode(arg1, arg2);
    }
    
    std::string QRCodeDetector_detectAndDecode_wrapper_2(cv::QRCodeDetector& arg0 , const cv::Mat& arg1) {
        return arg0.detectAndDecode(arg1);
    }
    
    bool QRCodeDetector_detectMulti_wrapper(cv::QRCodeDetector& arg0 , const cv::Mat& arg1, cv::Mat& arg2) {
        return arg0.detectMulti(arg1, arg2);
    }
    
    void QRCodeDetector_setEpsX_wrapper(cv::QRCodeDetector& arg0 , double arg1) {
        return arg0.setEpsX(arg1);
    }
    
    void QRCodeDetector_setEpsY_wrapper(cv::QRCodeDetector& arg0 , double arg1) {
        return arg0.setEpsY(arg1);
    }
    
    Mat dnn_Net_forward_wrapper(cv::dnn::Net& arg0 , const std::string& arg1) {
        return arg0.forward(arg1);
    }
    
    Mat dnn_Net_forward_wrapper_1(cv::dnn::Net& arg0 ) {
        return arg0.forward();
    }
    
    void dnn_Net_forward_wrapper1(cv::dnn::Net& arg0 , std::vector<cv::Mat>& arg1, const std::string& arg2) {
        return arg0.forward(arg1, arg2);
    }
    
    void dnn_Net_forward_wrapper1_1(cv::dnn::Net& arg0 , std::vector<cv::Mat>& arg1) {
        return arg0.forward(arg1);
    }
    
    void dnn_Net_forward_wrapper2(cv::dnn::Net& arg0 , std::vector<cv::Mat>& arg1, const emscripten::val& arg2) {
        return arg0.forward(arg1, emscripten::vecFromJSArray<String>(arg2));
    }
    
    void dnn_Net_setInput_wrapper(cv::dnn::Net& arg0 , const cv::Mat& arg1, const std::string& arg2, double arg3, const Scalar& arg4) {
        return arg0.setInput(arg1, arg2, arg3, arg4);
    }
    
    void dnn_Net_setInput_wrapper_1(cv::dnn::Net& arg0 , const cv::Mat& arg1, const std::string& arg2, double arg3) {
        return arg0.setInput(arg1, arg2, arg3);
    }
    
    void dnn_Net_setInput_wrapper_2(cv::dnn::Net& arg0 , const cv::Mat& arg1, const std::string& arg2) {
        return arg0.setInput(arg1, arg2);
    }
    
    void dnn_Net_setInput_wrapper_3(cv::dnn::Net& arg0 , const cv::Mat& arg1) {
        return arg0.setInput(arg1);
    }
    
}

EMSCRIPTEN_BINDINGS(testBinding) {
    function("Canny", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, bool)>(&Wrappers::Canny_wrapper));

    function("Canny", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int)>(&Wrappers::Canny_wrapper_1));

    function("Canny", select_overload<void(const cv::Mat&, cv::Mat&, double, double)>(&Wrappers::Canny_wrapper_2));

    function("Canny1", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double, double, bool)>(&Wrappers::Canny_wrapper1));

    function("Canny1", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double, double)>(&Wrappers::Canny_wrapper1_1));

    function("GaussianBlur", select_overload<void(const cv::Mat&, cv::Mat&, Size, double, double, int)>(&Wrappers::GaussianBlur_wrapper));

    function("GaussianBlur", select_overload<void(const cv::Mat&, cv::Mat&, Size, double, double)>(&Wrappers::GaussianBlur_wrapper_1));

    function("GaussianBlur", select_overload<void(const cv::Mat&, cv::Mat&, Size, double)>(&Wrappers::GaussianBlur_wrapper_2));

    function("HoughCircles", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, double, double, int, int)>(&Wrappers::HoughCircles_wrapper));

    function("HoughCircles", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, double, double, int)>(&Wrappers::HoughCircles_wrapper_1));

    function("HoughCircles", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, double, double)>(&Wrappers::HoughCircles_wrapper_2));

    function("HoughCircles", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, double)>(&Wrappers::HoughCircles_wrapper_3));

    function("HoughCircles", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double)>(&Wrappers::HoughCircles_wrapper_4));

    function("HoughLines", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, double, double, double, double)>(&Wrappers::HoughLines_wrapper));

    function("HoughLines", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, double, double, double)>(&Wrappers::HoughLines_wrapper_1));

    function("HoughLines", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, double, double)>(&Wrappers::HoughLines_wrapper_2));

    function("HoughLines", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, double)>(&Wrappers::HoughLines_wrapper_3));

    function("HoughLines", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int)>(&Wrappers::HoughLines_wrapper_4));

    function("HoughLinesP", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, double, double)>(&Wrappers::HoughLinesP_wrapper));

    function("HoughLinesP", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, double)>(&Wrappers::HoughLinesP_wrapper_1));

    function("HoughLinesP", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int)>(&Wrappers::HoughLinesP_wrapper_2));

    function("Laplacian", select_overload<void(const cv::Mat&, cv::Mat&, int, int, double, double, int)>(&Wrappers::Laplacian_wrapper));

    function("Laplacian", select_overload<void(const cv::Mat&, cv::Mat&, int, int, double, double)>(&Wrappers::Laplacian_wrapper_1));

    function("Laplacian", select_overload<void(const cv::Mat&, cv::Mat&, int, int, double)>(&Wrappers::Laplacian_wrapper_2));

    function("Laplacian", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::Laplacian_wrapper_3));

    function("Laplacian", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::Laplacian_wrapper_4));

    function("Rodrigues", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::Rodrigues_wrapper));

    function("Rodrigues", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::Rodrigues_wrapper_1));

    function("Scharr", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, double, double, int)>(&Wrappers::Scharr_wrapper));

    function("Scharr", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, double, double)>(&Wrappers::Scharr_wrapper_1));

    function("Scharr", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, double)>(&Wrappers::Scharr_wrapper_2));

    function("Scharr", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int)>(&Wrappers::Scharr_wrapper_3));

    function("Sobel", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, int, double, double, int)>(&Wrappers::Sobel_wrapper));

    function("Sobel", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, int, double, double)>(&Wrappers::Sobel_wrapper_1));

    function("Sobel", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, int, double)>(&Wrappers::Sobel_wrapper_2));

    function("Sobel", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, int)>(&Wrappers::Sobel_wrapper_3));

    function("Sobel", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int)>(&Wrappers::Sobel_wrapper_4));

    function("absdiff", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::absdiff_wrapper));

    function("adaptiveThreshold", select_overload<void(const cv::Mat&, cv::Mat&, double, int, int, int, double)>(&Wrappers::adaptiveThreshold_wrapper));

    function("add", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&, int)>(&Wrappers::add_wrapper));

    function("add", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::add_wrapper_1));

    function("add", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::add_wrapper_2));

    function("addWeighted", select_overload<void(const cv::Mat&, double, const cv::Mat&, double, double, cv::Mat&, int)>(&Wrappers::addWeighted_wrapper));

    function("addWeighted", select_overload<void(const cv::Mat&, double, const cv::Mat&, double, double, cv::Mat&)>(&Wrappers::addWeighted_wrapper_1));

    function("approxPolyDP", select_overload<void(const cv::Mat&, cv::Mat&, double, bool)>(&Wrappers::approxPolyDP_wrapper));

    function("arcLength", select_overload<double(const cv::Mat&, bool)>(&Wrappers::arcLength_wrapper));

    function("bilateralFilter", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, int)>(&Wrappers::bilateralFilter_wrapper));

    function("bilateralFilter", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double)>(&Wrappers::bilateralFilter_wrapper_1));

    function("bitwise_and", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::bitwise_and_wrapper));

    function("bitwise_and", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::bitwise_and_wrapper_1));

    function("bitwise_not", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::bitwise_not_wrapper));

    function("bitwise_not", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::bitwise_not_wrapper_1));

    function("bitwise_or", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::bitwise_or_wrapper));

    function("bitwise_or", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::bitwise_or_wrapper_1));

    function("bitwise_xor", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::bitwise_xor_wrapper));

    function("bitwise_xor", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::bitwise_xor_wrapper_1));

    function("blur", select_overload<void(const cv::Mat&, cv::Mat&, Size, Point, int)>(&Wrappers::blur_wrapper));

    function("blur", select_overload<void(const cv::Mat&, cv::Mat&, Size, Point)>(&Wrappers::blur_wrapper_1));

    function("blur", select_overload<void(const cv::Mat&, cv::Mat&, Size)>(&Wrappers::blur_wrapper_2));

    function("boundingRect", select_overload<Rect(const cv::Mat&)>(&Wrappers::boundingRect_wrapper));

    function("boxFilter", select_overload<void(const cv::Mat&, cv::Mat&, int, Size, Point, bool, int)>(&Wrappers::boxFilter_wrapper));

    function("boxFilter", select_overload<void(const cv::Mat&, cv::Mat&, int, Size, Point, bool)>(&Wrappers::boxFilter_wrapper_1));

    function("boxFilter", select_overload<void(const cv::Mat&, cv::Mat&, int, Size, Point)>(&Wrappers::boxFilter_wrapper_2));

    function("boxFilter", select_overload<void(const cv::Mat&, cv::Mat&, int, Size)>(&Wrappers::boxFilter_wrapper_3));

    function("calcBackProject", select_overload<void(const std::vector<cv::Mat>&, const emscripten::val&, const cv::Mat&, cv::Mat&, const emscripten::val&, double)>(&Wrappers::calcBackProject_wrapper));

    function("calcHist", select_overload<void(const std::vector<cv::Mat>&, const emscripten::val&, const cv::Mat&, cv::Mat&, const emscripten::val&, const emscripten::val&, bool)>(&Wrappers::calcHist_wrapper));

    function("calcHist", select_overload<void(const std::vector<cv::Mat>&, const emscripten::val&, const cv::Mat&, cv::Mat&, const emscripten::val&, const emscripten::val&)>(&Wrappers::calcHist_wrapper_1));

    function("calcOpticalFlowFarneback", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double, int, int, int, int, double, int)>(&Wrappers::calcOpticalFlowFarneback_wrapper));

    function("calcOpticalFlowPyrLK", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, Size, int, TermCriteria, int, double)>(&Wrappers::calcOpticalFlowPyrLK_wrapper));

    function("calcOpticalFlowPyrLK", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, Size, int, TermCriteria, int)>(&Wrappers::calcOpticalFlowPyrLK_wrapper_1));

    function("calcOpticalFlowPyrLK", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, Size, int, TermCriteria)>(&Wrappers::calcOpticalFlowPyrLK_wrapper_2));

    function("calcOpticalFlowPyrLK", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, Size, int)>(&Wrappers::calcOpticalFlowPyrLK_wrapper_3));

    function("calcOpticalFlowPyrLK", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, Size)>(&Wrappers::calcOpticalFlowPyrLK_wrapper_4));

    function("calcOpticalFlowPyrLK", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::calcOpticalFlowPyrLK_wrapper_5));

    function("calibrateCameraExtended", select_overload<double(const std::vector<cv::Mat>&, const std::vector<cv::Mat>&, Size, cv::Mat&, cv::Mat&, std::vector<cv::Mat>&, std::vector<cv::Mat>&, cv::Mat&, cv::Mat&, cv::Mat&, int, TermCriteria)>(&Wrappers::calibrateCameraExtended_wrapper));

    function("calibrateCameraExtended", select_overload<double(const std::vector<cv::Mat>&, const std::vector<cv::Mat>&, Size, cv::Mat&, cv::Mat&, std::vector<cv::Mat>&, std::vector<cv::Mat>&, cv::Mat&, cv::Mat&, cv::Mat&, int)>(&Wrappers::calibrateCameraExtended_wrapper_1));

    function("calibrateCameraExtended", select_overload<double(const std::vector<cv::Mat>&, const std::vector<cv::Mat>&, Size, cv::Mat&, cv::Mat&, std::vector<cv::Mat>&, std::vector<cv::Mat>&, cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::calibrateCameraExtended_wrapper_2));

    function("cartToPolar", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool)>(&Wrappers::cartToPolar_wrapper));

    function("cartToPolar", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::cartToPolar_wrapper_1));

    function("circle", select_overload<void(cv::Mat&, Point, int, const Scalar&, int, int, int)>(&Wrappers::circle_wrapper));

    function("circle", select_overload<void(cv::Mat&, Point, int, const Scalar&, int, int)>(&Wrappers::circle_wrapper_1));

    function("circle", select_overload<void(cv::Mat&, Point, int, const Scalar&, int)>(&Wrappers::circle_wrapper_2));

    function("circle", select_overload<void(cv::Mat&, Point, int, const Scalar&)>(&Wrappers::circle_wrapper_3));

    function("compare", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, int)>(&Wrappers::compare_wrapper));

    function("compareHist", select_overload<double(const cv::Mat&, const cv::Mat&, int)>(&Wrappers::compareHist_wrapper));

    function("connectedComponents", select_overload<int(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::connectedComponents_wrapper));

    function("connectedComponents", select_overload<int(const cv::Mat&, cv::Mat&, int)>(&Wrappers::connectedComponents_wrapper_1));

    function("connectedComponents", select_overload<int(const cv::Mat&, cv::Mat&)>(&Wrappers::connectedComponents_wrapper_2));

    function("connectedComponentsWithStats", select_overload<int(const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, int, int)>(&Wrappers::connectedComponentsWithStats_wrapper));

    function("connectedComponentsWithStats", select_overload<int(const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&, int)>(&Wrappers::connectedComponentsWithStats_wrapper_1));

    function("connectedComponentsWithStats", select_overload<int(const cv::Mat&, cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::connectedComponentsWithStats_wrapper_2));

    function("contourArea", select_overload<double(const cv::Mat&, bool)>(&Wrappers::contourArea_wrapper));

    function("contourArea", select_overload<double(const cv::Mat&)>(&Wrappers::contourArea_wrapper_1));

    function("convertScaleAbs", select_overload<void(const cv::Mat&, cv::Mat&, double, double)>(&Wrappers::convertScaleAbs_wrapper));

    function("convertScaleAbs", select_overload<void(const cv::Mat&, cv::Mat&, double)>(&Wrappers::convertScaleAbs_wrapper_1));

    function("convertScaleAbs", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::convertScaleAbs_wrapper_2));

    function("convexHull", select_overload<void(const cv::Mat&, cv::Mat&, bool, bool)>(&Wrappers::convexHull_wrapper));

    function("convexHull", select_overload<void(const cv::Mat&, cv::Mat&, bool)>(&Wrappers::convexHull_wrapper_1));

    function("convexHull", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::convexHull_wrapper_2));

    function("convexityDefects", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::convexityDefects_wrapper));

    function("copyMakeBorder", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, int, int, const Scalar&)>(&Wrappers::copyMakeBorder_wrapper));

    function("copyMakeBorder", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int, int, int)>(&Wrappers::copyMakeBorder_wrapper_1));

    function("cornerHarris", select_overload<void(const cv::Mat&, cv::Mat&, int, int, double, int)>(&Wrappers::cornerHarris_wrapper));

    function("cornerHarris", select_overload<void(const cv::Mat&, cv::Mat&, int, int, double)>(&Wrappers::cornerHarris_wrapper_1));

    function("cornerMinEigenVal", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int)>(&Wrappers::cornerMinEigenVal_wrapper));

    function("cornerMinEigenVal", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::cornerMinEigenVal_wrapper_1));

    function("cornerMinEigenVal", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::cornerMinEigenVal_wrapper_2));

    function("countNonZero", select_overload<int(const cv::Mat&)>(&Wrappers::countNonZero_wrapper));

    function("cvtColor", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::cvtColor_wrapper));

    function("cvtColor", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::cvtColor_wrapper_1));

    function("demosaicing", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::demosaicing_wrapper));

    function("demosaicing", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::demosaicing_wrapper_1));

    function("determinant", select_overload<double(const cv::Mat&)>(&Wrappers::determinant_wrapper));

    function("dft", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::dft_wrapper));

    function("dft", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::dft_wrapper_1));

    function("dft", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::dft_wrapper_2));

    function("dilate", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point, int, int, const Scalar&)>(&Wrappers::dilate_wrapper));

    function("dilate", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point, int, int)>(&Wrappers::dilate_wrapper_1));

    function("dilate", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point, int)>(&Wrappers::dilate_wrapper_2));

    function("dilate", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point)>(&Wrappers::dilate_wrapper_3));

    function("dilate", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::dilate_wrapper_4));

    function("distanceTransform", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int)>(&Wrappers::distanceTransform_wrapper));

    function("distanceTransform", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::distanceTransform_wrapper_1));

    function("distanceTransformWithLabels", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&, int, int, int)>(&Wrappers::distanceTransformWithLabels_wrapper));

    function("distanceTransformWithLabels", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&, int, int)>(&Wrappers::distanceTransformWithLabels_wrapper_1));

    function("divide", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double, int)>(&Wrappers::divide_wrapper));

    function("divide", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double)>(&Wrappers::divide_wrapper_1));

    function("divide", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::divide_wrapper_2));

    function("divide1", select_overload<void(double, const cv::Mat&, cv::Mat&, int)>(&Wrappers::divide_wrapper1));

    function("divide1", select_overload<void(double, const cv::Mat&, cv::Mat&)>(&Wrappers::divide_wrapper1_1));

    function("drawContours", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, int, const Scalar&, int, int, const cv::Mat&, int, Point)>(&Wrappers::drawContours_wrapper));

    function("drawContours", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, int, const Scalar&, int, int, const cv::Mat&, int)>(&Wrappers::drawContours_wrapper_1));

    function("drawContours", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, int, const Scalar&, int, int, const cv::Mat&)>(&Wrappers::drawContours_wrapper_2));

    function("drawContours", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, int, const Scalar&, int, int)>(&Wrappers::drawContours_wrapper_3));

    function("drawContours", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, int, const Scalar&, int)>(&Wrappers::drawContours_wrapper_4));

    function("drawContours", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, int, const Scalar&)>(&Wrappers::drawContours_wrapper_5));

    function("drawFrameAxes", select_overload<void(cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, float, int)>(&Wrappers::drawFrameAxes_wrapper));

    function("drawFrameAxes", select_overload<void(cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, float)>(&Wrappers::drawFrameAxes_wrapper_1));

    function("drawKeypoints", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, cv::Mat&, const Scalar&, int)>(&Wrappers::drawKeypoints_wrapper));

    function("drawKeypoints", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, cv::Mat&, const Scalar&)>(&Wrappers::drawKeypoints_wrapper_1));

    function("drawKeypoints", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, cv::Mat&)>(&Wrappers::drawKeypoints_wrapper_2));

    function("drawMatches", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<DMatch>&, cv::Mat&, const Scalar&, const Scalar&, const emscripten::val&, int)>(&Wrappers::drawMatches_wrapper));

    function("drawMatches", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<DMatch>&, cv::Mat&, const Scalar&, const Scalar&, const emscripten::val&)>(&Wrappers::drawMatches_wrapper_1));

    function("drawMatches", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<DMatch>&, cv::Mat&, const Scalar&, const Scalar&)>(&Wrappers::drawMatches_wrapper_2));

    function("drawMatches", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<DMatch>&, cv::Mat&, const Scalar&)>(&Wrappers::drawMatches_wrapper_3));

    function("drawMatches", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<DMatch>&, cv::Mat&)>(&Wrappers::drawMatches_wrapper_4));

    function("drawMatchesKnn", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<std::vector<DMatch>>&, cv::Mat&, const Scalar&, const Scalar&, const std::vector<std::vector<char>>&, int)>(&Wrappers::drawMatchesKnn_wrapper));

    function("drawMatchesKnn", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<std::vector<DMatch>>&, cv::Mat&, const Scalar&, const Scalar&, const std::vector<std::vector<char>>&)>(&Wrappers::drawMatchesKnn_wrapper_1));

    function("drawMatchesKnn", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<std::vector<DMatch>>&, cv::Mat&, const Scalar&, const Scalar&)>(&Wrappers::drawMatchesKnn_wrapper_2));

    function("drawMatchesKnn", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<std::vector<DMatch>>&, cv::Mat&, const Scalar&)>(&Wrappers::drawMatchesKnn_wrapper_3));

    function("drawMatchesKnn", select_overload<void(const cv::Mat&, const std::vector<KeyPoint>&, const cv::Mat&, const std::vector<KeyPoint>&, const std::vector<std::vector<DMatch>>&, cv::Mat&)>(&Wrappers::drawMatchesKnn_wrapper_4));

    function("eigen", select_overload<bool(const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::eigen_wrapper));

    function("eigen", select_overload<bool(const cv::Mat&, cv::Mat&)>(&Wrappers::eigen_wrapper_1));

    function("ellipse", select_overload<void(cv::Mat&, Point, Size, double, double, double, const Scalar&, int, int, int)>(&Wrappers::ellipse_wrapper));

    function("ellipse", select_overload<void(cv::Mat&, Point, Size, double, double, double, const Scalar&, int, int)>(&Wrappers::ellipse_wrapper_1));

    function("ellipse", select_overload<void(cv::Mat&, Point, Size, double, double, double, const Scalar&, int)>(&Wrappers::ellipse_wrapper_2));

    function("ellipse", select_overload<void(cv::Mat&, Point, Size, double, double, double, const Scalar&)>(&Wrappers::ellipse_wrapper_3));

    function("ellipse1", select_overload<void(cv::Mat&, const RotatedRect&, const Scalar&, int, int)>(&Wrappers::ellipse_wrapper1));

    function("ellipse1", select_overload<void(cv::Mat&, const RotatedRect&, const Scalar&, int)>(&Wrappers::ellipse_wrapper1_1));

    function("ellipse1", select_overload<void(cv::Mat&, const RotatedRect&, const Scalar&)>(&Wrappers::ellipse_wrapper1_2));

    function("ellipse2Poly", select_overload<void(Point, Size, int, int, int, int, std::vector<Point>&)>(&Wrappers::ellipse2Poly_wrapper));

    function("equalizeHist", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::equalizeHist_wrapper));

    function("erode", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point, int, int, const Scalar&)>(&Wrappers::erode_wrapper));

    function("erode", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point, int, int)>(&Wrappers::erode_wrapper_1));

    function("erode", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point, int)>(&Wrappers::erode_wrapper_2));

    function("erode", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Point)>(&Wrappers::erode_wrapper_3));

    function("erode", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::erode_wrapper_4));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&, cv::Mat&, int, double, size_t, double, size_t)>(&Wrappers::estimateAffine2D_wrapper));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&, cv::Mat&, int, double, size_t, double)>(&Wrappers::estimateAffine2D_wrapper_1));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&, cv::Mat&, int, double, size_t)>(&Wrappers::estimateAffine2D_wrapper_2));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&, cv::Mat&, int, double)>(&Wrappers::estimateAffine2D_wrapper_3));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&, cv::Mat&, int)>(&Wrappers::estimateAffine2D_wrapper_4));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::estimateAffine2D_wrapper_5));

    function("estimateAffine2D", select_overload<Mat(const cv::Mat&, const cv::Mat&)>(&Wrappers::estimateAffine2D_wrapper_6));

    function("exp", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::exp_wrapper));

    function("fillConvexPoly", select_overload<void(cv::Mat&, const cv::Mat&, const Scalar&, int, int)>(&Wrappers::fillConvexPoly_wrapper));

    function("fillConvexPoly", select_overload<void(cv::Mat&, const cv::Mat&, const Scalar&, int)>(&Wrappers::fillConvexPoly_wrapper_1));

    function("fillConvexPoly", select_overload<void(cv::Mat&, const cv::Mat&, const Scalar&)>(&Wrappers::fillConvexPoly_wrapper_2));

    function("fillPoly", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, const Scalar&, int, int, Point)>(&Wrappers::fillPoly_wrapper));

    function("fillPoly", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, const Scalar&, int, int)>(&Wrappers::fillPoly_wrapper_1));

    function("fillPoly", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, const Scalar&, int)>(&Wrappers::fillPoly_wrapper_2));

    function("fillPoly", select_overload<void(cv::Mat&, const std::vector<cv::Mat>&, const Scalar&)>(&Wrappers::fillPoly_wrapper_3));

    function("filter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point, double, int)>(&Wrappers::filter2D_wrapper));

    function("filter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point, double)>(&Wrappers::filter2D_wrapper_1));

    function("filter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point)>(&Wrappers::filter2D_wrapper_2));

    function("filter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&)>(&Wrappers::filter2D_wrapper_3));

    function("findContours", select_overload<void(cv::Mat&, std::vector<cv::Mat>&, cv::Mat&, int, int, Point)>(&Wrappers::findContours_wrapper));

    function("findContours", select_overload<void(cv::Mat&, std::vector<cv::Mat>&, cv::Mat&, int, int)>(&Wrappers::findContours_wrapper_1));

    function("findHomography", select_overload<Mat(const cv::Mat&, const cv::Mat&, int, double, cv::Mat&, const int, const double)>(&Wrappers::findHomography_wrapper));

    function("findHomography", select_overload<Mat(const cv::Mat&, const cv::Mat&, int, double, cv::Mat&, const int)>(&Wrappers::findHomography_wrapper_1));

    function("findHomography", select_overload<Mat(const cv::Mat&, const cv::Mat&, int, double, cv::Mat&)>(&Wrappers::findHomography_wrapper_2));

    function("findHomography", select_overload<Mat(const cv::Mat&, const cv::Mat&, int, double)>(&Wrappers::findHomography_wrapper_3));

    function("findHomography", select_overload<Mat(const cv::Mat&, const cv::Mat&, int)>(&Wrappers::findHomography_wrapper_4));

    function("findHomography", select_overload<Mat(const cv::Mat&, const cv::Mat&)>(&Wrappers::findHomography_wrapper_5));

    function("findTransformECC", select_overload<double(const cv::Mat&, const cv::Mat&, cv::Mat&, int, TermCriteria, const cv::Mat&, int)>(&Wrappers::findTransformECC_wrapper));

    function("fitEllipse", select_overload<RotatedRect(const cv::Mat&)>(&Wrappers::fitEllipse_wrapper));

    function("fitLine", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, double)>(&Wrappers::fitLine_wrapper));

    function("flip", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::flip_wrapper));

    function("gemm", select_overload<void(const cv::Mat&, const cv::Mat&, double, const cv::Mat&, double, cv::Mat&, int)>(&Wrappers::gemm_wrapper));

    function("gemm", select_overload<void(const cv::Mat&, const cv::Mat&, double, const cv::Mat&, double, cv::Mat&)>(&Wrappers::gemm_wrapper_1));

    function("getAffineTransform", select_overload<Mat(const cv::Mat&, const cv::Mat&)>(&Wrappers::getAffineTransform_wrapper));

    function("getDefaultNewCameraMatrix", select_overload<Mat(const cv::Mat&, Size, bool)>(&Wrappers::getDefaultNewCameraMatrix_wrapper));

    function("getDefaultNewCameraMatrix", select_overload<Mat(const cv::Mat&, Size)>(&Wrappers::getDefaultNewCameraMatrix_wrapper_1));

    function("getDefaultNewCameraMatrix", select_overload<Mat(const cv::Mat&)>(&Wrappers::getDefaultNewCameraMatrix_wrapper_2));

    function("getLogLevel", select_overload<int()>(&Wrappers::getLogLevel_wrapper));

    function("getOptimalDFTSize", select_overload<int(int)>(&Wrappers::getOptimalDFTSize_wrapper));

    function("getPerspectiveTransform", select_overload<Mat(const cv::Mat&, const cv::Mat&)>(&Wrappers::getPerspectiveTransform_wrapper));

    function("getRotationMatrix2D", select_overload<Mat(Point2f, double, double)>(&Wrappers::getRotationMatrix2D_wrapper));

    function("getStructuringElement", select_overload<Mat(int, Size, Point)>(&Wrappers::getStructuringElement_wrapper));

    function("getStructuringElement", select_overload<Mat(int, Size)>(&Wrappers::getStructuringElement_wrapper_1));

    function("goodFeaturesToTrack", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&, int, bool, double)>(&Wrappers::goodFeaturesToTrack_wrapper));

    function("goodFeaturesToTrack", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&, int, bool)>(&Wrappers::goodFeaturesToTrack_wrapper_1));

    function("goodFeaturesToTrack", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&, int)>(&Wrappers::goodFeaturesToTrack_wrapper_2));

    function("goodFeaturesToTrack", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&)>(&Wrappers::goodFeaturesToTrack_wrapper_3));

    function("goodFeaturesToTrack", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double)>(&Wrappers::goodFeaturesToTrack_wrapper_4));

    function("goodFeaturesToTrack1", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&, int, int, bool, double)>(&Wrappers::goodFeaturesToTrack_wrapper1));

    function("goodFeaturesToTrack1", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&, int, int, bool)>(&Wrappers::goodFeaturesToTrack_wrapper1_1));

    function("goodFeaturesToTrack1", select_overload<void(const cv::Mat&, cv::Mat&, int, double, double, const cv::Mat&, int, int)>(&Wrappers::goodFeaturesToTrack_wrapper1_2));

    function("grabCut", select_overload<void(const cv::Mat&, cv::Mat&, Rect, cv::Mat&, cv::Mat&, int, int)>(&Wrappers::grabCut_wrapper));

    function("grabCut", select_overload<void(const cv::Mat&, cv::Mat&, Rect, cv::Mat&, cv::Mat&, int)>(&Wrappers::grabCut_wrapper_1));

    function("groupRectangles", select_overload<void(std::vector<Rect>&, std::vector<int>&, int, double)>(&Wrappers::groupRectangles_wrapper));

    function("groupRectangles", select_overload<void(std::vector<Rect>&, std::vector<int>&, int)>(&Wrappers::groupRectangles_wrapper_1));

    function("hconcat", select_overload<void(const std::vector<cv::Mat>&, cv::Mat&)>(&Wrappers::hconcat_wrapper));

    function("inRange", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::inRange_wrapper));

    function("initUndistortRectifyMap", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, Size, int, cv::Mat&, cv::Mat&)>(&Wrappers::initUndistortRectifyMap_wrapper));

    function("integral", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::integral_wrapper));

    function("integral", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::integral_wrapper_1));

    function("integral2", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&, int, int)>(&Wrappers::integral2_wrapper));

    function("integral2", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&, int)>(&Wrappers::integral2_wrapper_1));

    function("integral2", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::integral2_wrapper_2));

    function("invert", select_overload<double(const cv::Mat&, cv::Mat&, int)>(&Wrappers::invert_wrapper));

    function("invert", select_overload<double(const cv::Mat&, cv::Mat&)>(&Wrappers::invert_wrapper_1));

    function("isContourConvex", select_overload<bool(const cv::Mat&)>(&Wrappers::isContourConvex_wrapper));

    function("kmeans", select_overload<double(const cv::Mat&, int, cv::Mat&, TermCriteria, int, int, cv::Mat&)>(&Wrappers::kmeans_wrapper));

    function("kmeans", select_overload<double(const cv::Mat&, int, cv::Mat&, TermCriteria, int, int)>(&Wrappers::kmeans_wrapper_1));

    function("line", select_overload<void(cv::Mat&, Point, Point, const Scalar&, int, int, int)>(&Wrappers::line_wrapper));

    function("line", select_overload<void(cv::Mat&, Point, Point, const Scalar&, int, int)>(&Wrappers::line_wrapper_1));

    function("line", select_overload<void(cv::Mat&, Point, Point, const Scalar&, int)>(&Wrappers::line_wrapper_2));

    function("line", select_overload<void(cv::Mat&, Point, Point, const Scalar&)>(&Wrappers::line_wrapper_3));

    function("log", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::log_wrapper));

    function("magnitude", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::magnitude_wrapper));

    function("matchShapes", select_overload<double(const cv::Mat&, const cv::Mat&, int, double)>(&Wrappers::matchShapes_wrapper));

    function("matchTemplate", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, int, const cv::Mat&)>(&Wrappers::matchTemplate_wrapper));

    function("matchTemplate", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, int)>(&Wrappers::matchTemplate_wrapper_1));

    function("max", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::max_wrapper));

    function("mean", select_overload<Scalar(const cv::Mat&, const cv::Mat&)>(&Wrappers::mean_wrapper));

    function("mean", select_overload<Scalar(const cv::Mat&)>(&Wrappers::mean_wrapper_1));

    function("meanStdDev", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::meanStdDev_wrapper));

    function("meanStdDev", select_overload<void(const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::meanStdDev_wrapper_1));

    function("medianBlur", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::medianBlur_wrapper));

    function("merge", select_overload<void(const std::vector<cv::Mat>&, cv::Mat&)>(&Wrappers::merge_wrapper));

    function("min", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::min_wrapper));

    function("minAreaRect", select_overload<RotatedRect(const cv::Mat&)>(&Wrappers::minAreaRect_wrapper));

    function("mixChannels", select_overload<void(const std::vector<cv::Mat>&, std::vector<cv::Mat>&, const emscripten::val&)>(&Wrappers::mixChannels_wrapper));

    function("moments", select_overload<Moments(const cv::Mat&, bool)>(&Wrappers::moments_wrapper));

    function("moments", select_overload<Moments(const cv::Mat&)>(&Wrappers::moments_wrapper_1));

    function("morphologyEx", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point, int, int, const Scalar&)>(&Wrappers::morphologyEx_wrapper));

    function("morphologyEx", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point, int, int)>(&Wrappers::morphologyEx_wrapper_1));

    function("morphologyEx", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point, int)>(&Wrappers::morphologyEx_wrapper_2));

    function("morphologyEx", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, Point)>(&Wrappers::morphologyEx_wrapper_3));

    function("morphologyEx", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&)>(&Wrappers::morphologyEx_wrapper_4));

    function("multiply", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double, int)>(&Wrappers::multiply_wrapper));

    function("multiply", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, double)>(&Wrappers::multiply_wrapper_1));

    function("multiply", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::multiply_wrapper_2));

    function("norm", select_overload<double(const cv::Mat&, int, const cv::Mat&)>(&Wrappers::norm_wrapper));

    function("norm", select_overload<double(const cv::Mat&, int)>(&Wrappers::norm_wrapper_1));

    function("norm", select_overload<double(const cv::Mat&)>(&Wrappers::norm_wrapper_2));

    function("norm1", select_overload<double(const cv::Mat&, const cv::Mat&, int, const cv::Mat&)>(&Wrappers::norm_wrapper1));

    function("norm1", select_overload<double(const cv::Mat&, const cv::Mat&, int)>(&Wrappers::norm_wrapper1_1));

    function("norm1", select_overload<double(const cv::Mat&, const cv::Mat&)>(&Wrappers::norm_wrapper1_2));

    function("normalize", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, int, const cv::Mat&)>(&Wrappers::normalize_wrapper));

    function("normalize", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int, int)>(&Wrappers::normalize_wrapper_1));

    function("normalize", select_overload<void(const cv::Mat&, cv::Mat&, double, double, int)>(&Wrappers::normalize_wrapper_2));

    function("normalize", select_overload<void(const cv::Mat&, cv::Mat&, double, double)>(&Wrappers::normalize_wrapper_3));

    function("normalize", select_overload<void(const cv::Mat&, cv::Mat&, double)>(&Wrappers::normalize_wrapper_4));

    function("normalize", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::normalize_wrapper_5));

    function("perspectiveTransform", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::perspectiveTransform_wrapper));

    function("pointPolygonTest", select_overload<double(const cv::Mat&, Point2f, bool)>(&Wrappers::pointPolygonTest_wrapper));

    function("polarToCart", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool)>(&Wrappers::polarToCart_wrapper));

    function("polarToCart", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::polarToCart_wrapper_1));

    function("pow", select_overload<void(const cv::Mat&, double, cv::Mat&)>(&Wrappers::pow_wrapper));

    function("putText", select_overload<void(cv::Mat&, const std::string&, Point, int, double, Scalar, int, int, bool)>(&Wrappers::putText_wrapper));

    function("putText", select_overload<void(cv::Mat&, const std::string&, Point, int, double, Scalar, int, int)>(&Wrappers::putText_wrapper_1));

    function("putText", select_overload<void(cv::Mat&, const std::string&, Point, int, double, Scalar, int)>(&Wrappers::putText_wrapper_2));

    function("putText", select_overload<void(cv::Mat&, const std::string&, Point, int, double, Scalar)>(&Wrappers::putText_wrapper_3));

    function("pyrDown", select_overload<void(const cv::Mat&, cv::Mat&, const Size&, int)>(&Wrappers::pyrDown_wrapper));

    function("pyrDown", select_overload<void(const cv::Mat&, cv::Mat&, const Size&)>(&Wrappers::pyrDown_wrapper_1));

    function("pyrDown", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::pyrDown_wrapper_2));

    function("pyrUp", select_overload<void(const cv::Mat&, cv::Mat&, const Size&, int)>(&Wrappers::pyrUp_wrapper));

    function("pyrUp", select_overload<void(const cv::Mat&, cv::Mat&, const Size&)>(&Wrappers::pyrUp_wrapper_1));

    function("pyrUp", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::pyrUp_wrapper_2));

    function("randn", select_overload<void(cv::Mat&, const cv::Mat&, const cv::Mat&)>(&Wrappers::randn_wrapper));

    function("randu", select_overload<void(cv::Mat&, const cv::Mat&, const cv::Mat&)>(&Wrappers::randu_wrapper));

    function("rectangle", select_overload<void(cv::Mat&, Point, Point, const Scalar&, int, int, int)>(&Wrappers::rectangle_wrapper));

    function("rectangle", select_overload<void(cv::Mat&, Point, Point, const Scalar&, int, int)>(&Wrappers::rectangle_wrapper_1));

    function("rectangle", select_overload<void(cv::Mat&, Point, Point, const Scalar&, int)>(&Wrappers::rectangle_wrapper_2));

    function("rectangle", select_overload<void(cv::Mat&, Point, Point, const Scalar&)>(&Wrappers::rectangle_wrapper_3));

    function("reduce", select_overload<void(const cv::Mat&, cv::Mat&, int, int, int)>(&Wrappers::reduce_wrapper));

    function("reduce", select_overload<void(const cv::Mat&, cv::Mat&, int, int)>(&Wrappers::reduce_wrapper_1));

    function("remap", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, const cv::Mat&, int, int, const Scalar&)>(&Wrappers::remap_wrapper));

    function("remap", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, const cv::Mat&, int, int)>(&Wrappers::remap_wrapper_1));

    function("remap", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, const cv::Mat&, int)>(&Wrappers::remap_wrapper_2));

    function("repeat", select_overload<void(const cv::Mat&, int, int, cv::Mat&)>(&Wrappers::repeat_wrapper));

    function("resize", select_overload<void(const cv::Mat&, cv::Mat&, Size, double, double, int)>(&Wrappers::resize_wrapper));

    function("resize", select_overload<void(const cv::Mat&, cv::Mat&, Size, double, double)>(&Wrappers::resize_wrapper_1));

    function("resize", select_overload<void(const cv::Mat&, cv::Mat&, Size, double)>(&Wrappers::resize_wrapper_2));

    function("resize", select_overload<void(const cv::Mat&, cv::Mat&, Size)>(&Wrappers::resize_wrapper_3));

    function("rotate", select_overload<void(const cv::Mat&, cv::Mat&, int)>(&Wrappers::rotate_wrapper));

    function("sepFilter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, const cv::Mat&, Point, double, int)>(&Wrappers::sepFilter2D_wrapper));

    function("sepFilter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, const cv::Mat&, Point, double)>(&Wrappers::sepFilter2D_wrapper_1));

    function("sepFilter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, const cv::Mat&, Point)>(&Wrappers::sepFilter2D_wrapper_2));

    function("sepFilter2D", select_overload<void(const cv::Mat&, cv::Mat&, int, const cv::Mat&, const cv::Mat&)>(&Wrappers::sepFilter2D_wrapper_3));

    function("setIdentity", select_overload<void(cv::Mat&, const Scalar&)>(&Wrappers::setIdentity_wrapper));

    function("setIdentity", select_overload<void(cv::Mat&)>(&Wrappers::setIdentity_wrapper_1));

    function("setLogLevel", select_overload<int(int)>(&Wrappers::setLogLevel_wrapper));

    function("setRNGSeed", select_overload<void(int)>(&Wrappers::setRNGSeed_wrapper));

    function("solve", select_overload<bool(const cv::Mat&, const cv::Mat&, cv::Mat&, int)>(&Wrappers::solve_wrapper));

    function("solve", select_overload<bool(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::solve_wrapper_1));

    function("solvePnP", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool, int)>(&Wrappers::solvePnP_wrapper));

    function("solvePnP", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool)>(&Wrappers::solvePnP_wrapper_1));

    function("solvePnP", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::solvePnP_wrapper_2));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool, int, float, double, cv::Mat&, int)>(&Wrappers::solvePnPRansac_wrapper));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool, int, float, double, cv::Mat&)>(&Wrappers::solvePnPRansac_wrapper_1));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool, int, float, double)>(&Wrappers::solvePnPRansac_wrapper_2));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool, int, float)>(&Wrappers::solvePnPRansac_wrapper_3));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool, int)>(&Wrappers::solvePnPRansac_wrapper_4));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, bool)>(&Wrappers::solvePnPRansac_wrapper_5));

    function("solvePnPRansac", select_overload<bool(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::solvePnPRansac_wrapper_6));

    function("solvePnPRefineLM", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&, TermCriteria)>(&Wrappers::solvePnPRefineLM_wrapper));

    function("solvePnPRefineLM", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, cv::Mat&, cv::Mat&)>(&Wrappers::solvePnPRefineLM_wrapper_1));

    function("solvePoly", select_overload<double(const cv::Mat&, cv::Mat&, int)>(&Wrappers::solvePoly_wrapper));

    function("solvePoly", select_overload<double(const cv::Mat&, cv::Mat&)>(&Wrappers::solvePoly_wrapper_1));

    function("split", select_overload<void(const cv::Mat&, std::vector<cv::Mat>&)>(&Wrappers::split_wrapper));

    function("sqrt", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::sqrt_wrapper));

    function("subtract", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&, int)>(&Wrappers::subtract_wrapper));

    function("subtract", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::subtract_wrapper_1));

    function("subtract", select_overload<void(const cv::Mat&, const cv::Mat&, cv::Mat&)>(&Wrappers::subtract_wrapper_2));

    function("threshold", select_overload<double(const cv::Mat&, cv::Mat&, double, double, int)>(&Wrappers::threshold_wrapper));

    function("trace", select_overload<Scalar(const cv::Mat&)>(&Wrappers::trace_wrapper));

    function("transform", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&)>(&Wrappers::transform_wrapper));

    function("transpose", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::transpose_wrapper));

    function("undistort", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&)>(&Wrappers::undistort_wrapper));

    function("undistort", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, const cv::Mat&)>(&Wrappers::undistort_wrapper_1));

    function("vconcat", select_overload<void(const std::vector<cv::Mat>&, cv::Mat&)>(&Wrappers::vconcat_wrapper));

    function("warpAffine", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size, int, int, const Scalar&)>(&Wrappers::warpAffine_wrapper));

    function("warpAffine", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size, int, int)>(&Wrappers::warpAffine_wrapper_1));

    function("warpAffine", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size, int)>(&Wrappers::warpAffine_wrapper_2));

    function("warpAffine", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size)>(&Wrappers::warpAffine_wrapper_3));

    function("warpPerspective", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size, int, int, const Scalar&)>(&Wrappers::warpPerspective_wrapper));

    function("warpPerspective", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size, int, int)>(&Wrappers::warpPerspective_wrapper_1));

    function("warpPerspective", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size, int)>(&Wrappers::warpPerspective_wrapper_2));

    function("warpPerspective", select_overload<void(const cv::Mat&, cv::Mat&, const cv::Mat&, Size)>(&Wrappers::warpPerspective_wrapper_3));

    function("warpPolar", select_overload<void(const cv::Mat&, cv::Mat&, Size, Point2f, double, int)>(&Wrappers::warpPolar_wrapper));

    function("watershed", select_overload<void(const cv::Mat&, cv::Mat&)>(&Wrappers::watershed_wrapper));

    function("blobFromImage", select_overload<Mat(const cv::Mat&, double, const Size&, const Scalar&, bool, bool, int)>(&Wrappers::blobFromImage_wrapper));

    function("blobFromImage", select_overload<Mat(const cv::Mat&, double, const Size&, const Scalar&, bool, bool)>(&Wrappers::blobFromImage_wrapper_1));

    function("blobFromImage", select_overload<Mat(const cv::Mat&, double, const Size&, const Scalar&, bool)>(&Wrappers::blobFromImage_wrapper_2));

    function("blobFromImage", select_overload<Mat(const cv::Mat&, double, const Size&, const Scalar&)>(&Wrappers::blobFromImage_wrapper_3));

    function("blobFromImage", select_overload<Mat(const cv::Mat&, double, const Size&)>(&Wrappers::blobFromImage_wrapper_4));

    function("blobFromImage", select_overload<Mat(const cv::Mat&, double)>(&Wrappers::blobFromImage_wrapper_5));

    function("blobFromImage", select_overload<Mat(const cv::Mat&)>(&Wrappers::blobFromImage_wrapper_6));

    function("readNet", select_overload<Net(const std::string&, const std::string&, const std::string&)>(&Wrappers::readNet_wrapper));

    function("readNet", select_overload<Net(const std::string&, const std::string&)>(&Wrappers::readNet_wrapper_1));

    function("readNet", select_overload<Net(const std::string&)>(&Wrappers::readNet_wrapper_2));

    function("readNet1", select_overload<Net(const std::string&, const emscripten::val&, const emscripten::val&)>(&Wrappers::readNet_wrapper1));

    function("readNet1", select_overload<Net(const std::string&, const emscripten::val&)>(&Wrappers::readNet_wrapper1_1));

    function("readNetFromCaffe", select_overload<Net(const std::string&, const std::string&)>(&Wrappers::readNetFromCaffe_wrapper));

    function("readNetFromCaffe", select_overload<Net(const std::string&)>(&Wrappers::readNetFromCaffe_wrapper_1));

    function("readNetFromCaffe1", select_overload<Net(const emscripten::val&, const emscripten::val&)>(&Wrappers::readNetFromCaffe_wrapper1));

    function("readNetFromCaffe1", select_overload<Net(const emscripten::val&)>(&Wrappers::readNetFromCaffe_wrapper1_1));

    function("readNetFromDarknet", select_overload<Net(const std::string&, const std::string&)>(&Wrappers::readNetFromDarknet_wrapper));

    function("readNetFromDarknet", select_overload<Net(const std::string&)>(&Wrappers::readNetFromDarknet_wrapper_1));

    function("readNetFromDarknet1", select_overload<Net(const emscripten::val&, const emscripten::val&)>(&Wrappers::readNetFromDarknet_wrapper1));

    function("readNetFromDarknet1", select_overload<Net(const emscripten::val&)>(&Wrappers::readNetFromDarknet_wrapper1_1));

    function("readNetFromONNX", select_overload<Net(const std::string&)>(&Wrappers::readNetFromONNX_wrapper));

    function("readNetFromONNX1", select_overload<Net(const emscripten::val&)>(&Wrappers::readNetFromONNX_wrapper1));

    function("readNetFromTensorflow", select_overload<Net(const std::string&, const std::string&)>(&Wrappers::readNetFromTensorflow_wrapper));

    function("readNetFromTensorflow", select_overload<Net(const std::string&)>(&Wrappers::readNetFromTensorflow_wrapper_1));

    function("readNetFromTensorflow1", select_overload<Net(const emscripten::val&, const emscripten::val&)>(&Wrappers::readNetFromTensorflow_wrapper1));

    function("readNetFromTensorflow1", select_overload<Net(const emscripten::val&)>(&Wrappers::readNetFromTensorflow_wrapper1_1));

    function("readNetFromTorch", select_overload<Net(const std::string&, bool, bool)>(&Wrappers::readNetFromTorch_wrapper));

    function("readNetFromTorch", select_overload<Net(const std::string&, bool)>(&Wrappers::readNetFromTorch_wrapper_1));

    function("readNetFromTorch", select_overload<Net(const std::string&)>(&Wrappers::readNetFromTorch_wrapper_2));

    function("initUndistortRectifyMap", select_overload<void(const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&, const Size&, int, cv::Mat&, cv::Mat&)>(&Wrappers::initUndistortRectifyMap_wrapper));

    emscripten::class_<cv::AKAZE ,base<Feature2D>>("AKAZE")
        .constructor(select_overload<Ptr<AKAZE>(int,int,int,float,int,int,int)>(&Wrappers::AKAZE_create_wrapper))
        .constructor(select_overload<Ptr<AKAZE>(int,int,int,float,int,int)>(&Wrappers::AKAZE_create_wrapper_1))
        .constructor(select_overload<Ptr<AKAZE>(int,int,int,float,int)>(&Wrappers::AKAZE_create_wrapper_2))
        .constructor(select_overload<Ptr<AKAZE>(int,int,int,float)>(&Wrappers::AKAZE_create_wrapper_3))
        .constructor(select_overload<Ptr<AKAZE>(int,int,int)>(&Wrappers::AKAZE_create_wrapper_4))
        .constructor(select_overload<Ptr<AKAZE>(int,int)>(&Wrappers::AKAZE_create_wrapper_5))
        .constructor(select_overload<Ptr<AKAZE>(int)>(&Wrappers::AKAZE_create_wrapper_6))
        .constructor(select_overload<Ptr<AKAZE>()>(&Wrappers::AKAZE_create_wrapper_7))
        .function("getDefaultName", select_overload<std::string(cv::AKAZE&)>(&Wrappers::AKAZE_getDefaultName_wrapper))
        .function("getDescriptorChannels", select_overload<int()const>(&cv::AKAZE::getDescriptorChannels), pure_virtual())
        .function("getDescriptorSize", select_overload<int()const>(&cv::AKAZE::getDescriptorSize), pure_virtual())
        .function("getDescriptorType", select_overload<int()const>(&cv::AKAZE::getDescriptorType), pure_virtual())
        .function("getDiffusivity", select_overload<int()const>(&cv::AKAZE::getDiffusivity), pure_virtual())
        .function("getNOctaveLayers", select_overload<int()const>(&cv::AKAZE::getNOctaveLayers), pure_virtual())
        .function("getNOctaves", select_overload<int()const>(&cv::AKAZE::getNOctaves), pure_virtual())
        .function("getThreshold", select_overload<double()const>(&cv::AKAZE::getThreshold), pure_virtual())
        .function("setDescriptorChannels", select_overload<void(cv::AKAZE&,int)>(&Wrappers::AKAZE_setDescriptorChannels_wrapper), pure_virtual())
        .function("setDescriptorSize", select_overload<void(cv::AKAZE&,int)>(&Wrappers::AKAZE_setDescriptorSize_wrapper), pure_virtual())
        .function("setDescriptorType", select_overload<void(cv::AKAZE&,int)>(&Wrappers::AKAZE_setDescriptorType_wrapper), pure_virtual())
        .function("setDiffusivity", select_overload<void(cv::AKAZE&,int)>(&Wrappers::AKAZE_setDiffusivity_wrapper), pure_virtual())
        .function("setNOctaveLayers", select_overload<void(cv::AKAZE&,int)>(&Wrappers::AKAZE_setNOctaveLayers_wrapper), pure_virtual())
        .function("setNOctaves", select_overload<void(cv::AKAZE&,int)>(&Wrappers::AKAZE_setNOctaves_wrapper), pure_virtual())
        .function("setThreshold", select_overload<void(cv::AKAZE&,double)>(&Wrappers::AKAZE_setThreshold_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::AKAZE>>("Ptr<AKAZE>")
;

    emscripten::class_<cv::AgastFeatureDetector ,base<Feature2D>>("AgastFeatureDetector")
        .constructor(select_overload<Ptr<AgastFeatureDetector>(int,bool,int)>(&Wrappers::AgastFeatureDetector_create_wrapper))
        .constructor(select_overload<Ptr<AgastFeatureDetector>(int,bool)>(&Wrappers::AgastFeatureDetector_create_wrapper_1))
        .constructor(select_overload<Ptr<AgastFeatureDetector>(int)>(&Wrappers::AgastFeatureDetector_create_wrapper_2))
        .constructor(select_overload<Ptr<AgastFeatureDetector>()>(&Wrappers::AgastFeatureDetector_create_wrapper_3))
        .function("getDefaultName", select_overload<std::string(cv::AgastFeatureDetector&)>(&Wrappers::AgastFeatureDetector_getDefaultName_wrapper))
        .function("getNonmaxSuppression", select_overload<bool()const>(&cv::AgastFeatureDetector::getNonmaxSuppression), pure_virtual())
        .function("getThreshold", select_overload<int()const>(&cv::AgastFeatureDetector::getThreshold), pure_virtual())
        .function("getType", select_overload<int()const>(&cv::AgastFeatureDetector::getType), pure_virtual())
        .function("setNonmaxSuppression", select_overload<void(cv::AgastFeatureDetector&,bool)>(&Wrappers::AgastFeatureDetector_setNonmaxSuppression_wrapper), pure_virtual())
        .function("setThreshold", select_overload<void(cv::AgastFeatureDetector&,int)>(&Wrappers::AgastFeatureDetector_setThreshold_wrapper), pure_virtual())
        .function("setType", select_overload<void(cv::AgastFeatureDetector&,int)>(&Wrappers::AgastFeatureDetector_setType_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::AgastFeatureDetector>>("Ptr<AgastFeatureDetector>")
;

    emscripten::class_<cv::Algorithm >("Algorithm");

    emscripten::class_<cv::BFMatcher ,base<DescriptorMatcher>>("BFMatcher")
        .constructor(select_overload<Ptr<BFMatcher>(int,bool)>(&Wrappers::BFMatcher_create_wrapper))
        .constructor(select_overload<Ptr<BFMatcher>(int)>(&Wrappers::BFMatcher_create_wrapper_1))
        .constructor(select_overload<Ptr<BFMatcher>()>(&Wrappers::BFMatcher_create_wrapper_2))
        .smart_ptr<Ptr<cv::BFMatcher>>("Ptr<BFMatcher>")
;

    emscripten::class_<cv::BRISK ,base<Feature2D>>("BRISK")
        .constructor(select_overload<Ptr<BRISK>(int,int,float)>(&Wrappers::BRISK_create_wrapper))
        .constructor(select_overload<Ptr<BRISK>(int,int)>(&Wrappers::BRISK_create_wrapper_1))
        .constructor(select_overload<Ptr<BRISK>(int)>(&Wrappers::BRISK_create_wrapper_2))
        .constructor(select_overload<Ptr<BRISK>()>(&Wrappers::BRISK_create_wrapper_3))
        .constructor(select_overload<Ptr<BRISK>(const emscripten::val&,const emscripten::val&,float,float,const emscripten::val&)>(&Wrappers::BRISK_create_wrapper1))
        .constructor(select_overload<Ptr<BRISK>(const emscripten::val&,const emscripten::val&,float,float)>(&Wrappers::BRISK_create_wrapper1_1))
        .constructor(select_overload<Ptr<BRISK>(int,int,const emscripten::val&,const emscripten::val&,float,float,const emscripten::val&)>(&Wrappers::BRISK_create_wrapper2))
        .constructor(select_overload<Ptr<BRISK>(int,int,const emscripten::val&,const emscripten::val&,float,float)>(&Wrappers::BRISK_create_wrapper2_1))
        .function("getDefaultName", select_overload<std::string(cv::BRISK&)>(&Wrappers::BRISK_getDefaultName_wrapper))
        .smart_ptr<Ptr<cv::BRISK>>("Ptr<BRISK>")
;

    emscripten::class_<cv::BackgroundSubtractor ,base<Algorithm>>("BackgroundSubtractor")
        .function("apply", select_overload<void(cv::BackgroundSubtractor&,const cv::Mat&,cv::Mat&,double)>(&Wrappers::BackgroundSubtractor_apply_wrapper), pure_virtual())
        .function("apply", select_overload<void(cv::BackgroundSubtractor&,const cv::Mat&,cv::Mat&)>(&Wrappers::BackgroundSubtractor_apply_wrapper_1), pure_virtual())
        .function("getBackgroundImage", select_overload<void(cv::BackgroundSubtractor&,cv::Mat&)>(&Wrappers::BackgroundSubtractor_getBackgroundImage_wrapper), pure_virtual());

    emscripten::class_<cv::BackgroundSubtractorMOG2 ,base<BackgroundSubtractor>>("BackgroundSubtractorMOG2")
        .function("apply", select_overload<void(cv::BackgroundSubtractorMOG2&,const cv::Mat&,cv::Mat&,double)>(&Wrappers::BackgroundSubtractorMOG2_apply_wrapper), pure_virtual())
        .function("apply", select_overload<void(cv::BackgroundSubtractorMOG2&,const cv::Mat&,cv::Mat&)>(&Wrappers::BackgroundSubtractorMOG2_apply_wrapper_1), pure_virtual())
        .constructor(select_overload<Ptr<BackgroundSubtractorMOG2>(int,double,bool)>(&Wrappers::_createBackgroundSubtractorMOG2_wrapper))
        .constructor(select_overload<Ptr<BackgroundSubtractorMOG2>(int,double)>(&Wrappers::_createBackgroundSubtractorMOG2_wrapper_1))
        .constructor(select_overload<Ptr<BackgroundSubtractorMOG2>(int)>(&Wrappers::_createBackgroundSubtractorMOG2_wrapper_2))
        .constructor(select_overload<Ptr<BackgroundSubtractorMOG2>()>(&Wrappers::_createBackgroundSubtractorMOG2_wrapper_3))
        .smart_ptr<Ptr<cv::BackgroundSubtractorMOG2>>("Ptr<BackgroundSubtractorMOG2>")
;

    emscripten::class_<cv::CLAHE ,base<Algorithm>>("CLAHE")
        .function("apply", select_overload<void(cv::CLAHE&,const cv::Mat&,cv::Mat&)>(&Wrappers::CLAHE_apply_wrapper), pure_virtual())
        .function("collectGarbage", select_overload<void()>(&cv::CLAHE::collectGarbage), pure_virtual())
        .constructor(select_overload<Ptr<CLAHE>(double,Size)>(&Wrappers::_createCLAHE_wrapper))
        .constructor(select_overload<Ptr<CLAHE>(double)>(&Wrappers::_createCLAHE_wrapper_1))
        .constructor(select_overload<Ptr<CLAHE>()>(&Wrappers::_createCLAHE_wrapper_2))
        .function("getClipLimit", select_overload<double()const>(&cv::CLAHE::getClipLimit), pure_virtual())
        .function("getTilesGridSize", select_overload<Size()const>(&cv::CLAHE::getTilesGridSize), pure_virtual())
        .function("setClipLimit", select_overload<void(cv::CLAHE&,double)>(&Wrappers::CLAHE_setClipLimit_wrapper), pure_virtual())
        .function("setTilesGridSize", select_overload<void(cv::CLAHE&,Size)>(&Wrappers::CLAHE_setTilesGridSize_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::CLAHE>>("Ptr<CLAHE>")
;

    emscripten::class_<cv::CascadeClassifier >("CascadeClassifier")
        .constructor<>()
        .constructor<const std::string&>()
        .function("detectMultiScale", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,double,int,int,Size,Size)>(&Wrappers::CascadeClassifier_detectMultiScale_wrapper))
        .function("detectMultiScale", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,double,int,int,Size)>(&Wrappers::CascadeClassifier_detectMultiScale_wrapper_1))
        .function("detectMultiScale", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,double,int,int)>(&Wrappers::CascadeClassifier_detectMultiScale_wrapper_2))
        .function("detectMultiScale", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,double,int)>(&Wrappers::CascadeClassifier_detectMultiScale_wrapper_3))
        .function("detectMultiScale", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,double)>(&Wrappers::CascadeClassifier_detectMultiScale_wrapper_4))
        .function("detectMultiScale", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&)>(&Wrappers::CascadeClassifier_detectMultiScale_wrapper_5))
        .function("detectMultiScale2", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,double,int,int,Size,Size)>(&Wrappers::CascadeClassifier_detectMultiScale2_wrapper))
        .function("detectMultiScale2", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,double,int,int,Size)>(&Wrappers::CascadeClassifier_detectMultiScale2_wrapper_1))
        .function("detectMultiScale2", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,double,int,int)>(&Wrappers::CascadeClassifier_detectMultiScale2_wrapper_2))
        .function("detectMultiScale2", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,double,int)>(&Wrappers::CascadeClassifier_detectMultiScale2_wrapper_3))
        .function("detectMultiScale2", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,double)>(&Wrappers::CascadeClassifier_detectMultiScale2_wrapper_4))
        .function("detectMultiScale2", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&)>(&Wrappers::CascadeClassifier_detectMultiScale2_wrapper_5))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&,double,int,int,Size,Size,bool)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&,double,int,int,Size,Size)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper_1))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&,double,int,int,Size)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper_2))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&,double,int,int)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper_3))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&,double,int)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper_4))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&,double)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper_5))
        .function("detectMultiScale3", select_overload<void(cv::CascadeClassifier&,const cv::Mat&,std::vector<Rect>&,std::vector<int>&,std::vector<double>&)>(&Wrappers::CascadeClassifier_detectMultiScale3_wrapper_6))
        .function("empty", select_overload<bool()const>(&cv::CascadeClassifier::empty))
        .function("load", select_overload<bool(cv::CascadeClassifier&,const std::string&)>(&Wrappers::CascadeClassifier_load_wrapper));

    emscripten::class_<cv::DescriptorMatcher ,base<Algorithm>>("DescriptorMatcher")
        .function("add", select_overload<void(cv::DescriptorMatcher&,const std::vector<cv::Mat>&)>(&Wrappers::DescriptorMatcher_add_wrapper))
        .function("clear", select_overload<void()>(&cv::DescriptorMatcher::clear))
        .constructor(select_overload<Ptr<DescriptorMatcher>(const std::string&)>(&Wrappers::DescriptorMatcher_create_wrapper))
        .function("empty", select_overload<bool()const>(&cv::DescriptorMatcher::empty))
        .function("isMaskSupported", select_overload<bool()const>(&cv::DescriptorMatcher::isMaskSupported), pure_virtual())
        .function("knnMatch", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<std::vector<DMatch>>&,int,const cv::Mat&,bool)>(&Wrappers::DescriptorMatcher_knnMatch_wrapper))
        .function("knnMatch", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<std::vector<DMatch>>&,int,const cv::Mat&)>(&Wrappers::DescriptorMatcher_knnMatch_wrapper_1))
        .function("knnMatch", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<std::vector<DMatch>>&,int)>(&Wrappers::DescriptorMatcher_knnMatch_wrapper_2))
        .function("knnMatch1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<std::vector<DMatch>>&,int,const std::vector<cv::Mat>&,bool)>(&Wrappers::DescriptorMatcher_knnMatch_wrapper1))
        .function("knnMatch1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<std::vector<DMatch>>&,int,const std::vector<cv::Mat>&)>(&Wrappers::DescriptorMatcher_knnMatch_wrapper1_1))
        .function("knnMatch1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<std::vector<DMatch>>&,int)>(&Wrappers::DescriptorMatcher_knnMatch_wrapper1_2))
        .function("match", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<DMatch>&,const cv::Mat&)>(&Wrappers::DescriptorMatcher_match_wrapper))
        .function("match", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<DMatch>&)>(&Wrappers::DescriptorMatcher_match_wrapper_1))
        .function("match1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<DMatch>&,const std::vector<cv::Mat>&)>(&Wrappers::DescriptorMatcher_match_wrapper1))
        .function("match1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<DMatch>&)>(&Wrappers::DescriptorMatcher_match_wrapper1_1))
        .function("radiusMatch", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<std::vector<DMatch>>&,float,const cv::Mat&,bool)>(&Wrappers::DescriptorMatcher_radiusMatch_wrapper))
        .function("radiusMatch", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<std::vector<DMatch>>&,float,const cv::Mat&)>(&Wrappers::DescriptorMatcher_radiusMatch_wrapper_1))
        .function("radiusMatch", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,const cv::Mat&,std::vector<std::vector<DMatch>>&,float)>(&Wrappers::DescriptorMatcher_radiusMatch_wrapper_2))
        .function("radiusMatch1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<std::vector<DMatch>>&,float,const std::vector<cv::Mat>&,bool)>(&Wrappers::DescriptorMatcher_radiusMatch_wrapper1))
        .function("radiusMatch1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<std::vector<DMatch>>&,float,const std::vector<cv::Mat>&)>(&Wrappers::DescriptorMatcher_radiusMatch_wrapper1_1))
        .function("radiusMatch1", select_overload<void(cv::DescriptorMatcher&,const cv::Mat&,std::vector<std::vector<DMatch>>&,float)>(&Wrappers::DescriptorMatcher_radiusMatch_wrapper1_2))
        .function("train", select_overload<void()>(&cv::DescriptorMatcher::train))
        .smart_ptr<Ptr<cv::DescriptorMatcher>>("Ptr<DescriptorMatcher>")
;

    emscripten::class_<cv::FastFeatureDetector ,base<Feature2D>>("FastFeatureDetector")
        .constructor(select_overload<Ptr<FastFeatureDetector>(int,bool,int)>(&Wrappers::FastFeatureDetector_create_wrapper))
        .constructor(select_overload<Ptr<FastFeatureDetector>(int,bool)>(&Wrappers::FastFeatureDetector_create_wrapper_1))
        .constructor(select_overload<Ptr<FastFeatureDetector>(int)>(&Wrappers::FastFeatureDetector_create_wrapper_2))
        .constructor(select_overload<Ptr<FastFeatureDetector>()>(&Wrappers::FastFeatureDetector_create_wrapper_3))
        .function("getDefaultName", select_overload<std::string(cv::FastFeatureDetector&)>(&Wrappers::FastFeatureDetector_getDefaultName_wrapper))
        .function("getNonmaxSuppression", select_overload<bool()const>(&cv::FastFeatureDetector::getNonmaxSuppression), pure_virtual())
        .function("getThreshold", select_overload<int()const>(&cv::FastFeatureDetector::getThreshold), pure_virtual())
        .function("getType", select_overload<int()const>(&cv::FastFeatureDetector::getType), pure_virtual())
        .function("setNonmaxSuppression", select_overload<void(cv::FastFeatureDetector&,bool)>(&Wrappers::FastFeatureDetector_setNonmaxSuppression_wrapper), pure_virtual())
        .function("setThreshold", select_overload<void(cv::FastFeatureDetector&,int)>(&Wrappers::FastFeatureDetector_setThreshold_wrapper), pure_virtual())
        .function("setType", select_overload<void(cv::FastFeatureDetector&,int)>(&Wrappers::FastFeatureDetector_setType_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::FastFeatureDetector>>("Ptr<FastFeatureDetector>")
;

    emscripten::class_<cv::Feature2D ,base<Algorithm>>("Feature2D")
        .function("compute", select_overload<void(cv::Feature2D&,const cv::Mat&,std::vector<KeyPoint>&,cv::Mat&)>(&Wrappers::Feature2D_compute_wrapper))
        .function("compute1", select_overload<void(cv::Feature2D&,const std::vector<cv::Mat>&,std::vector<std::vector<KeyPoint>>&,std::vector<cv::Mat>&)>(&Wrappers::Feature2D_compute_wrapper1))
        .function("defaultNorm", select_overload<int()const>(&cv::Feature2D::defaultNorm))
        .function("descriptorSize", select_overload<int()const>(&cv::Feature2D::descriptorSize))
        .function("descriptorType", select_overload<int()const>(&cv::Feature2D::descriptorType))
        .function("detect", select_overload<void(cv::Feature2D&,const cv::Mat&,std::vector<KeyPoint>&,const cv::Mat&)>(&Wrappers::Feature2D_detect_wrapper))
        .function("detect", select_overload<void(cv::Feature2D&,const cv::Mat&,std::vector<KeyPoint>&)>(&Wrappers::Feature2D_detect_wrapper_1))
        .function("detect1", select_overload<void(cv::Feature2D&,const std::vector<cv::Mat>&,std::vector<std::vector<KeyPoint>>&,const std::vector<cv::Mat>&)>(&Wrappers::Feature2D_detect_wrapper1))
        .function("detect1", select_overload<void(cv::Feature2D&,const std::vector<cv::Mat>&,std::vector<std::vector<KeyPoint>>&)>(&Wrappers::Feature2D_detect_wrapper1_1))
        .function("detectAndCompute", select_overload<void(cv::Feature2D&,const cv::Mat&,const cv::Mat&,std::vector<KeyPoint>&,cv::Mat&,bool)>(&Wrappers::Feature2D_detectAndCompute_wrapper))
        .function("detectAndCompute", select_overload<void(cv::Feature2D&,const cv::Mat&,const cv::Mat&,std::vector<KeyPoint>&,cv::Mat&)>(&Wrappers::Feature2D_detectAndCompute_wrapper_1))
        .function("empty", select_overload<bool()const>(&cv::Feature2D::empty))
        .function("getDefaultName", select_overload<std::string(cv::Feature2D&)>(&Wrappers::Feature2D_getDefaultName_wrapper));

    emscripten::class_<cv::GFTTDetector ,base<Feature2D>>("GFTTDetector")
        .constructor(select_overload<Ptr<GFTTDetector>(int,double,double,int,bool,double)>(&Wrappers::GFTTDetector_create_wrapper))
        .constructor(select_overload<Ptr<GFTTDetector>(int,double,double,int,bool)>(&Wrappers::GFTTDetector_create_wrapper_1))
        .constructor(select_overload<Ptr<GFTTDetector>(int,double,double,int)>(&Wrappers::GFTTDetector_create_wrapper_2))
        .constructor(select_overload<Ptr<GFTTDetector>(int,double,double)>(&Wrappers::GFTTDetector_create_wrapper_3))
        .constructor(select_overload<Ptr<GFTTDetector>(int,double)>(&Wrappers::GFTTDetector_create_wrapper_4))
        .constructor(select_overload<Ptr<GFTTDetector>(int)>(&Wrappers::GFTTDetector_create_wrapper_5))
        .constructor(select_overload<Ptr<GFTTDetector>()>(&Wrappers::GFTTDetector_create_wrapper_6))
        .constructor(select_overload<Ptr<GFTTDetector>(int,double,double,int,int,bool,double)>(&Wrappers::GFTTDetector_create_wrapper1))
        .function("getBlockSize", select_overload<int()const>(&cv::GFTTDetector::getBlockSize), pure_virtual())
        .function("getDefaultName", select_overload<std::string(cv::GFTTDetector&)>(&Wrappers::GFTTDetector_getDefaultName_wrapper))
        .function("getHarrisDetector", select_overload<bool()const>(&cv::GFTTDetector::getHarrisDetector), pure_virtual())
        .function("getK", select_overload<double()const>(&cv::GFTTDetector::getK), pure_virtual())
        .function("getMaxFeatures", select_overload<int()const>(&cv::GFTTDetector::getMaxFeatures), pure_virtual())
        .function("getMinDistance", select_overload<double()const>(&cv::GFTTDetector::getMinDistance), pure_virtual())
        .function("getQualityLevel", select_overload<double()const>(&cv::GFTTDetector::getQualityLevel), pure_virtual())
        .function("setBlockSize", select_overload<void(cv::GFTTDetector&,int)>(&Wrappers::GFTTDetector_setBlockSize_wrapper), pure_virtual())
        .function("setHarrisDetector", select_overload<void(cv::GFTTDetector&,bool)>(&Wrappers::GFTTDetector_setHarrisDetector_wrapper), pure_virtual())
        .function("setK", select_overload<void(cv::GFTTDetector&,double)>(&Wrappers::GFTTDetector_setK_wrapper), pure_virtual())
        .function("setMaxFeatures", select_overload<void(cv::GFTTDetector&,int)>(&Wrappers::GFTTDetector_setMaxFeatures_wrapper), pure_virtual())
        .function("setMinDistance", select_overload<void(cv::GFTTDetector&,double)>(&Wrappers::GFTTDetector_setMinDistance_wrapper), pure_virtual())
        .function("setQualityLevel", select_overload<void(cv::GFTTDetector&,double)>(&Wrappers::GFTTDetector_setQualityLevel_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::GFTTDetector>>("Ptr<GFTTDetector>")
;

    emscripten::class_<cv::HOGDescriptor >("HOGDescriptor")
        .constructor<>()
        .constructor<Size, Size, Size, Size, int, int, double, int, double, bool, int, bool>()
        .constructor<const std::string&>()
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&,double,Size,Size,double,double,bool)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper))
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&,double,Size,Size,double,double)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper_1))
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&,double,Size,Size,double)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper_2))
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&,double,Size,Size)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper_3))
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&,double,Size)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper_4))
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&,double)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper_5))
        .function("detectMultiScale", select_overload<void(cv::HOGDescriptor&,const cv::Mat&,std::vector<Rect>&,std::vector<double>&)>(&Wrappers::HOGDescriptor_detectMultiScale_wrapper_6))
        .class_function("getDaimlerPeopleDetector", select_overload<std::vector<float>()>(&cv::HOGDescriptor::getDaimlerPeopleDetector))
        .class_function("getDefaultPeopleDetector", select_overload<std::vector<float>()>(&cv::HOGDescriptor::getDefaultPeopleDetector))
        .function("load", select_overload<bool(cv::HOGDescriptor&,const std::string&,const std::string&)>(&Wrappers::HOGDescriptor_load_wrapper))
        .function("load", select_overload<bool(cv::HOGDescriptor&,const std::string&)>(&Wrappers::HOGDescriptor_load_wrapper_1))
        .function("setSVMDetector", select_overload<void(cv::HOGDescriptor&,const cv::Mat&)>(&Wrappers::HOGDescriptor_setSVMDetector_wrapper))
        .property("winSize", &cv::HOGDescriptor::winSize)
        .property("blockSize", &cv::HOGDescriptor::blockSize)
        .property("blockStride", &cv::HOGDescriptor::blockStride)
        .property("cellSize", &cv::HOGDescriptor::cellSize)
        .property("nbins", &cv::HOGDescriptor::nbins)
        .property("derivAperture", &cv::HOGDescriptor::derivAperture)
        .property("winSigma", &cv::HOGDescriptor::winSigma)
        .property("histogramNormType", &cv::HOGDescriptor::histogramNormType)
        .property("L2HysThreshold", &cv::HOGDescriptor::L2HysThreshold)
        .property("gammaCorrection", &cv::HOGDescriptor::gammaCorrection)
        .property("svmDetector", &cv::HOGDescriptor::svmDetector)
        .property("nlevels", &cv::HOGDescriptor::nlevels)
        .property("signedGradient", &cv::HOGDescriptor::signedGradient);

    emscripten::class_<cv::KAZE ,base<Feature2D>>("KAZE")
        .constructor(select_overload<Ptr<KAZE>(bool,bool,float,int,int,int)>(&Wrappers::KAZE_create_wrapper))
        .constructor(select_overload<Ptr<KAZE>(bool,bool,float,int,int)>(&Wrappers::KAZE_create_wrapper_1))
        .constructor(select_overload<Ptr<KAZE>(bool,bool,float,int)>(&Wrappers::KAZE_create_wrapper_2))
        .constructor(select_overload<Ptr<KAZE>(bool,bool,float)>(&Wrappers::KAZE_create_wrapper_3))
        .constructor(select_overload<Ptr<KAZE>(bool,bool)>(&Wrappers::KAZE_create_wrapper_4))
        .constructor(select_overload<Ptr<KAZE>(bool)>(&Wrappers::KAZE_create_wrapper_5))
        .constructor(select_overload<Ptr<KAZE>()>(&Wrappers::KAZE_create_wrapper_6))
        .function("getDefaultName", select_overload<std::string(cv::KAZE&)>(&Wrappers::KAZE_getDefaultName_wrapper))
        .function("getDiffusivity", select_overload<int()const>(&cv::KAZE::getDiffusivity), pure_virtual())
        .function("getExtended", select_overload<bool()const>(&cv::KAZE::getExtended), pure_virtual())
        .function("getNOctaveLayers", select_overload<int()const>(&cv::KAZE::getNOctaveLayers), pure_virtual())
        .function("getNOctaves", select_overload<int()const>(&cv::KAZE::getNOctaves), pure_virtual())
        .function("getThreshold", select_overload<double()const>(&cv::KAZE::getThreshold), pure_virtual())
        .function("getUpright", select_overload<bool()const>(&cv::KAZE::getUpright), pure_virtual())
        .function("setDiffusivity", select_overload<void(cv::KAZE&,int)>(&Wrappers::KAZE_setDiffusivity_wrapper), pure_virtual())
        .function("setExtended", select_overload<void(cv::KAZE&,bool)>(&Wrappers::KAZE_setExtended_wrapper), pure_virtual())
        .function("setNOctaveLayers", select_overload<void(cv::KAZE&,int)>(&Wrappers::KAZE_setNOctaveLayers_wrapper), pure_virtual())
        .function("setNOctaves", select_overload<void(cv::KAZE&,int)>(&Wrappers::KAZE_setNOctaves_wrapper), pure_virtual())
        .function("setThreshold", select_overload<void(cv::KAZE&,double)>(&Wrappers::KAZE_setThreshold_wrapper), pure_virtual())
        .function("setUpright", select_overload<void(cv::KAZE&,bool)>(&Wrappers::KAZE_setUpright_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::KAZE>>("Ptr<KAZE>")
;

    emscripten::class_<cv::MSER ,base<Feature2D>>("MSER")
        .constructor(select_overload<Ptr<MSER>(int,int,int,double,double,int,double,double,int)>(&Wrappers::MSER_create_wrapper))
        .constructor(select_overload<Ptr<MSER>(int,int,int,double,double,int,double,double)>(&Wrappers::MSER_create_wrapper_1))
        .constructor(select_overload<Ptr<MSER>(int,int,int,double,double,int,double)>(&Wrappers::MSER_create_wrapper_2))
        .constructor(select_overload<Ptr<MSER>(int,int,int,double,double,int)>(&Wrappers::MSER_create_wrapper_3))
        .constructor(select_overload<Ptr<MSER>(int,int,int,double,double)>(&Wrappers::MSER_create_wrapper_4))
        .constructor(select_overload<Ptr<MSER>(int,int,int,double)>(&Wrappers::MSER_create_wrapper_5))
        .constructor(select_overload<Ptr<MSER>(int,int,int)>(&Wrappers::MSER_create_wrapper_6))
        .constructor(select_overload<Ptr<MSER>(int,int)>(&Wrappers::MSER_create_wrapper_7))
        .constructor(select_overload<Ptr<MSER>(int)>(&Wrappers::MSER_create_wrapper_8))
        .constructor(select_overload<Ptr<MSER>()>(&Wrappers::MSER_create_wrapper_9))
        .function("detectRegions", select_overload<void(cv::MSER&,const cv::Mat&,std::vector<std::vector<Point>>&,std::vector<Rect>&)>(&Wrappers::MSER_detectRegions_wrapper), pure_virtual())
        .function("getDefaultName", select_overload<std::string(cv::MSER&)>(&Wrappers::MSER_getDefaultName_wrapper))
        .function("getDelta", select_overload<int()const>(&cv::MSER::getDelta), pure_virtual())
        .function("getMaxArea", select_overload<int()const>(&cv::MSER::getMaxArea), pure_virtual())
        .function("getMinArea", select_overload<int()const>(&cv::MSER::getMinArea), pure_virtual())
        .function("getPass2Only", select_overload<bool()const>(&cv::MSER::getPass2Only), pure_virtual())
        .function("setDelta", select_overload<void(cv::MSER&,int)>(&Wrappers::MSER_setDelta_wrapper), pure_virtual())
        .function("setMaxArea", select_overload<void(cv::MSER&,int)>(&Wrappers::MSER_setMaxArea_wrapper), pure_virtual())
        .function("setMinArea", select_overload<void(cv::MSER&,int)>(&Wrappers::MSER_setMinArea_wrapper), pure_virtual())
        .function("setPass2Only", select_overload<void(cv::MSER&,bool)>(&Wrappers::MSER_setPass2Only_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::MSER>>("Ptr<MSER>")
;

    emscripten::class_<cv::ORB ,base<Feature2D>>("ORB")
        .constructor(select_overload<Ptr<ORB>(int,float,int,int,int,int,int,int,int)>(&Wrappers::ORB_create_wrapper))
        .constructor(select_overload<Ptr<ORB>(int,float,int,int,int,int,int,int)>(&Wrappers::ORB_create_wrapper_1))
        .constructor(select_overload<Ptr<ORB>(int,float,int,int,int,int,int)>(&Wrappers::ORB_create_wrapper_2))
        .constructor(select_overload<Ptr<ORB>(int,float,int,int,int,int)>(&Wrappers::ORB_create_wrapper_3))
        .constructor(select_overload<Ptr<ORB>(int,float,int,int,int)>(&Wrappers::ORB_create_wrapper_4))
        .constructor(select_overload<Ptr<ORB>(int,float,int,int)>(&Wrappers::ORB_create_wrapper_5))
        .constructor(select_overload<Ptr<ORB>(int,float,int)>(&Wrappers::ORB_create_wrapper_6))
        .constructor(select_overload<Ptr<ORB>(int,float)>(&Wrappers::ORB_create_wrapper_7))
        .constructor(select_overload<Ptr<ORB>(int)>(&Wrappers::ORB_create_wrapper_8))
        .constructor(select_overload<Ptr<ORB>()>(&Wrappers::ORB_create_wrapper_9))
        .function("getDefaultName", select_overload<std::string(cv::ORB&)>(&Wrappers::ORB_getDefaultName_wrapper))
        .function("getFastThreshold", select_overload<int()const>(&cv::ORB::getFastThreshold), pure_virtual())
        .function("setEdgeThreshold", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setEdgeThreshold_wrapper), pure_virtual())
        .function("setFirstLevel", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setFirstLevel_wrapper), pure_virtual())
        .function("setMaxFeatures", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setMaxFeatures_wrapper), pure_virtual())
        .function("setNLevels", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setNLevels_wrapper), pure_virtual())
        .function("setPatchSize", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setPatchSize_wrapper), pure_virtual())
        .function("setScaleFactor", select_overload<void(cv::ORB&,double)>(&Wrappers::ORB_setScaleFactor_wrapper), pure_virtual())
        .function("setScoreType", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setScoreType_wrapper), pure_virtual())
        .function("setWTA_K", select_overload<void(cv::ORB&,int)>(&Wrappers::ORB_setWTA_K_wrapper), pure_virtual())
        .smart_ptr<Ptr<cv::ORB>>("Ptr<ORB>")
;

    emscripten::class_<cv::QRCodeDetector >("QRCodeDetector")
        .constructor<>()
        .function("decode", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&,const cv::Mat&,cv::Mat&)>(&Wrappers::QRCodeDetector_decode_wrapper))
        .function("decode", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&,const cv::Mat&)>(&Wrappers::QRCodeDetector_decode_wrapper_1))
        .function("decodeCurved", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&,const cv::Mat&,cv::Mat&)>(&Wrappers::QRCodeDetector_decodeCurved_wrapper))
        .function("decodeCurved", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&,const cv::Mat&)>(&Wrappers::QRCodeDetector_decodeCurved_wrapper_1))
        .function("detect", select_overload<bool(cv::QRCodeDetector&,const cv::Mat&,cv::Mat&)>(&Wrappers::QRCodeDetector_detect_wrapper))
        .function("detectAndDecode", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&,cv::Mat&,cv::Mat&)>(&Wrappers::QRCodeDetector_detectAndDecode_wrapper))
        .function("detectAndDecode", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&,cv::Mat&)>(&Wrappers::QRCodeDetector_detectAndDecode_wrapper_1))
        .function("detectAndDecode", select_overload<std::string(cv::QRCodeDetector&,const cv::Mat&)>(&Wrappers::QRCodeDetector_detectAndDecode_wrapper_2))
        .function("detectMulti", select_overload<bool(cv::QRCodeDetector&,const cv::Mat&,cv::Mat&)>(&Wrappers::QRCodeDetector_detectMulti_wrapper))
        .function("setEpsX", select_overload<void(cv::QRCodeDetector&,double)>(&Wrappers::QRCodeDetector_setEpsX_wrapper))
        .function("setEpsY", select_overload<void(cv::QRCodeDetector&,double)>(&Wrappers::QRCodeDetector_setEpsY_wrapper));

    emscripten::class_<cv::dnn::Net >("dnn_Net")
        .function("forward", select_overload<Mat(cv::dnn::Net&,const std::string&)>(&Wrappers::dnn_Net_forward_wrapper))
        .function("forward", select_overload<Mat(cv::dnn::Net&)>(&Wrappers::dnn_Net_forward_wrapper_1))
        .function("forward1", select_overload<void(cv::dnn::Net&,std::vector<cv::Mat>&,const std::string&)>(&Wrappers::dnn_Net_forward_wrapper1))
        .function("forward1", select_overload<void(cv::dnn::Net&,std::vector<cv::Mat>&)>(&Wrappers::dnn_Net_forward_wrapper1_1))
        .function("forward2", select_overload<void(cv::dnn::Net&,std::vector<cv::Mat>&,const emscripten::val&)>(&Wrappers::dnn_Net_forward_wrapper2))
        .function("setInput", select_overload<void(cv::dnn::Net&,const cv::Mat&,const std::string&,double,const Scalar&)>(&Wrappers::dnn_Net_setInput_wrapper))
        .function("setInput", select_overload<void(cv::dnn::Net&,const cv::Mat&,const std::string&,double)>(&Wrappers::dnn_Net_setInput_wrapper_1))
        .function("setInput", select_overload<void(cv::dnn::Net&,const cv::Mat&,const std::string&)>(&Wrappers::dnn_Net_setInput_wrapper_2))
        .function("setInput", select_overload<void(cv::dnn::Net&,const cv::Mat&)>(&Wrappers::dnn_Net_setInput_wrapper_3));

    constant("ACCESS_FAST", +cv::ACCESS_FAST);

    constant("ACCESS_MASK", +cv::ACCESS_MASK);

    constant("ACCESS_READ", +cv::ACCESS_READ);

    constant("ACCESS_RW", +cv::ACCESS_RW);

    constant("ACCESS_WRITE", +cv::ACCESS_WRITE);

    constant("ADAPTIVE_THRESH_GAUSSIAN_C", +cv::ADAPTIVE_THRESH_GAUSSIAN_C);

    constant("ADAPTIVE_THRESH_MEAN_C", +cv::ADAPTIVE_THRESH_MEAN_C);

    constant("AKAZE_DESCRIPTOR_KAZE", +cv::AKAZE::DESCRIPTOR_KAZE);

    constant("AKAZE_DESCRIPTOR_KAZE_UPRIGHT", +cv::AKAZE::DESCRIPTOR_KAZE_UPRIGHT);

    constant("AKAZE_DESCRIPTOR_MLDB", +cv::AKAZE::DESCRIPTOR_MLDB);

    constant("AKAZE_DESCRIPTOR_MLDB_UPRIGHT", +cv::AKAZE::DESCRIPTOR_MLDB_UPRIGHT);

    constant("AgastFeatureDetector_AGAST_5_8", +cv::AgastFeatureDetector::AGAST_5_8);

    constant("AgastFeatureDetector_AGAST_7_12d", +cv::AgastFeatureDetector::AGAST_7_12d);

    constant("AgastFeatureDetector_AGAST_7_12s", +cv::AgastFeatureDetector::AGAST_7_12s);

    constant("AgastFeatureDetector_NONMAX_SUPPRESSION", +cv::AgastFeatureDetector::NONMAX_SUPPRESSION);

    constant("AgastFeatureDetector_OAST_9_16", +cv::AgastFeatureDetector::OAST_9_16);

    constant("AgastFeatureDetector_THRESHOLD", +cv::AgastFeatureDetector::THRESHOLD);

    constant("BORDER_CONSTANT", +cv::BORDER_CONSTANT);

    constant("BORDER_DEFAULT", +cv::BORDER_DEFAULT);

    constant("BORDER_ISOLATED", +cv::BORDER_ISOLATED);

    constant("BORDER_REFLECT", +cv::BORDER_REFLECT);

    constant("BORDER_REFLECT101", +cv::BORDER_REFLECT101);

    constant("BORDER_REFLECT_101", +cv::BORDER_REFLECT_101);

    constant("BORDER_REPLICATE", +cv::BORDER_REPLICATE);

    constant("BORDER_TRANSPARENT", +cv::BORDER_TRANSPARENT);

    constant("BORDER_WRAP", +cv::BORDER_WRAP);

    constant("CALIB_CB_ADAPTIVE_THRESH", +cv::CALIB_CB_ADAPTIVE_THRESH);

    constant("CALIB_CB_ASYMMETRIC_GRID", +cv::CALIB_CB_ASYMMETRIC_GRID);

    constant("CALIB_CB_CLUSTERING", +cv::CALIB_CB_CLUSTERING);

    constant("CALIB_CB_FAST_CHECK", +cv::CALIB_CB_FAST_CHECK);

    constant("CALIB_CB_FILTER_QUADS", +cv::CALIB_CB_FILTER_QUADS);

    constant("CALIB_CB_NORMALIZE_IMAGE", +cv::CALIB_CB_NORMALIZE_IMAGE);

    constant("CALIB_CB_SYMMETRIC_GRID", +cv::CALIB_CB_SYMMETRIC_GRID);

    constant("CALIB_FIX_ASPECT_RATIO", +cv::CALIB_FIX_ASPECT_RATIO);

    constant("CALIB_FIX_FOCAL_LENGTH", +cv::CALIB_FIX_FOCAL_LENGTH);

    constant("CALIB_FIX_INTRINSIC", +cv::CALIB_FIX_INTRINSIC);

    constant("CALIB_FIX_K1", +cv::CALIB_FIX_K1);

    constant("CALIB_FIX_K2", +cv::CALIB_FIX_K2);

    constant("CALIB_FIX_K3", +cv::CALIB_FIX_K3);

    constant("CALIB_FIX_K4", +cv::CALIB_FIX_K4);

    constant("CALIB_FIX_K5", +cv::CALIB_FIX_K5);

    constant("CALIB_FIX_K6", +cv::CALIB_FIX_K6);

    constant("CALIB_FIX_PRINCIPAL_POINT", +cv::CALIB_FIX_PRINCIPAL_POINT);

    constant("CALIB_FIX_S1_S2_S3_S4", +cv::CALIB_FIX_S1_S2_S3_S4);

    constant("CALIB_FIX_TANGENT_DIST", +cv::CALIB_FIX_TANGENT_DIST);

    constant("CALIB_FIX_TAUX_TAUY", +cv::CALIB_FIX_TAUX_TAUY);

    constant("CALIB_HAND_EYE_ANDREFF", +cv::CALIB_HAND_EYE_ANDREFF);

    constant("CALIB_HAND_EYE_DANIILIDIS", +cv::CALIB_HAND_EYE_DANIILIDIS);

    constant("CALIB_HAND_EYE_HORAUD", +cv::CALIB_HAND_EYE_HORAUD);

    constant("CALIB_HAND_EYE_PARK", +cv::CALIB_HAND_EYE_PARK);

    constant("CALIB_HAND_EYE_TSAI", +cv::CALIB_HAND_EYE_TSAI);

    constant("CALIB_RATIONAL_MODEL", +cv::CALIB_RATIONAL_MODEL);

    constant("CALIB_SAME_FOCAL_LENGTH", +cv::CALIB_SAME_FOCAL_LENGTH);

    constant("CALIB_THIN_PRISM_MODEL", +cv::CALIB_THIN_PRISM_MODEL);

    constant("CALIB_TILTED_MODEL", +cv::CALIB_TILTED_MODEL);

    constant("CALIB_USE_EXTRINSIC_GUESS", +cv::CALIB_USE_EXTRINSIC_GUESS);

    constant("CALIB_USE_INTRINSIC_GUESS", +cv::CALIB_USE_INTRINSIC_GUESS);

    constant("CALIB_USE_LU", +cv::CALIB_USE_LU);

    constant("CALIB_USE_QR", +cv::CALIB_USE_QR);

    constant("CALIB_ZERO_DISPARITY", +cv::CALIB_ZERO_DISPARITY);

    constant("CALIB_ZERO_TANGENT_DIST", +cv::CALIB_ZERO_TANGENT_DIST);

    constant("CASCADE_DO_CANNY_PRUNING", +cv::CASCADE_DO_CANNY_PRUNING);

    constant("CASCADE_DO_ROUGH_SEARCH", +cv::CASCADE_DO_ROUGH_SEARCH);

    constant("CASCADE_FIND_BIGGEST_OBJECT", +cv::CASCADE_FIND_BIGGEST_OBJECT);

    constant("CASCADE_SCALE_IMAGE", +cv::CASCADE_SCALE_IMAGE);

    constant("CCL_BBDT", +cv::CCL_BBDT);

    constant("CCL_BOLELLI", +cv::CCL_BOLELLI);

    constant("CCL_DEFAULT", +cv::CCL_DEFAULT);

    constant("CCL_GRANA", +cv::CCL_GRANA);

    constant("CCL_SAUF", +cv::CCL_SAUF);

    constant("CCL_SPAGHETTI", +cv::CCL_SPAGHETTI);

    constant("CCL_WU", +cv::CCL_WU);

    constant("CC_STAT_AREA", +cv::CC_STAT_AREA);

    constant("CC_STAT_HEIGHT", +cv::CC_STAT_HEIGHT);

    constant("CC_STAT_LEFT", +cv::CC_STAT_LEFT);

    constant("CC_STAT_MAX", +cv::CC_STAT_MAX);

    constant("CC_STAT_TOP", +cv::CC_STAT_TOP);

    constant("CC_STAT_WIDTH", +cv::CC_STAT_WIDTH);

    constant("CHAIN_APPROX_NONE", +cv::CHAIN_APPROX_NONE);

    constant("CHAIN_APPROX_SIMPLE", +cv::CHAIN_APPROX_SIMPLE);

    constant("CHAIN_APPROX_TC89_KCOS", +cv::CHAIN_APPROX_TC89_KCOS);

    constant("CHAIN_APPROX_TC89_L1", +cv::CHAIN_APPROX_TC89_L1);

    constant("CMP_EQ", +cv::CMP_EQ);

    constant("CMP_GE", +cv::CMP_GE);

    constant("CMP_GT", +cv::CMP_GT);

    constant("CMP_LE", +cv::CMP_LE);

    constant("CMP_LT", +cv::CMP_LT);

    constant("CMP_NE", +cv::CMP_NE);

    constant("COLORMAP_AUTUMN", +cv::COLORMAP_AUTUMN);

    constant("COLORMAP_BONE", +cv::COLORMAP_BONE);

    constant("COLORMAP_CIVIDIS", +cv::COLORMAP_CIVIDIS);

    constant("COLORMAP_COOL", +cv::COLORMAP_COOL);

    constant("COLORMAP_DEEPGREEN", +cv::COLORMAP_DEEPGREEN);

    constant("COLORMAP_HOT", +cv::COLORMAP_HOT);

    constant("COLORMAP_HSV", +cv::COLORMAP_HSV);

    constant("COLORMAP_INFERNO", +cv::COLORMAP_INFERNO);

    constant("COLORMAP_JET", +cv::COLORMAP_JET);

    constant("COLORMAP_MAGMA", +cv::COLORMAP_MAGMA);

    constant("COLORMAP_OCEAN", +cv::COLORMAP_OCEAN);

    constant("COLORMAP_PARULA", +cv::COLORMAP_PARULA);

    constant("COLORMAP_PINK", +cv::COLORMAP_PINK);

    constant("COLORMAP_PLASMA", +cv::COLORMAP_PLASMA);

    constant("COLORMAP_RAINBOW", +cv::COLORMAP_RAINBOW);

    constant("COLORMAP_SPRING", +cv::COLORMAP_SPRING);

    constant("COLORMAP_SUMMER", +cv::COLORMAP_SUMMER);

    constant("COLORMAP_TURBO", +cv::COLORMAP_TURBO);

    constant("COLORMAP_TWILIGHT", +cv::COLORMAP_TWILIGHT);

    constant("COLORMAP_TWILIGHT_SHIFTED", +cv::COLORMAP_TWILIGHT_SHIFTED);

    constant("COLORMAP_VIRIDIS", +cv::COLORMAP_VIRIDIS);

    constant("COLORMAP_WINTER", +cv::COLORMAP_WINTER);

    constant("COLOR_BGR2BGR555", +cv::COLOR_BGR2BGR555);

    constant("COLOR_BGR2BGR565", +cv::COLOR_BGR2BGR565);

    constant("COLOR_BGR2BGRA", +cv::COLOR_BGR2BGRA);

    constant("COLOR_BGR2GRAY", +cv::COLOR_BGR2GRAY);

    constant("COLOR_BGR2HLS", +cv::COLOR_BGR2HLS);

    constant("COLOR_BGR2HLS_FULL", +cv::COLOR_BGR2HLS_FULL);

    constant("COLOR_BGR2HSV", +cv::COLOR_BGR2HSV);

    constant("COLOR_BGR2HSV_FULL", +cv::COLOR_BGR2HSV_FULL);

    constant("COLOR_BGR2Lab", +cv::COLOR_BGR2Lab);

    constant("COLOR_BGR2Luv", +cv::COLOR_BGR2Luv);

    constant("COLOR_BGR2RGB", +cv::COLOR_BGR2RGB);

    constant("COLOR_BGR2RGBA", +cv::COLOR_BGR2RGBA);

    constant("COLOR_BGR2XYZ", +cv::COLOR_BGR2XYZ);

    constant("COLOR_BGR2YCrCb", +cv::COLOR_BGR2YCrCb);

    constant("COLOR_BGR2YUV", +cv::COLOR_BGR2YUV);

    constant("COLOR_BGR2YUV_I420", +cv::COLOR_BGR2YUV_I420);

    constant("COLOR_BGR2YUV_IYUV", +cv::COLOR_BGR2YUV_IYUV);

    constant("COLOR_BGR2YUV_YV12", +cv::COLOR_BGR2YUV_YV12);

    constant("COLOR_BGR5552BGR", +cv::COLOR_BGR5552BGR);

    constant("COLOR_BGR5552BGRA", +cv::COLOR_BGR5552BGRA);

    constant("COLOR_BGR5552GRAY", +cv::COLOR_BGR5552GRAY);

    constant("COLOR_BGR5552RGB", +cv::COLOR_BGR5552RGB);

    constant("COLOR_BGR5552RGBA", +cv::COLOR_BGR5552RGBA);

    constant("COLOR_BGR5652BGR", +cv::COLOR_BGR5652BGR);

    constant("COLOR_BGR5652BGRA", +cv::COLOR_BGR5652BGRA);

    constant("COLOR_BGR5652GRAY", +cv::COLOR_BGR5652GRAY);

    constant("COLOR_BGR5652RGB", +cv::COLOR_BGR5652RGB);

    constant("COLOR_BGR5652RGBA", +cv::COLOR_BGR5652RGBA);

    constant("COLOR_BGRA2BGR", +cv::COLOR_BGRA2BGR);

    constant("COLOR_BGRA2BGR555", +cv::COLOR_BGRA2BGR555);

    constant("COLOR_BGRA2BGR565", +cv::COLOR_BGRA2BGR565);

    constant("COLOR_BGRA2GRAY", +cv::COLOR_BGRA2GRAY);

    constant("COLOR_BGRA2RGB", +cv::COLOR_BGRA2RGB);

    constant("COLOR_BGRA2RGBA", +cv::COLOR_BGRA2RGBA);

    constant("COLOR_BGRA2YUV_I420", +cv::COLOR_BGRA2YUV_I420);

    constant("COLOR_BGRA2YUV_IYUV", +cv::COLOR_BGRA2YUV_IYUV);

    constant("COLOR_BGRA2YUV_YV12", +cv::COLOR_BGRA2YUV_YV12);

    constant("COLOR_BayerBG2BGR", +cv::COLOR_BayerBG2BGR);

    constant("COLOR_BayerBG2BGRA", +cv::COLOR_BayerBG2BGRA);

    constant("COLOR_BayerBG2BGR_EA", +cv::COLOR_BayerBG2BGR_EA);

    constant("COLOR_BayerBG2BGR_VNG", +cv::COLOR_BayerBG2BGR_VNG);

    constant("COLOR_BayerBG2GRAY", +cv::COLOR_BayerBG2GRAY);

    constant("COLOR_BayerBG2RGB", +cv::COLOR_BayerBG2RGB);

    constant("COLOR_BayerBG2RGBA", +cv::COLOR_BayerBG2RGBA);

    constant("COLOR_BayerBG2RGB_EA", +cv::COLOR_BayerBG2RGB_EA);

    constant("COLOR_BayerBG2RGB_VNG", +cv::COLOR_BayerBG2RGB_VNG);

    constant("COLOR_BayerGB2BGR", +cv::COLOR_BayerGB2BGR);

    constant("COLOR_BayerGB2BGRA", +cv::COLOR_BayerGB2BGRA);

    constant("COLOR_BayerGB2BGR_EA", +cv::COLOR_BayerGB2BGR_EA);

    constant("COLOR_BayerGB2BGR_VNG", +cv::COLOR_BayerGB2BGR_VNG);

    constant("COLOR_BayerGB2GRAY", +cv::COLOR_BayerGB2GRAY);

    constant("COLOR_BayerGB2RGB", +cv::COLOR_BayerGB2RGB);

    constant("COLOR_BayerGB2RGBA", +cv::COLOR_BayerGB2RGBA);

    constant("COLOR_BayerGB2RGB_EA", +cv::COLOR_BayerGB2RGB_EA);

    constant("COLOR_BayerGB2RGB_VNG", +cv::COLOR_BayerGB2RGB_VNG);

    constant("COLOR_BayerGR2BGR", +cv::COLOR_BayerGR2BGR);

    constant("COLOR_BayerGR2BGRA", +cv::COLOR_BayerGR2BGRA);

    constant("COLOR_BayerGR2BGR_EA", +cv::COLOR_BayerGR2BGR_EA);

    constant("COLOR_BayerGR2BGR_VNG", +cv::COLOR_BayerGR2BGR_VNG);

    constant("COLOR_BayerGR2GRAY", +cv::COLOR_BayerGR2GRAY);

    constant("COLOR_BayerGR2RGB", +cv::COLOR_BayerGR2RGB);

    constant("COLOR_BayerGR2RGBA", +cv::COLOR_BayerGR2RGBA);

    constant("COLOR_BayerGR2RGB_EA", +cv::COLOR_BayerGR2RGB_EA);

    constant("COLOR_BayerGR2RGB_VNG", +cv::COLOR_BayerGR2RGB_VNG);

    constant("COLOR_BayerRG2BGR", +cv::COLOR_BayerRG2BGR);

    constant("COLOR_BayerRG2BGRA", +cv::COLOR_BayerRG2BGRA);

    constant("COLOR_BayerRG2BGR_EA", +cv::COLOR_BayerRG2BGR_EA);

    constant("COLOR_BayerRG2BGR_VNG", +cv::COLOR_BayerRG2BGR_VNG);

    constant("COLOR_BayerRG2GRAY", +cv::COLOR_BayerRG2GRAY);

    constant("COLOR_BayerRG2RGB", +cv::COLOR_BayerRG2RGB);

    constant("COLOR_BayerRG2RGBA", +cv::COLOR_BayerRG2RGBA);

    constant("COLOR_BayerRG2RGB_EA", +cv::COLOR_BayerRG2RGB_EA);

    constant("COLOR_BayerRG2RGB_VNG", +cv::COLOR_BayerRG2RGB_VNG);

    constant("COLOR_COLORCVT_MAX", +cv::COLOR_COLORCVT_MAX);

    constant("COLOR_GRAY2BGR", +cv::COLOR_GRAY2BGR);

    constant("COLOR_GRAY2BGR555", +cv::COLOR_GRAY2BGR555);

    constant("COLOR_GRAY2BGR565", +cv::COLOR_GRAY2BGR565);

    constant("COLOR_GRAY2BGRA", +cv::COLOR_GRAY2BGRA);

    constant("COLOR_GRAY2RGB", +cv::COLOR_GRAY2RGB);

    constant("COLOR_GRAY2RGBA", +cv::COLOR_GRAY2RGBA);

    constant("COLOR_HLS2BGR", +cv::COLOR_HLS2BGR);

    constant("COLOR_HLS2BGR_FULL", +cv::COLOR_HLS2BGR_FULL);

    constant("COLOR_HLS2RGB", +cv::COLOR_HLS2RGB);

    constant("COLOR_HLS2RGB_FULL", +cv::COLOR_HLS2RGB_FULL);

    constant("COLOR_HSV2BGR", +cv::COLOR_HSV2BGR);

    constant("COLOR_HSV2BGR_FULL", +cv::COLOR_HSV2BGR_FULL);

    constant("COLOR_HSV2RGB", +cv::COLOR_HSV2RGB);

    constant("COLOR_HSV2RGB_FULL", +cv::COLOR_HSV2RGB_FULL);

    constant("COLOR_LBGR2Lab", +cv::COLOR_LBGR2Lab);

    constant("COLOR_LBGR2Luv", +cv::COLOR_LBGR2Luv);

    constant("COLOR_LRGB2Lab", +cv::COLOR_LRGB2Lab);

    constant("COLOR_LRGB2Luv", +cv::COLOR_LRGB2Luv);

    constant("COLOR_Lab2BGR", +cv::COLOR_Lab2BGR);

    constant("COLOR_Lab2LBGR", +cv::COLOR_Lab2LBGR);

    constant("COLOR_Lab2LRGB", +cv::COLOR_Lab2LRGB);

    constant("COLOR_Lab2RGB", +cv::COLOR_Lab2RGB);

    constant("COLOR_Luv2BGR", +cv::COLOR_Luv2BGR);

    constant("COLOR_Luv2LBGR", +cv::COLOR_Luv2LBGR);

    constant("COLOR_Luv2LRGB", +cv::COLOR_Luv2LRGB);

    constant("COLOR_Luv2RGB", +cv::COLOR_Luv2RGB);

    constant("COLOR_RGB2BGR", +cv::COLOR_RGB2BGR);

    constant("COLOR_RGB2BGR555", +cv::COLOR_RGB2BGR555);

    constant("COLOR_RGB2BGR565", +cv::COLOR_RGB2BGR565);

    constant("COLOR_RGB2BGRA", +cv::COLOR_RGB2BGRA);

    constant("COLOR_RGB2GRAY", +cv::COLOR_RGB2GRAY);

    constant("COLOR_RGB2HLS", +cv::COLOR_RGB2HLS);

    constant("COLOR_RGB2HLS_FULL", +cv::COLOR_RGB2HLS_FULL);

    constant("COLOR_RGB2HSV", +cv::COLOR_RGB2HSV);

    constant("COLOR_RGB2HSV_FULL", +cv::COLOR_RGB2HSV_FULL);

    constant("COLOR_RGB2Lab", +cv::COLOR_RGB2Lab);

    constant("COLOR_RGB2Luv", +cv::COLOR_RGB2Luv);

    constant("COLOR_RGB2RGBA", +cv::COLOR_RGB2RGBA);

    constant("COLOR_RGB2XYZ", +cv::COLOR_RGB2XYZ);

    constant("COLOR_RGB2YCrCb", +cv::COLOR_RGB2YCrCb);

    constant("COLOR_RGB2YUV", +cv::COLOR_RGB2YUV);

    constant("COLOR_RGB2YUV_I420", +cv::COLOR_RGB2YUV_I420);

    constant("COLOR_RGB2YUV_IYUV", +cv::COLOR_RGB2YUV_IYUV);

    constant("COLOR_RGB2YUV_YV12", +cv::COLOR_RGB2YUV_YV12);

    constant("COLOR_RGBA2BGR", +cv::COLOR_RGBA2BGR);

    constant("COLOR_RGBA2BGR555", +cv::COLOR_RGBA2BGR555);

    constant("COLOR_RGBA2BGR565", +cv::COLOR_RGBA2BGR565);

    constant("COLOR_RGBA2BGRA", +cv::COLOR_RGBA2BGRA);

    constant("COLOR_RGBA2GRAY", +cv::COLOR_RGBA2GRAY);

    constant("COLOR_RGBA2RGB", +cv::COLOR_RGBA2RGB);

    constant("COLOR_RGBA2YUV_I420", +cv::COLOR_RGBA2YUV_I420);

    constant("COLOR_RGBA2YUV_IYUV", +cv::COLOR_RGBA2YUV_IYUV);

    constant("COLOR_RGBA2YUV_YV12", +cv::COLOR_RGBA2YUV_YV12);

    constant("COLOR_RGBA2mRGBA", +cv::COLOR_RGBA2mRGBA);

    constant("COLOR_XYZ2BGR", +cv::COLOR_XYZ2BGR);

    constant("COLOR_XYZ2RGB", +cv::COLOR_XYZ2RGB);

    constant("COLOR_YCrCb2BGR", +cv::COLOR_YCrCb2BGR);

    constant("COLOR_YCrCb2RGB", +cv::COLOR_YCrCb2RGB);

    constant("COLOR_YUV2BGR", +cv::COLOR_YUV2BGR);

    constant("COLOR_YUV2BGRA_I420", +cv::COLOR_YUV2BGRA_I420);

    constant("COLOR_YUV2BGRA_IYUV", +cv::COLOR_YUV2BGRA_IYUV);

    constant("COLOR_YUV2BGRA_NV12", +cv::COLOR_YUV2BGRA_NV12);

    constant("COLOR_YUV2BGRA_NV21", +cv::COLOR_YUV2BGRA_NV21);

    constant("COLOR_YUV2BGRA_UYNV", +cv::COLOR_YUV2BGRA_UYNV);

    constant("COLOR_YUV2BGRA_UYVY", +cv::COLOR_YUV2BGRA_UYVY);

    constant("COLOR_YUV2BGRA_Y422", +cv::COLOR_YUV2BGRA_Y422);

    constant("COLOR_YUV2BGRA_YUNV", +cv::COLOR_YUV2BGRA_YUNV);

    constant("COLOR_YUV2BGRA_YUY2", +cv::COLOR_YUV2BGRA_YUY2);

    constant("COLOR_YUV2BGRA_YUYV", +cv::COLOR_YUV2BGRA_YUYV);

    constant("COLOR_YUV2BGRA_YV12", +cv::COLOR_YUV2BGRA_YV12);

    constant("COLOR_YUV2BGRA_YVYU", +cv::COLOR_YUV2BGRA_YVYU);

    constant("COLOR_YUV2BGR_I420", +cv::COLOR_YUV2BGR_I420);

    constant("COLOR_YUV2BGR_IYUV", +cv::COLOR_YUV2BGR_IYUV);

    constant("COLOR_YUV2BGR_NV12", +cv::COLOR_YUV2BGR_NV12);

    constant("COLOR_YUV2BGR_NV21", +cv::COLOR_YUV2BGR_NV21);

    constant("COLOR_YUV2BGR_UYNV", +cv::COLOR_YUV2BGR_UYNV);

    constant("COLOR_YUV2BGR_UYVY", +cv::COLOR_YUV2BGR_UYVY);

    constant("COLOR_YUV2BGR_Y422", +cv::COLOR_YUV2BGR_Y422);

    constant("COLOR_YUV2BGR_YUNV", +cv::COLOR_YUV2BGR_YUNV);

    constant("COLOR_YUV2BGR_YUY2", +cv::COLOR_YUV2BGR_YUY2);

    constant("COLOR_YUV2BGR_YUYV", +cv::COLOR_YUV2BGR_YUYV);

    constant("COLOR_YUV2BGR_YV12", +cv::COLOR_YUV2BGR_YV12);

    constant("COLOR_YUV2BGR_YVYU", +cv::COLOR_YUV2BGR_YVYU);

    constant("COLOR_YUV2GRAY_420", +cv::COLOR_YUV2GRAY_420);

    constant("COLOR_YUV2GRAY_I420", +cv::COLOR_YUV2GRAY_I420);

    constant("COLOR_YUV2GRAY_IYUV", +cv::COLOR_YUV2GRAY_IYUV);

    constant("COLOR_YUV2GRAY_NV12", +cv::COLOR_YUV2GRAY_NV12);

    constant("COLOR_YUV2GRAY_NV21", +cv::COLOR_YUV2GRAY_NV21);

    constant("COLOR_YUV2GRAY_UYNV", +cv::COLOR_YUV2GRAY_UYNV);

    constant("COLOR_YUV2GRAY_UYVY", +cv::COLOR_YUV2GRAY_UYVY);

    constant("COLOR_YUV2GRAY_Y422", +cv::COLOR_YUV2GRAY_Y422);

    constant("COLOR_YUV2GRAY_YUNV", +cv::COLOR_YUV2GRAY_YUNV);

    constant("COLOR_YUV2GRAY_YUY2", +cv::COLOR_YUV2GRAY_YUY2);

    constant("COLOR_YUV2GRAY_YUYV", +cv::COLOR_YUV2GRAY_YUYV);

    constant("COLOR_YUV2GRAY_YV12", +cv::COLOR_YUV2GRAY_YV12);

    constant("COLOR_YUV2GRAY_YVYU", +cv::COLOR_YUV2GRAY_YVYU);

    constant("COLOR_YUV2RGB", +cv::COLOR_YUV2RGB);

    constant("COLOR_YUV2RGBA_I420", +cv::COLOR_YUV2RGBA_I420);

    constant("COLOR_YUV2RGBA_IYUV", +cv::COLOR_YUV2RGBA_IYUV);

    constant("COLOR_YUV2RGBA_NV12", +cv::COLOR_YUV2RGBA_NV12);

    constant("COLOR_YUV2RGBA_NV21", +cv::COLOR_YUV2RGBA_NV21);

    constant("COLOR_YUV2RGBA_UYNV", +cv::COLOR_YUV2RGBA_UYNV);

    constant("COLOR_YUV2RGBA_UYVY", +cv::COLOR_YUV2RGBA_UYVY);

    constant("COLOR_YUV2RGBA_Y422", +cv::COLOR_YUV2RGBA_Y422);

    constant("COLOR_YUV2RGBA_YUNV", +cv::COLOR_YUV2RGBA_YUNV);

    constant("COLOR_YUV2RGBA_YUY2", +cv::COLOR_YUV2RGBA_YUY2);

    constant("COLOR_YUV2RGBA_YUYV", +cv::COLOR_YUV2RGBA_YUYV);

    constant("COLOR_YUV2RGBA_YV12", +cv::COLOR_YUV2RGBA_YV12);

    constant("COLOR_YUV2RGBA_YVYU", +cv::COLOR_YUV2RGBA_YVYU);

    constant("COLOR_YUV2RGB_I420", +cv::COLOR_YUV2RGB_I420);

    constant("COLOR_YUV2RGB_IYUV", +cv::COLOR_YUV2RGB_IYUV);

    constant("COLOR_YUV2RGB_NV12", +cv::COLOR_YUV2RGB_NV12);

    constant("COLOR_YUV2RGB_NV21", +cv::COLOR_YUV2RGB_NV21);

    constant("COLOR_YUV2RGB_UYNV", +cv::COLOR_YUV2RGB_UYNV);

    constant("COLOR_YUV2RGB_UYVY", +cv::COLOR_YUV2RGB_UYVY);

    constant("COLOR_YUV2RGB_Y422", +cv::COLOR_YUV2RGB_Y422);

    constant("COLOR_YUV2RGB_YUNV", +cv::COLOR_YUV2RGB_YUNV);

    constant("COLOR_YUV2RGB_YUY2", +cv::COLOR_YUV2RGB_YUY2);

    constant("COLOR_YUV2RGB_YUYV", +cv::COLOR_YUV2RGB_YUYV);

    constant("COLOR_YUV2RGB_YV12", +cv::COLOR_YUV2RGB_YV12);

    constant("COLOR_YUV2RGB_YVYU", +cv::COLOR_YUV2RGB_YVYU);

    constant("COLOR_YUV420p2BGR", +cv::COLOR_YUV420p2BGR);

    constant("COLOR_YUV420p2BGRA", +cv::COLOR_YUV420p2BGRA);

    constant("COLOR_YUV420p2GRAY", +cv::COLOR_YUV420p2GRAY);

    constant("COLOR_YUV420p2RGB", +cv::COLOR_YUV420p2RGB);

    constant("COLOR_YUV420p2RGBA", +cv::COLOR_YUV420p2RGBA);

    constant("COLOR_YUV420sp2BGR", +cv::COLOR_YUV420sp2BGR);

    constant("COLOR_YUV420sp2BGRA", +cv::COLOR_YUV420sp2BGRA);

    constant("COLOR_YUV420sp2GRAY", +cv::COLOR_YUV420sp2GRAY);

    constant("COLOR_YUV420sp2RGB", +cv::COLOR_YUV420sp2RGB);

    constant("COLOR_YUV420sp2RGBA", +cv::COLOR_YUV420sp2RGBA);

    constant("COLOR_mRGBA2RGBA", +cv::COLOR_mRGBA2RGBA);

    constant("CONTOURS_MATCH_I1", +cv::CONTOURS_MATCH_I1);

    constant("CONTOURS_MATCH_I2", +cv::CONTOURS_MATCH_I2);

    constant("CONTOURS_MATCH_I3", +cv::CONTOURS_MATCH_I3);

    constant("COVAR_COLS", +cv::COVAR_COLS);

    constant("COVAR_NORMAL", +cv::COVAR_NORMAL);

    constant("COVAR_ROWS", +cv::COVAR_ROWS);

    constant("COVAR_SCALE", +cv::COVAR_SCALE);

    constant("COVAR_SCRAMBLED", +cv::COVAR_SCRAMBLED);

    constant("COVAR_USE_AVG", +cv::COVAR_USE_AVG);

    constant("CirclesGridFinderParameters_ASYMMETRIC_GRID", +cv::CirclesGridFinderParameters::ASYMMETRIC_GRID);

    constant("CirclesGridFinderParameters_SYMMETRIC_GRID", +cv::CirclesGridFinderParameters::SYMMETRIC_GRID);

    constant("DCT_INVERSE", +cv::DCT_INVERSE);

    constant("DCT_ROWS", +cv::DCT_ROWS);

    constant("DECOMP_CHOLESKY", +cv::DECOMP_CHOLESKY);

    constant("DECOMP_EIG", +cv::DECOMP_EIG);

    constant("DECOMP_LU", +cv::DECOMP_LU);

    constant("DECOMP_NORMAL", +cv::DECOMP_NORMAL);

    constant("DECOMP_QR", +cv::DECOMP_QR);

    constant("DECOMP_SVD", +cv::DECOMP_SVD);

    constant("DFT_COMPLEX_INPUT", +cv::DFT_COMPLEX_INPUT);

    constant("DFT_COMPLEX_OUTPUT", +cv::DFT_COMPLEX_OUTPUT);

    constant("DFT_INVERSE", +cv::DFT_INVERSE);

    constant("DFT_REAL_OUTPUT", +cv::DFT_REAL_OUTPUT);

    constant("DFT_ROWS", +cv::DFT_ROWS);

    constant("DFT_SCALE", +cv::DFT_SCALE);

    constant("DIST_C", +cv::DIST_C);

    constant("DIST_FAIR", +cv::DIST_FAIR);

    constant("DIST_HUBER", +cv::DIST_HUBER);

    constant("DIST_L1", +cv::DIST_L1);

    constant("DIST_L12", +cv::DIST_L12);

    constant("DIST_L2", +cv::DIST_L2);

    constant("DIST_LABEL_CCOMP", +cv::DIST_LABEL_CCOMP);

    constant("DIST_LABEL_PIXEL", +cv::DIST_LABEL_PIXEL);

    constant("DIST_MASK_3", +cv::DIST_MASK_3);

    constant("DIST_MASK_5", +cv::DIST_MASK_5);

    constant("DIST_MASK_PRECISE", +cv::DIST_MASK_PRECISE);

    constant("DIST_USER", +cv::DIST_USER);

    constant("DIST_WELSCH", +cv::DIST_WELSCH);

    constant("DescriptorMatcher_BRUTEFORCE", +cv::DescriptorMatcher::BRUTEFORCE);

    constant("DescriptorMatcher_BRUTEFORCE_HAMMING", +cv::DescriptorMatcher::BRUTEFORCE_HAMMING);

    constant("DescriptorMatcher_BRUTEFORCE_HAMMINGLUT", +cv::DescriptorMatcher::BRUTEFORCE_HAMMINGLUT);

    constant("DescriptorMatcher_BRUTEFORCE_L1", +cv::DescriptorMatcher::BRUTEFORCE_L1);

    constant("DescriptorMatcher_BRUTEFORCE_SL2", +cv::DescriptorMatcher::BRUTEFORCE_SL2);

    constant("DescriptorMatcher_FLANNBASED", +cv::DescriptorMatcher::FLANNBASED);

    constant("DrawMatchesFlags_DEFAULT", +cv::DrawMatchesFlags::DEFAULT);

    constant("DrawMatchesFlags_DRAW_OVER_OUTIMG", +cv::DrawMatchesFlags::DRAW_OVER_OUTIMG);

    constant("DrawMatchesFlags_DRAW_RICH_KEYPOINTS", +cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    constant("DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS", +cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    constant("FILLED", +cv::FILLED);

    constant("FLOODFILL_FIXED_RANGE", +cv::FLOODFILL_FIXED_RANGE);

    constant("FLOODFILL_MASK_ONLY", +cv::FLOODFILL_MASK_ONLY);

    constant("FM_7POINT", +cv::FM_7POINT);

    constant("FM_8POINT", +cv::FM_8POINT);

    constant("FM_LMEDS", +cv::FM_LMEDS);

    constant("FM_RANSAC", +cv::FM_RANSAC);

    constant("FONT_HERSHEY_COMPLEX", +cv::FONT_HERSHEY_COMPLEX);

    constant("FONT_HERSHEY_COMPLEX_SMALL", +cv::FONT_HERSHEY_COMPLEX_SMALL);

    constant("FONT_HERSHEY_DUPLEX", +cv::FONT_HERSHEY_DUPLEX);

    constant("FONT_HERSHEY_PLAIN", +cv::FONT_HERSHEY_PLAIN);

    constant("FONT_HERSHEY_SCRIPT_COMPLEX", +cv::FONT_HERSHEY_SCRIPT_COMPLEX);

    constant("FONT_HERSHEY_SCRIPT_SIMPLEX", +cv::FONT_HERSHEY_SCRIPT_SIMPLEX);

    constant("FONT_HERSHEY_SIMPLEX", +cv::FONT_HERSHEY_SIMPLEX);

    constant("FONT_HERSHEY_TRIPLEX", +cv::FONT_HERSHEY_TRIPLEX);

    constant("FONT_ITALIC", +cv::FONT_ITALIC);

    constant("FastFeatureDetector_FAST_N", +cv::FastFeatureDetector::FAST_N);

    constant("FastFeatureDetector_NONMAX_SUPPRESSION", +cv::FastFeatureDetector::NONMAX_SUPPRESSION);

    constant("FastFeatureDetector_THRESHOLD", +cv::FastFeatureDetector::THRESHOLD);

    constant("FastFeatureDetector_TYPE_5_8", +cv::FastFeatureDetector::TYPE_5_8);

    constant("FastFeatureDetector_TYPE_7_12", +cv::FastFeatureDetector::TYPE_7_12);

    constant("FastFeatureDetector_TYPE_9_16", +cv::FastFeatureDetector::TYPE_9_16);

    constant("FileNode_EMPTY", +cv::FileNode::EMPTY);

    constant("FileNode_FLOAT", +cv::FileNode::FLOAT);

    constant("FileNode_FLOW", +cv::FileNode::FLOW);

    constant("FileNode_INT", +cv::FileNode::INT);

    constant("FileNode_MAP", +cv::FileNode::MAP);

    constant("FileNode_NAMED", +cv::FileNode::NAMED);

    constant("FileNode_NONE", +cv::FileNode::NONE);

    constant("FileNode_REAL", +cv::FileNode::REAL);

    constant("FileNode_REF", +cv::FileNode::REF);

    constant("FileNode_SEQ", +cv::FileNode::SEQ);

    constant("FileNode_STR", +cv::FileNode::STR);

    constant("FileNode_STRING", +cv::FileNode::STRING);

    constant("FileNode_TYPE_MASK", +cv::FileNode::TYPE_MASK);

    constant("FileNode_USER", +cv::FileNode::USER);

    constant("FileStorage_APPEND", +cv::FileStorage::APPEND);

    constant("FileStorage_BASE64", +cv::FileStorage::BASE64);

    constant("FileStorage_FORMAT_AUTO", +cv::FileStorage::FORMAT_AUTO);

    constant("FileStorage_FORMAT_JSON", +cv::FileStorage::FORMAT_JSON);

    constant("FileStorage_FORMAT_MASK", +cv::FileStorage::FORMAT_MASK);

    constant("FileStorage_FORMAT_XML", +cv::FileStorage::FORMAT_XML);

    constant("FileStorage_FORMAT_YAML", +cv::FileStorage::FORMAT_YAML);

    constant("FileStorage_INSIDE_MAP", +cv::FileStorage::INSIDE_MAP);

    constant("FileStorage_MEMORY", +cv::FileStorage::MEMORY);

    constant("FileStorage_NAME_EXPECTED", +cv::FileStorage::NAME_EXPECTED);

    constant("FileStorage_READ", +cv::FileStorage::READ);

    constant("FileStorage_UNDEFINED", +cv::FileStorage::UNDEFINED);

    constant("FileStorage_VALUE_EXPECTED", +cv::FileStorage::VALUE_EXPECTED);

    constant("FileStorage_WRITE", +cv::FileStorage::WRITE);

    constant("FileStorage_WRITE_BASE64", +cv::FileStorage::WRITE_BASE64);

    constant("Formatter_FMT_C", +cv::Formatter::FMT_C);

    constant("Formatter_FMT_CSV", +cv::Formatter::FMT_CSV);

    constant("Formatter_FMT_DEFAULT", +cv::Formatter::FMT_DEFAULT);

    constant("Formatter_FMT_MATLAB", +cv::Formatter::FMT_MATLAB);

    constant("Formatter_FMT_NUMPY", +cv::Formatter::FMT_NUMPY);

    constant("Formatter_FMT_PYTHON", +cv::Formatter::FMT_PYTHON);

    constant("GC_BGD", +cv::GC_BGD);

    constant("GC_EVAL", +cv::GC_EVAL);

    constant("GC_EVAL_FREEZE_MODEL", +cv::GC_EVAL_FREEZE_MODEL);

    constant("GC_FGD", +cv::GC_FGD);

    constant("GC_INIT_WITH_MASK", +cv::GC_INIT_WITH_MASK);

    constant("GC_INIT_WITH_RECT", +cv::GC_INIT_WITH_RECT);

    constant("GC_PR_BGD", +cv::GC_PR_BGD);

    constant("GC_PR_FGD", +cv::GC_PR_FGD);

    constant("GEMM_1_T", +cv::GEMM_1_T);

    constant("GEMM_2_T", +cv::GEMM_2_T);

    constant("GEMM_3_T", +cv::GEMM_3_T);

    constant("HISTCMP_BHATTACHARYYA", +cv::HISTCMP_BHATTACHARYYA);

    constant("HISTCMP_CHISQR", +cv::HISTCMP_CHISQR);

    constant("HISTCMP_CHISQR_ALT", +cv::HISTCMP_CHISQR_ALT);

    constant("HISTCMP_CORREL", +cv::HISTCMP_CORREL);

    constant("HISTCMP_HELLINGER", +cv::HISTCMP_HELLINGER);

    constant("HISTCMP_INTERSECT", +cv::HISTCMP_INTERSECT);

    constant("HISTCMP_KL_DIV", +cv::HISTCMP_KL_DIV);

    constant("HOGDescriptor_DEFAULT_NLEVELS", +cv::HOGDescriptor::DEFAULT_NLEVELS);

    constant("HOGDescriptor_L2Hys", +cv::HOGDescriptor::L2Hys);

    constant("HOUGH_GRADIENT", +cv::HOUGH_GRADIENT);

    constant("HOUGH_MULTI_SCALE", +cv::HOUGH_MULTI_SCALE);

    constant("HOUGH_PROBABILISTIC", +cv::HOUGH_PROBABILISTIC);

    constant("HOUGH_STANDARD", +cv::HOUGH_STANDARD);

    constant("Hamming_normType", +cv::Hamming::normType);

    constant("INTERSECT_FULL", +cv::INTERSECT_FULL);

    constant("INTERSECT_NONE", +cv::INTERSECT_NONE);

    constant("INTERSECT_PARTIAL", +cv::INTERSECT_PARTIAL);

    constant("INTER_AREA", +cv::INTER_AREA);

    constant("INTER_BITS", +cv::INTER_BITS);

    constant("INTER_BITS2", +cv::INTER_BITS2);

    constant("INTER_CUBIC", +cv::INTER_CUBIC);

    constant("INTER_LANCZOS4", +cv::INTER_LANCZOS4);

    constant("INTER_LINEAR", +cv::INTER_LINEAR);

    constant("INTER_LINEAR_EXACT", +cv::INTER_LINEAR_EXACT);

    constant("INTER_MAX", +cv::INTER_MAX);

    constant("INTER_NEAREST", +cv::INTER_NEAREST);

    constant("INTER_NEAREST_EXACT", +cv::INTER_NEAREST_EXACT);

    constant("INTER_TAB_SIZE", +cv::INTER_TAB_SIZE);

    constant("INTER_TAB_SIZE2", +cv::INTER_TAB_SIZE2);

    constant("KAZE_DIFF_CHARBONNIER", +cv::KAZE::DIFF_CHARBONNIER);

    constant("KAZE_DIFF_PM_G1", +cv::KAZE::DIFF_PM_G1);

    constant("KAZE_DIFF_PM_G2", +cv::KAZE::DIFF_PM_G2);

    constant("KAZE_DIFF_WEICKERT", +cv::KAZE::DIFF_WEICKERT);

    constant("KMEANS_PP_CENTERS", +cv::KMEANS_PP_CENTERS);

    constant("KMEANS_RANDOM_CENTERS", +cv::KMEANS_RANDOM_CENTERS);

    constant("KMEANS_USE_INITIAL_LABELS", +cv::KMEANS_USE_INITIAL_LABELS);

    constant("LINE_4", +cv::LINE_4);

    constant("LINE_8", +cv::LINE_8);

    constant("LINE_AA", +cv::LINE_AA);

    constant("LMEDS", +cv::LMEDS);

    constant("LSD_REFINE_ADV", +cv::LSD_REFINE_ADV);

    constant("LSD_REFINE_NONE", +cv::LSD_REFINE_NONE);

    constant("LSD_REFINE_STD", +cv::LSD_REFINE_STD);

    constant("MARKER_CROSS", +cv::MARKER_CROSS);

    constant("MARKER_DIAMOND", +cv::MARKER_DIAMOND);

    constant("MARKER_SQUARE", +cv::MARKER_SQUARE);

    constant("MARKER_STAR", +cv::MARKER_STAR);

    constant("MARKER_TILTED_CROSS", +cv::MARKER_TILTED_CROSS);

    constant("MARKER_TRIANGLE_DOWN", +cv::MARKER_TRIANGLE_DOWN);

    constant("MARKER_TRIANGLE_UP", +cv::MARKER_TRIANGLE_UP);

    constant("MORPH_BLACKHAT", +cv::MORPH_BLACKHAT);

    constant("MORPH_CLOSE", +cv::MORPH_CLOSE);

    constant("MORPH_CROSS", +cv::MORPH_CROSS);

    constant("MORPH_DILATE", +cv::MORPH_DILATE);

    constant("MORPH_ELLIPSE", +cv::MORPH_ELLIPSE);

    constant("MORPH_ERODE", +cv::MORPH_ERODE);

    constant("MORPH_GRADIENT", +cv::MORPH_GRADIENT);

    constant("MORPH_HITMISS", +cv::MORPH_HITMISS);

    constant("MORPH_OPEN", +cv::MORPH_OPEN);

    constant("MORPH_RECT", +cv::MORPH_RECT);

    constant("MORPH_TOPHAT", +cv::MORPH_TOPHAT);

    constant("MOTION_AFFINE", +cv::MOTION_AFFINE);

    constant("MOTION_EUCLIDEAN", +cv::MOTION_EUCLIDEAN);

    constant("MOTION_HOMOGRAPHY", +cv::MOTION_HOMOGRAPHY);

    constant("MOTION_TRANSLATION", +cv::MOTION_TRANSLATION);

    constant("Mat_AUTO_STEP", +cv::Mat::AUTO_STEP);

    constant("Mat_CONTINUOUS_FLAG", +cv::Mat::CONTINUOUS_FLAG);

    constant("Mat_DEPTH_MASK", +cv::Mat::DEPTH_MASK);

    constant("Mat_MAGIC_MASK", +cv::Mat::MAGIC_MASK);

    constant("Mat_MAGIC_VAL", +cv::Mat::MAGIC_VAL);

    constant("Mat_SUBMATRIX_FLAG", +cv::Mat::SUBMATRIX_FLAG);

    constant("Mat_TYPE_MASK", +cv::Mat::TYPE_MASK);

    constant("NORM_HAMMING", +cv::NORM_HAMMING);

    constant("NORM_HAMMING2", +cv::NORM_HAMMING2);

    constant("NORM_INF", +cv::NORM_INF);

    constant("NORM_L1", +cv::NORM_L1);

    constant("NORM_L2", +cv::NORM_L2);

    constant("NORM_L2SQR", +cv::NORM_L2SQR);

    constant("NORM_MINMAX", +cv::NORM_MINMAX);

    constant("NORM_RELATIVE", +cv::NORM_RELATIVE);

    constant("NORM_TYPE_MASK", +cv::NORM_TYPE_MASK);

    constant("OPTFLOW_FARNEBACK_GAUSSIAN", +cv::OPTFLOW_FARNEBACK_GAUSSIAN);

    constant("OPTFLOW_LK_GET_MIN_EIGENVALS", +cv::OPTFLOW_LK_GET_MIN_EIGENVALS);

    constant("OPTFLOW_USE_INITIAL_FLOW", +cv::OPTFLOW_USE_INITIAL_FLOW);

    constant("ORB_FAST_SCORE", +cv::ORB::FAST_SCORE);

    constant("ORB_HARRIS_SCORE", +cv::ORB::HARRIS_SCORE);

    constant("ORB_kBytes", +cv::ORB::kBytes);

    constant("PCA_DATA_AS_COL", +cv::PCA::DATA_AS_COL);

    constant("PCA_DATA_AS_ROW", +cv::PCA::DATA_AS_ROW);

    constant("PCA_USE_AVG", +cv::PCA::USE_AVG);

    constant("PROJ_SPHERICAL_EQRECT", +cv::PROJ_SPHERICAL_EQRECT);

    constant("PROJ_SPHERICAL_ORTHO", +cv::PROJ_SPHERICAL_ORTHO);

    constant("Param_ALGORITHM", +cv::Param::ALGORITHM);

    constant("Param_BOOLEAN", +cv::Param::BOOLEAN);

    constant("Param_FLOAT", +cv::Param::FLOAT);

    constant("Param_INT", +cv::Param::INT);

    constant("Param_MAT", +cv::Param::MAT);

    constant("Param_MAT_VECTOR", +cv::Param::MAT_VECTOR);

    constant("Param_REAL", +cv::Param::REAL);

    constant("Param_SCALAR", +cv::Param::SCALAR);

    constant("Param_STRING", +cv::Param::STRING);

    constant("Param_UCHAR", +cv::Param::UCHAR);

    constant("Param_UINT64", +cv::Param::UINT64);

    constant("Param_UNSIGNED_INT", +cv::Param::UNSIGNED_INT);

    constant("RANSAC", +cv::RANSAC);

    constant("REDUCE_AVG", +cv::REDUCE_AVG);

    constant("REDUCE_MAX", +cv::REDUCE_MAX);

    constant("REDUCE_MIN", +cv::REDUCE_MIN);

    constant("REDUCE_SUM", +cv::REDUCE_SUM);

    constant("RETR_CCOMP", +cv::RETR_CCOMP);

    constant("RETR_EXTERNAL", +cv::RETR_EXTERNAL);

    constant("RETR_FLOODFILL", +cv::RETR_FLOODFILL);

    constant("RETR_LIST", +cv::RETR_LIST);

    constant("RETR_TREE", +cv::RETR_TREE);

    constant("RHO", +cv::RHO);

    constant("RNG_NORMAL", +cv::RNG::NORMAL);

    constant("RNG_UNIFORM", +cv::RNG::UNIFORM);

    constant("ROTATE_180", +cv::ROTATE_180);

    constant("ROTATE_90_CLOCKWISE", +cv::ROTATE_90_CLOCKWISE);

    constant("ROTATE_90_COUNTERCLOCKWISE", +cv::ROTATE_90_COUNTERCLOCKWISE);

    constant("SOLVELP_MULTI", +cv::SOLVELP_MULTI);

    constant("SOLVELP_SINGLE", +cv::SOLVELP_SINGLE);

    constant("SOLVELP_UNBOUNDED", +cv::SOLVELP_UNBOUNDED);

    constant("SOLVELP_UNFEASIBLE", +cv::SOLVELP_UNFEASIBLE);

    constant("SOLVEPNP_AP3P", +cv::SOLVEPNP_AP3P);

    constant("SOLVEPNP_DLS", +cv::SOLVEPNP_DLS);

    constant("SOLVEPNP_EPNP", +cv::SOLVEPNP_EPNP);

    constant("SOLVEPNP_IPPE", +cv::SOLVEPNP_IPPE);

    constant("SOLVEPNP_IPPE_SQUARE", +cv::SOLVEPNP_IPPE_SQUARE);

    constant("SOLVEPNP_ITERATIVE", +cv::SOLVEPNP_ITERATIVE);

    constant("SOLVEPNP_MAX_COUNT", +cv::SOLVEPNP_MAX_COUNT);

    constant("SOLVEPNP_P3P", +cv::SOLVEPNP_P3P);

    constant("SOLVEPNP_SQPNP", +cv::SOLVEPNP_SQPNP);

    constant("SOLVEPNP_UPNP", +cv::SOLVEPNP_UPNP);

    constant("SORT_ASCENDING", +cv::SORT_ASCENDING);

    constant("SORT_DESCENDING", +cv::SORT_DESCENDING);

    constant("SORT_EVERY_COLUMN", +cv::SORT_EVERY_COLUMN);

    constant("SORT_EVERY_ROW", +cv::SORT_EVERY_ROW);

    constant("SVD_FULL_UV", +cv::SVD::FULL_UV);

    constant("SVD_MODIFY_A", +cv::SVD::MODIFY_A);

    constant("SVD_NO_UV", +cv::SVD::NO_UV);

    constant("SparseMat_HASH_BIT", +cv::SparseMat::HASH_BIT);

    constant("SparseMat_HASH_SCALE", +cv::SparseMat::HASH_SCALE);

    constant("SparseMat_MAGIC_VAL", +cv::SparseMat::MAGIC_VAL);

    constant("SparseMat_MAX_DIM", +cv::SparseMat::MAX_DIM);

    constant("StereoBM_PREFILTER_NORMALIZED_RESPONSE", +cv::StereoBM::PREFILTER_NORMALIZED_RESPONSE);

    constant("StereoBM_PREFILTER_XSOBEL", +cv::StereoBM::PREFILTER_XSOBEL);

    constant("StereoMatcher_DISP_SCALE", +cv::StereoMatcher::DISP_SCALE);

    constant("StereoMatcher_DISP_SHIFT", +cv::StereoMatcher::DISP_SHIFT);

    constant("StereoSGBM_MODE_HH", +cv::StereoSGBM::MODE_HH);

    constant("StereoSGBM_MODE_HH4", +cv::StereoSGBM::MODE_HH4);

    constant("StereoSGBM_MODE_SGBM", +cv::StereoSGBM::MODE_SGBM);

    constant("StereoSGBM_MODE_SGBM_3WAY", +cv::StereoSGBM::MODE_SGBM_3WAY);

    constant("Subdiv2D_NEXT_AROUND_DST", +cv::Subdiv2D::NEXT_AROUND_DST);

    constant("Subdiv2D_NEXT_AROUND_LEFT", +cv::Subdiv2D::NEXT_AROUND_LEFT);

    constant("Subdiv2D_NEXT_AROUND_ORG", +cv::Subdiv2D::NEXT_AROUND_ORG);

    constant("Subdiv2D_NEXT_AROUND_RIGHT", +cv::Subdiv2D::NEXT_AROUND_RIGHT);

    constant("Subdiv2D_PREV_AROUND_DST", +cv::Subdiv2D::PREV_AROUND_DST);

    constant("Subdiv2D_PREV_AROUND_LEFT", +cv::Subdiv2D::PREV_AROUND_LEFT);

    constant("Subdiv2D_PREV_AROUND_ORG", +cv::Subdiv2D::PREV_AROUND_ORG);

    constant("Subdiv2D_PREV_AROUND_RIGHT", +cv::Subdiv2D::PREV_AROUND_RIGHT);

    constant("Subdiv2D_PTLOC_ERROR", +cv::Subdiv2D::PTLOC_ERROR);

    constant("Subdiv2D_PTLOC_INSIDE", +cv::Subdiv2D::PTLOC_INSIDE);

    constant("Subdiv2D_PTLOC_ON_EDGE", +cv::Subdiv2D::PTLOC_ON_EDGE);

    constant("Subdiv2D_PTLOC_OUTSIDE_RECT", +cv::Subdiv2D::PTLOC_OUTSIDE_RECT);

    constant("Subdiv2D_PTLOC_VERTEX", +cv::Subdiv2D::PTLOC_VERTEX);

    constant("THRESH_BINARY", +cv::THRESH_BINARY);

    constant("THRESH_BINARY_INV", +cv::THRESH_BINARY_INV);

    constant("THRESH_MASK", +cv::THRESH_MASK);

    constant("THRESH_OTSU", +cv::THRESH_OTSU);

    constant("THRESH_TOZERO", +cv::THRESH_TOZERO);

    constant("THRESH_TOZERO_INV", +cv::THRESH_TOZERO_INV);

    constant("THRESH_TRIANGLE", +cv::THRESH_TRIANGLE);

    constant("THRESH_TRUNC", +cv::THRESH_TRUNC);

    constant("TM_CCOEFF", +cv::TM_CCOEFF);

    constant("TM_CCOEFF_NORMED", +cv::TM_CCOEFF_NORMED);

    constant("TM_CCORR", +cv::TM_CCORR);

    constant("TM_CCORR_NORMED", +cv::TM_CCORR_NORMED);

    constant("TM_SQDIFF", +cv::TM_SQDIFF);

    constant("TM_SQDIFF_NORMED", +cv::TM_SQDIFF_NORMED);

    constant("TermCriteria_COUNT", +cv::TermCriteria::COUNT);

    constant("TermCriteria_EPS", +cv::TermCriteria::EPS);

    constant("TermCriteria_MAX_ITER", +cv::TermCriteria::MAX_ITER);

    constant("UMatData_ASYNC_CLEANUP", +cv::UMatData::ASYNC_CLEANUP);

    constant("UMatData_COPY_ON_MAP", +cv::UMatData::COPY_ON_MAP);

    constant("UMatData_DEVICE_COPY_OBSOLETE", +cv::UMatData::DEVICE_COPY_OBSOLETE);

    constant("UMatData_DEVICE_MEM_MAPPED", +cv::UMatData::DEVICE_MEM_MAPPED);

    constant("UMatData_HOST_COPY_OBSOLETE", +cv::UMatData::HOST_COPY_OBSOLETE);

    constant("UMatData_TEMP_COPIED_UMAT", +cv::UMatData::TEMP_COPIED_UMAT);

    constant("UMatData_TEMP_UMAT", +cv::UMatData::TEMP_UMAT);

    constant("UMatData_USER_ALLOCATED", +cv::UMatData::USER_ALLOCATED);

    constant("UMat_AUTO_STEP", +cv::UMat::AUTO_STEP);

    constant("UMat_CONTINUOUS_FLAG", +cv::UMat::CONTINUOUS_FLAG);

    constant("UMat_DEPTH_MASK", +cv::UMat::DEPTH_MASK);

    constant("UMat_MAGIC_MASK", +cv::UMat::MAGIC_MASK);

    constant("UMat_MAGIC_VAL", +cv::UMat::MAGIC_VAL);

    constant("UMat_SUBMATRIX_FLAG", +cv::UMat::SUBMATRIX_FLAG);

    constant("UMat_TYPE_MASK", +cv::UMat::TYPE_MASK);

    constant("USAGE_ALLOCATE_DEVICE_MEMORY", +cv::USAGE_ALLOCATE_DEVICE_MEMORY);

    constant("USAGE_ALLOCATE_HOST_MEMORY", +cv::USAGE_ALLOCATE_HOST_MEMORY);

    constant("USAGE_ALLOCATE_SHARED_MEMORY", +cv::USAGE_ALLOCATE_SHARED_MEMORY);

    constant("USAGE_DEFAULT", +cv::USAGE_DEFAULT);

    constant("WARP_FILL_OUTLIERS", +cv::WARP_FILL_OUTLIERS);

    constant("WARP_INVERSE_MAP", +cv::WARP_INVERSE_MAP);

    constant("WARP_POLAR_LINEAR", +cv::WARP_POLAR_LINEAR);

    constant("WARP_POLAR_LOG", +cv::WARP_POLAR_LOG);

    constant("_InputArray_CUDA_GPU_MAT", +cv::_InputArray::CUDA_GPU_MAT);

    constant("_InputArray_CUDA_HOST_MEM", +cv::_InputArray::CUDA_HOST_MEM);

    constant("_InputArray_EXPR", +cv::_InputArray::EXPR);

    constant("_InputArray_FIXED_SIZE", +cv::_InputArray::FIXED_SIZE);

    constant("_InputArray_FIXED_TYPE", +cv::_InputArray::FIXED_TYPE);

    constant("_InputArray_KIND_MASK", +cv::_InputArray::KIND_MASK);

    constant("_InputArray_KIND_SHIFT", +cv::_InputArray::KIND_SHIFT);

    constant("_InputArray_MAT", +cv::_InputArray::MAT);

    constant("_InputArray_MATX", +cv::_InputArray::MATX);

    constant("_InputArray_NONE", +cv::_InputArray::NONE);

    constant("_InputArray_OPENGL_BUFFER", +cv::_InputArray::OPENGL_BUFFER);

    constant("_InputArray_STD_ARRAY", +cv::_InputArray::STD_ARRAY);

    constant("_InputArray_STD_ARRAY_MAT", +cv::_InputArray::STD_ARRAY_MAT);

    constant("_InputArray_STD_BOOL_VECTOR", +cv::_InputArray::STD_BOOL_VECTOR);

    constant("_InputArray_STD_VECTOR", +cv::_InputArray::STD_VECTOR);

    constant("_InputArray_STD_VECTOR_CUDA_GPU_MAT", +cv::_InputArray::STD_VECTOR_CUDA_GPU_MAT);

    constant("_InputArray_STD_VECTOR_MAT", +cv::_InputArray::STD_VECTOR_MAT);

    constant("_InputArray_STD_VECTOR_UMAT", +cv::_InputArray::STD_VECTOR_UMAT);

    constant("_InputArray_STD_VECTOR_VECTOR", +cv::_InputArray::STD_VECTOR_VECTOR);

    constant("_InputArray_UMAT", +cv::_InputArray::UMAT);

    constant("_OutputArray_DEPTH_MASK_16S", +cv::_OutputArray::DEPTH_MASK_16S);

    constant("_OutputArray_DEPTH_MASK_16U", +cv::_OutputArray::DEPTH_MASK_16U);

    constant("_OutputArray_DEPTH_MASK_32F", +cv::_OutputArray::DEPTH_MASK_32F);

    constant("_OutputArray_DEPTH_MASK_32S", +cv::_OutputArray::DEPTH_MASK_32S);

    constant("_OutputArray_DEPTH_MASK_64F", +cv::_OutputArray::DEPTH_MASK_64F);

    constant("_OutputArray_DEPTH_MASK_8S", +cv::_OutputArray::DEPTH_MASK_8S);

    constant("_OutputArray_DEPTH_MASK_8U", +cv::_OutputArray::DEPTH_MASK_8U);

    constant("_OutputArray_DEPTH_MASK_ALL", +cv::_OutputArray::DEPTH_MASK_ALL);

    constant("_OutputArray_DEPTH_MASK_ALL_BUT_8S", +cv::_OutputArray::DEPTH_MASK_ALL_BUT_8S);

    constant("_OutputArray_DEPTH_MASK_FLT", +cv::_OutputArray::DEPTH_MASK_FLT);

    constant("__UMAT_USAGE_FLAGS_32BIT", +cv::__UMAT_USAGE_FLAGS_32BIT);

    constant("BadAlign", +cv::Error::BadAlign);

    constant("BadAlphaChannel", +cv::Error::BadAlphaChannel);

    constant("BadCOI", +cv::Error::BadCOI);

    constant("BadCallBack", +cv::Error::BadCallBack);

    constant("BadDataPtr", +cv::Error::BadDataPtr);

    constant("BadDepth", +cv::Error::BadDepth);

    constant("BadImageSize", +cv::Error::BadImageSize);

    constant("BadModelOrChSeq", +cv::Error::BadModelOrChSeq);

    constant("BadNumChannel1U", +cv::Error::BadNumChannel1U);

    constant("BadNumChannels", +cv::Error::BadNumChannels);

    constant("BadOffset", +cv::Error::BadOffset);

    constant("BadOrder", +cv::Error::BadOrder);

    constant("BadOrigin", +cv::Error::BadOrigin);

    constant("BadROISize", +cv::Error::BadROISize);

    constant("BadStep", +cv::Error::BadStep);

    constant("BadTileSize", +cv::Error::BadTileSize);

    constant("GpuApiCallError", +cv::Error::GpuApiCallError);

    constant("GpuNotSupported", +cv::Error::GpuNotSupported);

    constant("HeaderIsNull", +cv::Error::HeaderIsNull);

    constant("MaskIsTiled", +cv::Error::MaskIsTiled);

    constant("OpenCLApiCallError", +cv::Error::OpenCLApiCallError);

    constant("OpenCLDoubleNotSupported", +cv::Error::OpenCLDoubleNotSupported);

    constant("OpenCLInitError", +cv::Error::OpenCLInitError);

    constant("OpenCLNoAMDBlasFft", +cv::Error::OpenCLNoAMDBlasFft);

    constant("OpenGlApiCallError", +cv::Error::OpenGlApiCallError);

    constant("OpenGlNotSupported", +cv::Error::OpenGlNotSupported);

    constant("StsAssert", +cv::Error::StsAssert);

    constant("StsAutoTrace", +cv::Error::StsAutoTrace);

    constant("StsBackTrace", +cv::Error::StsBackTrace);

    constant("StsBadArg", +cv::Error::StsBadArg);

    constant("StsBadFlag", +cv::Error::StsBadFlag);

    constant("StsBadFunc", +cv::Error::StsBadFunc);

    constant("StsBadMask", +cv::Error::StsBadMask);

    constant("StsBadMemBlock", +cv::Error::StsBadMemBlock);

    constant("StsBadPoint", +cv::Error::StsBadPoint);

    constant("StsBadSize", +cv::Error::StsBadSize);

    constant("StsDivByZero", +cv::Error::StsDivByZero);

    constant("StsError", +cv::Error::StsError);

    constant("StsFilterOffsetErr", +cv::Error::StsFilterOffsetErr);

    constant("StsFilterStructContentErr", +cv::Error::StsFilterStructContentErr);

    constant("StsInplaceNotSupported", +cv::Error::StsInplaceNotSupported);

    constant("StsInternal", +cv::Error::StsInternal);

    constant("StsKernelStructContentErr", +cv::Error::StsKernelStructContentErr);

    constant("StsNoConv", +cv::Error::StsNoConv);

    constant("StsNoMem", +cv::Error::StsNoMem);

    constant("StsNotImplemented", +cv::Error::StsNotImplemented);

    constant("StsNullPtr", +cv::Error::StsNullPtr);

    constant("StsObjectNotFound", +cv::Error::StsObjectNotFound);

    constant("StsOk", +cv::Error::StsOk);

    constant("StsOutOfRange", +cv::Error::StsOutOfRange);

    constant("StsParseError", +cv::Error::StsParseError);

    constant("StsUnmatchedFormats", +cv::Error::StsUnmatchedFormats);

    constant("StsUnmatchedSizes", +cv::Error::StsUnmatchedSizes);

    constant("StsUnsupportedFormat", +cv::Error::StsUnsupportedFormat);

    constant("StsVecLengthErr", +cv::Error::StsVecLengthErr);

    constant("TEST_CUSTOM", +cv::detail::TEST_CUSTOM);

    constant("TEST_EQ", +cv::detail::TEST_EQ);

    constant("TEST_GE", +cv::detail::TEST_GE);

    constant("TEST_GT", +cv::detail::TEST_GT);

    constant("TEST_LE", +cv::detail::TEST_LE);

    constant("TEST_LT", +cv::detail::TEST_LT);

    constant("TEST_NE", +cv::detail::TEST_NE);

    constant("DNN_BACKEND_DEFAULT", +cv::dnn::DNN_BACKEND_DEFAULT);

    constant("DNN_BACKEND_HALIDE", +cv::dnn::DNN_BACKEND_HALIDE);

    constant("DNN_BACKEND_INFERENCE_ENGINE", +cv::dnn::DNN_BACKEND_INFERENCE_ENGINE);

    constant("DNN_BACKEND_OPENCV", +cv::dnn::DNN_BACKEND_OPENCV);

    constant("DNN_TARGET_CPU", +cv::dnn::DNN_TARGET_CPU);

    constant("DNN_TARGET_FPGA", +cv::dnn::DNN_TARGET_FPGA);

    constant("DNN_TARGET_MYRIAD", +cv::dnn::DNN_TARGET_MYRIAD);

    constant("DNN_TARGET_OPENCL", +cv::dnn::DNN_TARGET_OPENCL);

    constant("DNN_TARGET_OPENCL_FP16", +cv::dnn::DNN_TARGET_OPENCL_FP16);

    constant("CALIB_CHECK_COND", +cv::fisheye::CALIB_CHECK_COND);

    constant("CALIB_FIX_INTRINSIC", +cv::fisheye::CALIB_FIX_INTRINSIC);

    constant("CALIB_FIX_K1", +cv::fisheye::CALIB_FIX_K1);

    constant("CALIB_FIX_K2", +cv::fisheye::CALIB_FIX_K2);

    constant("CALIB_FIX_K3", +cv::fisheye::CALIB_FIX_K3);

    constant("CALIB_FIX_K4", +cv::fisheye::CALIB_FIX_K4);

    constant("CALIB_FIX_PRINCIPAL_POINT", +cv::fisheye::CALIB_FIX_PRINCIPAL_POINT);

    constant("CALIB_FIX_SKEW", +cv::fisheye::CALIB_FIX_SKEW);

    constant("CALIB_RECOMPUTE_EXTRINSIC", +cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC);

    constant("CALIB_USE_INTRINSIC_GUESS", +cv::fisheye::CALIB_USE_INTRINSIC_GUESS);

    constant("CALIB_ZERO_DISPARITY", +cv::fisheye::CALIB_ZERO_DISPARITY);

    constant("ENUM_LOG_LEVEL_FORCE_INT", +cv::utils::logging::ENUM_LOG_LEVEL_FORCE_INT);

    constant("LOG_LEVEL_DEBUG", +cv::utils::logging::LOG_LEVEL_DEBUG);

    constant("LOG_LEVEL_ERROR", +cv::utils::logging::LOG_LEVEL_ERROR);

    constant("LOG_LEVEL_FATAL", +cv::utils::logging::LOG_LEVEL_FATAL);

    constant("LOG_LEVEL_INFO", +cv::utils::logging::LOG_LEVEL_INFO);

    constant("LOG_LEVEL_SILENT", +cv::utils::logging::LOG_LEVEL_SILENT);

    constant("LOG_LEVEL_VERBOSE", +cv::utils::logging::LOG_LEVEL_VERBOSE);

    constant("LOG_LEVEL_WARNING", +cv::utils::logging::LOG_LEVEL_WARNING);

}
