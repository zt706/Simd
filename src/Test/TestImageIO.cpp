/*
* Tests for Simd Library (http://ermig1979.github.io/Simd).
*
* Copyright (c) 2011-2017 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/TestData.h"

#include "Simd/SimdImageLoad.h"
#include "Simd/SimdImageSave.h"

namespace Test
{
    namespace
    {
        struct FuncSM
        {
            typedef Simd::ImageSaveToMemoryPtr FuncPtr;

            FuncPtr func;
            String desc;

            FuncSM(const FuncPtr& f, const String& d) : func(f), desc(d) {}

            void Update(View::Format format, SimdImageFileType file)
            {
                desc = desc + "[" + ToString(format) + "-" + ToString(file) + "]";
            }

            uint8_t* Call(const View& src, SimdImageFileType file, int quality, size_t* size) const
            {
                TEST_PERFORMANCE_TEST(desc);
                return func(src.data, src.stride, src.width, src.height, (SimdPixelFormatType)src.format, file, quality, size);
            }
        };
    }

#define FUNC_SM(func) \
    FuncSM(func, std::string(#func))

    bool ImageSaveToMemoryAutoTest(size_t width, size_t height, View::Format format, SimdImageFileType file, int quality, FuncSM f1, FuncSM f2)
    {
        bool result = true;

        f1.Update(format, file);
        f2.Update(format, file);

        TEST_LOG_SS(Info, "Test " << f1.desc << " & " << f2.desc << " [" << width << ", " << height << "].");

        View src(width, height, format, NULL, TEST_ALIGN(width));
        FillRandom(src);

        uint8_t* data1 = NULL, * data2 = NULL;
        size_t size1 = 0, size2 = 0;

        data1 = f1.Call(src, file, quality, &size1);

        data2 = f2.Call(src, file, quality, &size2);

        result = result && Compare(data1, size1, data2, size2, 0, true, 64);

        if (data1)
            SimdFree(data1);
        if (data2)
            SimdFree(data2);

        //SimdImageSaveToFile(src.data, src.stride, src.width, src.height, (SimdPixelFormatType)src.format, file, 100, "saved.txt");

        return result;
    }

    bool ImageSaveToMemoryAutoTest(const FuncSM & f1, const FuncSM& f2)
    {
        bool result = true;

        View::Format formats[3] = { View::Gray8, View::Bgr24, View::Bgra32 };
        for (int format = 0; format < 3; format++)
        {
            for (int file = (int)SimdImageFilePgmTxt; file <= (int)SimdImageFilePpmBin; file++)
            {
                result = result && ImageSaveToMemoryAutoTest(W, H, formats[format], (SimdImageFileType)file, 100, f1, f2);
                result = result && ImageSaveToMemoryAutoTest(W + O, H - O, formats[format], (SimdImageFileType)file, 100, f1, f2);
            }
        }

        return result;
    }

    bool ImageSaveToMemoryAutoTest()
    {
        bool result = true;

        result = result && ImageSaveToMemoryAutoTest(FUNC_SM(Simd::Base::ImageSaveToMemory), FUNC_SM(SimdImageSaveToMemory));

        return result;
    }

    //-----------------------------------------------------------------------

    bool ImageLoadFromMemoryAutoTest()
    {
        bool result = true;

        //result = result && CopyFrameAutoTest(FUNC_F(Simd::Base::CopyFrame), FUNC_F(SimdCopyFrame));

        return result;
    }
}
