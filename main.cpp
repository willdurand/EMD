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

/*******************************************************************************
  Main
 *******************************************************************************/
int main()
{
#ifdef DEBUG
    CImg<float> inputImg(8, 8, 1, 3);
    int tab[][8] = {
        { 8, 8, 4, 1, 5, 2, 6, 3 },
        { 6, 3, 2, 3, 7, 3, 9, 3 },
        { 7, 8, 3, 2, 1, 4, 3, 7 },
        { 4, 1, 2, 4, 3, 5, 7, 8 },
        { 6, 4, 2, 1, 2, 5, 3, 4 },
        { 1, 3, 7, 9, 9, 8, 7, 8 },
        { 9, 2, 6, 7, 6, 8, 7, 7 },
        { 8, 2, 1, 9, 7, 9, 1, 1 }
    };

    printf("Base:\n");
    for (int i = 0; i < inputImg.width(); i++) {
        for (int j = 0; j < inputImg.height(); j++) {
            inputImg(i, j) = tab[i][j];
            printf("%d ", inputImg(i, j));
        }
        printf("\n");
    }
#else
    CImg<float> inputImg("lena.bmp");
    CImgDisplay dispBase(inputImg,"Source Image");
#endif

    std::vector<Euclidean> vectEMax, vectEMin;

    ///////////////////////////////////////////////////////////////////////////////
    //              Part 1: Finding minimas and maximas                          //
    ///////////////////////////////////////////////////////////////////////////////

    CImg<float> imgMax(inputImg.channel(0));
    CImg<float> imgMin(inputImg.channel(0));

    printf("  Calculate the extremas..\n");
    for (int i = 0; i < inputImg.width(); i += SIZE) {
        for (int j = 0; j < inputImg.height(); j += SIZE) {

            // Save max and min locations
            int xmax = i;
            int ymax = j;
            int xmin = i;
            int ymin = j;

            // save values
            float max = imgMax(i,j);
            float min = imgMin(i,j);

            Euclidean eMax(i, j);
            Euclidean eMin(i, j);

            // SIZExSIZE
            for (int k = i; k<i+SIZE ; k++) {
                for (int l = j; l<j+SIZE ; l++) {

                    // Max
                    if ((imgMax(k,l) <= max) && (l!=ymax || k!=xmax)) {
                        imgMax(k,l) = 0;
                    } else if (l!=ymax || k!=xmax) {
                        max = imgMax(k,l);
                        imgMax(xmax,ymax) = 0;
                        xmax = k;
                        ymax = l;

                        eMax.setX(k);
                        eMax.setY(l);
                    }

                    // Min
                    if ((imgMin(k,l) >= min) && (l!=ymin || k!=xmin)) {
                        imgMin(k,l) = 0;
                    } else if (l!=ymax || k!=xmax) {
                        min = imgMin(k,l);
                        imgMin(xmin,ymin) = 0;
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
    printf("Extremas:\n");

    printf("Max\n");
    for (int i = 0; i < imgMax.width(); i++) {
        for (int j = 0; j < imgMax.height(); j++) {
            printf("%d ", imgMax(i, j));
        }
        printf("\n");
    }

    printf("Min\n");
    for (int i = 0; i < imgMin.width(); i++) {
        for (int j = 0; j < imgMin.height(); j++) {
            printf("%d ", imgMin(i, j));
        }
        printf("\n");
    }
#endif

    // Array of Euclidean distance to the nearest non zero element
    std::vector<Euclidean>::iterator it1, it2;

    printf("  Calculate the Euclidean distances..\n");
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

    int wmax = 0;
    printf("  Calculate the window size..\n");
    // Calculate the window size
    for(unsigned int i = 0; i < vectEMin.size(); i++) {
        double d = MAX(Euclidean::max(vectEMax), Euclidean::max(vectEMin));

        wmax = (int)ceil(d);
        if(wmax % 2 == 0) {
            wmax++;
        }
    }

    CImg<float> imgSource(inputImg.channel(0));

    printf("  Order the filters..\n");
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

    CImg<float> newImgMax(imgMax.width(), imgMax.height());

    printf("  Calculate the upper envelope..\n");
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
    printf("Envelopes:\n");

    printf("Max\n");
    for (int i = 0; i < newImgMax.width(); i++) {
        for (int j = 0; j < newImgMax.height(); j++) {
            printf("%d ", newImgMax(i, j));
        }
        printf("\n");
    }
#endif

    printf("  Smooth the upper envelope..\n");
    // Smooth of the upper envelope
    for (int k = 0; k < imgSource.width(); k++) {
        for (int l = 0; l < imgSource.height(); l++) {
            if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                newImgMax(k, l) = (int)sum(imgMax, k, l, wmax) / (wmax * wmax);
            }
        }
    }

    CImg<float> newImgMin(imgMin.width(), imgMin.height());

    printf("  Calculate the lower envelope..\n");
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
    printf("Min\n");
    for (int i = 0; i < newImgMin.width(); i++) {
        for (int j = 0; j < newImgMin.height(); j++) {
            printf("%d ", newImgMin(i, j));
        }
        printf("\n");
    }
#endif

    printf("  Smooth the lower envelope..\n");
    // Smooth of the lower envelope
    for (int k = 0; k < imgSource.width(); k++) {
        for (int l = 0; l < imgSource.height(); l++) {
            if( (k >= 0 && k < imgSource.width()) && (l >= 0  && l < imgSource.height()) ) {
                newImgMin(k, l) = (int)sum(imgMin, k, l, wmax) / (wmax * wmax);
            }
        }
    }

#ifdef DEBUG
    printf("Smoothed envelopes:\n");

    printf("Max\n");
    for (int i = 0; i < newImgMax.width(); i++) {
        for (int j = 0; j < newImgMax.height(); j++) {
            printf("%d ", newImgMax(i, j));
        }
        printf("\n");
    }

    printf("Min\n");
    for (int i = 0; i < newImgMin.width(); i++) {
        for (int j = 0; j < newImgMin.height(); j++) {
            printf("%d ", newImgMin(i, j));
        }
        printf("\n");
    }
#else
    // Display images for max and min
    //CImgDisplay dispEMax(imgMax,"Envelope Max");
    //CImgDisplay dispEMin(imgMin,"Envelope Min");
    //CImgDisplay dispSMax(newImgMax,"Smooth Max");
    //CImgDisplay dispSMin(newImgMin,"Smooth Min");
#endif

    ///////////////////////////////////////////////////////////////////////////////
    //                       Part 2: Average                                     //
    ///////////////////////////////////////////////////////////////////////////////

    printf("  Average..\n");
    // Calculate the Average
    CImg<float> imgMoyenne(inputImg.width(), inputImg.height());

    for (int i = 0; i < inputImg.width(); i++) {
        for (int j = 0; j < inputImg.height(); j++) {
            imgMoyenne(i, j) = (newImgMin(i, j) + newImgMax(i, j)) /2;
        }
    }

#ifdef DEBUG
    printf("Average:\n");

    for (int i = 0; i < imgMoyenne.width(); i++) {
        for (int j = 0; j < imgMoyenne.height(); j++) {
            printf("%d ", imgMoyenne(i, j));
        }
        printf("\n");
    }
#else
    //CImgDisplay dispMoyenne(imgMoyenne, "Average");
#endif

    ///////////////////////////////////////////////////////////////////////////////
    //                         Partie 3: Deletion                                //
    ///////////////////////////////////////////////////////////////////////////////


#ifndef DEBUG
    printf("  Deletion..\n");
    CImg<float> imgMode(inputImg - imgMoyenne);
    CImgDisplay dispMode(imgMode, "Mode 1");
#endif

#ifndef DEBUG
    printf("End\n");
    while (!dispBase.is_closed()) {
        dispBase.wait();
    }
#endif

    return 0;
}
