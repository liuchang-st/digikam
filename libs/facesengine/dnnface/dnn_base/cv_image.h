/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * <a href="http://www.digikam.org">http://www.digikam.org</a>
 *
 * @date    2017-08-08
 * @brief   Class byte_orderer for dnn module, can be used for face recognition, 
 *          all codes are ported from dlib library (http://dlib.net/)
 *
 * @section DESCRIPTION
 *
 * @author Copyright (C) 2017 by Yingjie Liu
 *         <a href="mailto:yingjiewudi at gmail dot com">yingjiewudi at gmail dot com</a>
 *
 * @section LICENSE
 *
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 *
 * ============================================================ */
#ifndef _DLIB_CV_IMAGE_H_
#define _DLIB_CV_IMAGE_H_

#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include "algs.h"
#include "pixel.h"
#include "matrix_mat.h"

template <
        typename pixel_type
        >
class cv_image
{
public:
    typedef pixel_type type;
    typedef default_memory_manager mem_manager_type;

    cv_image (const cv::Mat img)
    {
        DLIB_CASSERT(img.depth() == cv::DataType<typename pixel_traits<pixel_type>::basic_pixel_type>::depth &&
                     img.channels() == pixel_traits<pixel_type>::num,
                     "The pixel type you gave doesn't match pixel used by the open cv Mat object."
                     << "\n\t img.depth():    " << img.depth()
                     << "\n\t img.cv::DataType<typename pixel_traits<pixel_type>::basic_pixel_type>::depth: "
                     << cv::DataType<typename pixel_traits<pixel_type>::basic_pixel_type>::depth
                     << "\n\t img.channels(): " << img.channels()
                     << "\n\t img.pixel_traits<pixel_type>::num: " << pixel_traits<pixel_type>::num
                     );
        IplImage temp = img;
        init(&temp);
    }

    cv_image (const IplImage img)
    {
        init(&img);
    }

    cv_image (const IplImage* img)
    {
        init(img);
    }

    cv_image() : _data(0), _widthStep(0), _nr(0), _nc(0) {}

    unsigned long size () const { return static_cast<unsigned long>(_nr*_nc); }

    inline pixel_type* operator[](const long row )
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(0 <= row && row < nr(),
                    "\tpixel_type* cv_image::operator[](row)"
                    << "\n\t you have asked for an out of bounds row "
                    << "\n\t row:  " << row
                    << "\n\t nr(): " << nr()
                    << "\n\t this:  " << this
                    );

        return reinterpret_cast<pixel_type*>( _data + _widthStep*row);
    }

    inline const pixel_type* operator[](const long row ) const
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(0 <= row && row < nr(),
                    "\tconst pixel_type* cv_image::operator[](row)"
                    << "\n\t you have asked for an out of bounds row "
                    << "\n\t row:  " << row
                    << "\n\t nr(): " << nr()
                    << "\n\t this:  " << this
                    );

        return reinterpret_cast<const pixel_type*>( _data + _widthStep*row);
    }

    long nr() const { return _nr; }
    long nc() const { return _nc; }
    long width_step() const { return _widthStep; }

    cv_image& operator=( const cv_image& item)
    {
        _data = item._data;
        _widthStep = item._widthStep;
        _nr = item._nr;
        _nc = item._nc;
        return *this;
    }

    cv_image& operator=( const IplImage* img)
    {
        init(img);
        return *this;
    }

    cv_image& operator=( const IplImage img)
    {
        init(&img);
        return *this;
    }

    cv_image& operator=( const cv::Mat img)
    {
        IplImage temp = img;
        init(&temp);
        return *this;
    }

private:

    void init (const IplImage* img)
    {
        DLIB_CASSERT( img->dataOrder == 0, "Only interleaved color channels are supported with cv_image");
        DLIB_CASSERT((img->depth&0xFF)/8*img->nChannels == sizeof(pixel_type),
                     "The pixel type you gave doesn't match the size of pixel used by the open cv image struct");

        _data = img->imageData;
        _widthStep = img->widthStep;
        _nr = img->height;
        _nc = img->width;

    }

    char* _data;
    long _widthStep;
    long _nr;
    long _nc;
};

// ----------------------------------------------------------------------------------------

template <
        typename T
        >
const matrix_op<op_array2d_to_mat<cv_image<T> > > mat (
        const cv_image<T>& m
        )
{
    typedef op_array2d_to_mat<cv_image<T> > op;
    return matrix_op<op>(op(m));
}

#endif // _DLIB_CV_IMAGE_H_