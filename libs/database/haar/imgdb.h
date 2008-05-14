/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-17
 * Description : Haar DB interface from imgseek project
 *
 * Copyright (C) 2003 by Ricardo Niederberger Cabral <nieder at mail dot ru>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef IMGDBASE_H
#define IMGDBASE_H

namespace Digikam
{

/** Weights for the Haar coefficients.
    Straight from the referenced paper
*/
const float weights[2][6][3]=
{
    // For scanned picture (sketch=0):
    //    Y      I      Q       idx total occurs
    {{ 5.00, 19.21, 34.37},  // 0   58.58      1 (`DC' component)
     { 0.83,  1.26,  0.36},  // 1    2.45      3
     { 1.01,  0.44,  0.45},  // 2    1.90      5
     { 0.52,  0.53,  0.14},  // 3    1.19      7
     { 0.47,  0.28,  0.18},  // 4    0.93      9
     { 0.30,  0.14,  0.27}}, // 5    0.71      16384-25=16359
    
    // For handdrawn/painted sketch (sketch=1):
    //    Y      I      Q
    {{ 4.04, 15.14, 22.62},
     { 0.78,  0.92,  0.40},
     { 0.46,  0.53,  0.63},
     { 0.42,  0.26,  0.25},
     { 0.41,  0.14,  0.15},
     { 0.32,  0.07,  0.38}}
};

/** signature structure 
*/
typedef struct sigStruct_
{
    long int id;			    // picture id
    Idx      sig1[NUM_COEFS];   // Y positions with largest magnitude
    Idx      sig2[NUM_COEFS];   // I positions with largest magnitude
    Idx      sig3[NUM_COEFS];   // Q positions with largest magnitude
    double   avgl[3];           // YIQ for position [0,0]
    double   score;             // used when doing queries
    
    // image properties extracted when opened for the first time
    int width;                  // in pixels
    int height;                 // in pixels
    
    bool operator< (const sigStruct_ & right) const 
    {
        return score < (right.score);
    }
} sigStruct;

struct cmpf
{
    bool operator()(const long int s1, const long int s2) const
    {
        return s1<s2;
    }
};

/** Fixed weight mask for pixel positions (i,j).
    Each entry x = i*NUM_PIXELS + j, gets value max(i,j) saturated at 5.
    To be treated as a constant.
*/
unsigned char imgBin[16384];

typedef std::map<const long int, sigStruct*, cmpf>::iterator sigIterator;
typedef std::list<long int>                                  long_list;
typedef long_list::iterator                                  long_listIterator;
typedef std::priority_queue <sigStruct>                      priqueue;
typedef std::list<long_list>                                 long_list_2; // a list of lists
typedef std::map<const long int, sigStruct*, cmpf>           sigMap;

sigMap sigs;

/** Lists of picture ids, indexed by [color-channel][sign][position], i.e.,
    R=0/G=1/B=2, pos=0/neg=1, (i*NUM_PIXELS+j)
*/
long_list imgbuckets[3][2][16384];

/** results priority queue; largest at top 
*/
priqueue  pqResults;            

/** current result waiting to be returned 
*/
sigStruct curResult;

/** number of results found 
*/
int       numres;               

}  // namespace Digikam

#endif // IMGDBASE_H
