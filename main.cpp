////////////////////////////////////////////////////////////////////////////////
//              Empirical Mode Decomposition                                  //
// BERNARD Guillaume                                                          //
// DURAND William                                                             //
// ZZ3F2 ISIMA                                                                //
////////////////////////////////////////////////////////////////////////////////

#include "CImg.h"
#include <math.h>

#include <vector>
#include <iostream>

#include "Euclidean.hpp"

#ifdef DEBUG
#include <time.h>
#endif

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

using namespace cimg_library;

int SIZE = 3;

double sum(CImg<float> img, int startedX, int startedY, int w) {
    double res = 0;
    for (int i = startedX - ((w - 1) / 2); i < startedX + ((w + 1) / 2); i++) {
        for (int j = startedY - ((w - 1) / 2) ; j < startedY + ((w + 1) / 2); j++) {
            if ((i >= 0 && i < img.width()) && (j >= 0  && j < img.height())) {
                res += img(i,j);
            }
        }
    }
    return res;
}

CImg<float> decompose(const CImg<float> input)
{
    CImg<float>inputImg(input);
    std::vector<Euclidean> vectEMax, vectEMin;

    ///////////////////////////////////////////////////////////////////////////////
    //              Part 1: Finding minimas and maximas                          //
    ///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    timeval tim;
    double t1, t2;
#endif

    CImg<float> imgMax(inputImg.channel(0));
    CImg<float> imgMin(inputImg.channel(0));

#ifdef DEBUG
    fprintf(stdout, "%-40s", "Calculate the extremas...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    int xmin, xmax, ymin, ymax;
    float min, max;

    for (int i = 0; i < inputImg.width(); i += SIZE) {
        for (int j = 0; j < inputImg.height(); j += SIZE) {

            // Save max and min locations
            xmax = i;
            ymax = j;
            xmin = i;
            ymin = j;

            // save values
            max = imgMax(i,j);
            min = imgMin(i,j);

            Euclidean eMax(i, j);
            Euclidean eMin(i, j);

            // SIZExSIZE
            for (int k = i; k < i + SIZE; k++) {
                for (int l = j; l < j + SIZE; l++) {

                    // Max
                    if ((imgMax(k, l) <= max) && (l != ymax || k != xmax)) {
                        imgMax(k, l) = 0;
                    } else if (l!=ymax || k!=xmax) {
                        max = imgMax(k, l);
                        imgMax(xmax,ymax) = 0;
                        xmax = k;
                        ymax = l;

                        eMax.setX(k);
                        eMax.setY(l);
                    }

                    // Min
                    if ((imgMin(k, l) >= min) && (l != ymin || k != xmin)) {
                        imgMin(k, l) = 0;
                    } else if (l != ymax || k != xmax) {
                        min = imgMin(k, l);
                        imgMin(xmin, ymin) = 0;
                        xmin = k;
                        ymin = l;

                        eMin.setX(k);
                        eMin.setY(l);
                    }
                }
            }

            vectEMax.push_back(eMax);
            vectEMin.push_back(eMin);
        }
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);
#endif

    // Array of Euclidean distance to the nearest non zero element
    std::vector<Euclidean>::iterator it1, it2;

#ifdef DEBUG
    fprintf(stdout, "%-40s", "Calculate the Euclidean distances...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    for (it1 = vectEMax.begin(); it1 != vectEMax.end(); it1++) {
        for (it2 = it1 + 1; it2 != vectEMax.end(); it2++) {
            double dist = (*it1).computeDistanceFrom(*it2);

            if (0 == (*it1).getDistance() || dist < (*it1).getDistance()) {
                (*it1).setDistance(dist);
                (*it1).setNearest(*it2);
            }

            if (0 == (*it2).getDistance() || dist < (*it2).getDistance()) {
                (*it2).setDistance(dist);
                (*it2).setNearest(*it1);
            }
        }
    }

    for (it1 = vectEMin.begin(); it1 != vectEMin.end(); it1++) {
        for (it2 = it1 + 1; it2 != vectEMin.end(); it2++) {
            double dist = (*it1).computeDistanceFrom(*it2);

            if (0 == (*it1).getDistance() || dist < (*it1).getDistance()) {
                (*it1).setDistance(dist);
                (*it1).setNearest(*it2);
            }

            if (0 == (*it2).getDistance() || dist < (*it2).getDistance()) {
                (*it2).setDistance(dist);
                (*it2).setNearest(*it1);
            }
        }
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);

    fprintf(stdout, "%-40s", "Calculate the window size...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Calculate the window size
    int wmax = 0;
    for(unsigned int i = 0; i < vectEMin.size(); i++) {
        double d = MAX(Euclidean::max(vectEMax), Euclidean::max(vectEMin));

        wmax = (int)ceil(d);
        if(wmax % 2 == 0) {
            wmax++;
        }
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);
#endif

    CImg<float> imgSource(inputImg.channel(0));

#ifdef DEBUG
    fprintf(stdout, "%-40s", "Order the filters...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Order filters with source image
    std::vector<float> vectFilterMax, vectFilterMin;

    for(int unsigned i = 0; i < vectEMax.size(); i++) {
        float max = 0;
        for (int k = vectEMax[i].getX() - ((wmax - 1) / 2); k < vectEMax[i].getX() + ((wmax + 1) / 2); k++) {
            for (int l = vectEMax[i].getY() - ((wmax - 1) / 2); l < vectEMax[i].getY() + ((wmax + 1) / 2); l++) {
                if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                    if (imgSource(k, l) > max) {
                        max = imgSource(k, l);
                    }
                }
            }
        }
        vectFilterMax.push_back(max);
    }

    for(int unsigned i = 0; i < vectEMin.size(); i++) {
        float min = 255;
        for (int k = vectEMin[i].getX() - ((wmax - 1) / 2); k < vectEMin[i].getX() + ((wmax + 1) / 2); k++) {
            for (int l = vectEMin[i].getY() - ((wmax - 1) / 2); l < vectEMin[i].getY() + ((wmax + 1) / 2); l++) {
                if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                    if (imgSource(k, l) < min) {
                        min = imgSource(k, l);
                    }
                }
            }
        }
        vectFilterMin.push_back(min);
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);
#endif

    CImg<float> newImgMax(imgMax.width(), imgMax.height());

#ifdef DEBUG
    fprintf(stdout, "%-40s", "Calculate the upper envelope...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Calculate the upper envelope
    for(int unsigned i = 0; i < vectEMax.size(); i++) {
        for (int k = vectEMax[i].getX() - ((wmax - 1) / 2); k < vectEMax[i].getX() + ((wmax + 1) / 2); k++) {
            for (int l = vectEMax[i].getY() - ((wmax - 1) / 2); l < vectEMax[i].getY() + ((wmax + 1) / 2); l++) {
                if ((k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height())) {
                    if( imgMax(k, l) == 0 ) {
                        imgMax(k, l) = vectFilterMax[i];
                    }
                    else {
                        imgMax(k, l) = (int)((imgMax(k, l) + vectFilterMax[i]) / 2);
                    }
                }
            }
        }
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);

    fprintf(stdout, "%-40s", "Smooth the upper envelope...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Smooth of the upper envelope
    for (int k = 0; k < imgSource.width(); k++) {
        for (int l = 0; l < imgSource.height(); l++) {
            if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                newImgMax(k, l) = (int)sum(imgMax, k, l, wmax) / (wmax * wmax);
            }
        }
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);
#endif

    CImg<float> newImgMin(imgMin.width(), imgMin.height());

#ifdef DEBUG
    fprintf(stdout, "%-40s", "Calculate the lower envelope...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Calculate the lower envelope
    for(int unsigned i = 0; i < vectEMin.size(); i++) {
        for (int k = vectEMin[i].getX() - ((wmax - 1) / 2); k < vectEMin[i].getX() + ((wmax + 1) / 2); k++) {
            for (int l = vectEMin[i].getY() - ((wmax - 1) / 2); l < vectEMin[i].getY() + ((wmax + 1) / 2); l++) {
                if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                    if( imgMin(k, l) == 0 ) {
                        imgMin(k, l) = vectFilterMin[i];
                    }
                    else {
                        imgMin(k, l) = (int)((imgMin(k, l) + vectFilterMin[i]) / 2);
                    }
                }
            }
        }
    }

#ifdef DEBUG
    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);

    fprintf(stdout, "%-40s", "Smooth the lower envelope...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Smooth of the lower envelope
    for (int k = 0; k < imgSource.width(); k++) {
        for (int l = 0; l < imgSource.height(); l++) {
            if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                newImgMin(k, l) = (int)sum(imgMin, k, l, wmax) / (wmax * wmax);
            }
        }
    }

#ifdef DEBUG
    // Display images for max and min
    CImgDisplay dispEMax(imgMax,"Envelope Max");
    CImgDisplay dispEMin(imgMin,"Envelope Min");
    CImgDisplay dispSMax(newImgMax,"Smooth Max");
    CImgDisplay dispSMin(newImgMin,"Smooth Min");

    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);
#endif

    ///////////////////////////////////////////////////////////////////////////////
    //                       Part 2: Average                                     //
    ///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    fprintf(stdout, "%-40s", "Average...");

    gettimeofday(&tim, NULL);
    t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
#endif

    // Calculate the Average
    CImg<float> imgMoyenne(inputImg.width(), inputImg.height());

    for (int i = 0; i < inputImg.width(); i++) {
        for (int j = 0; j < inputImg.height(); j++) {
            imgMoyenne(i, j) = (newImgMin(i, j) + newImgMax(i, j)) /2;
        }
    }

#ifdef DEBUG
    CImgDisplay dispMoyenne(imgMoyenne, "Average");

    gettimeofday(&tim, NULL);
    t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
    printf("%.6f seconds\n", t2 - t1);
#endif

    ///////////////////////////////////////////////////////////////////////////////
    //                         Partie 3: Deletion                                //
    ///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
    printf("Deletion..\n");
#endif

    return inputImg - imgMoyenne;
}

/*******************************************************************************
  Main
 *******************************************************************************/
int main()
{
    char buffer [50];
    CImg<float> inputImg("lena.bmp");
    CImgDisplay dispBase(inputImg, "Source Image");

    // 1st decomposition
    CImg<float> imgMode1 = decompose(inputImg);
    CImgDisplay dispMode1(imgMode1, "Mode 1");

    CImg<float> imgMode2 = decompose(inputImg - imgMode1);
    CImgDisplay dispMode2(imgMode2, "Mode 2");

    /*
    for (int i = 2; i < 3; i++) {
        sprintf(buffer, "Mode %d", i);
        fprintf(stdout, "Decomposing %s\n", buffer);

        inputImg = inputImg - imgMode;
        imgMode  = decompose(inputImg);

        CImgDisplay dispModeBis(imgMode, buffer);
    }
    */

    printf("End.\n");

    while (!dispBase.is_closed()) {
        dispBase.wait();
    }

    return 0;
}
