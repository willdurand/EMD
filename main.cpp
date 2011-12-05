////////////////////////////////////////////////////////////////////////////////
//                      Empirical Mode Decomposition                          //
// BERNARD Guillaume                                                          //
// DURAND William                                                             //
// ZZ3F2 ISIMA                                                                //
////////////////////////////////////////////////////////////////////////////////

#include "CImg.h"
#include <math.h>
#include <vector>

#include "Euclidean.hpp"

using namespace cimg_library;

/*******************************************************************************
  Main
 *******************************************************************************/
int main()
{
    CImg<unsigned char> imgLena("lena.bmp");

    CImgDisplay dispBase(imgLena,"Image de base");

    std::vector<Euclidean> vectEMax, vectEMin;

    ///////////////////////////////////////////////////////////////////////////////
    //                        Part 1: Finding minimas and maximas                //
    ///////////////////////////////////////////////////////////////////////////////
    CImg<unsigned char> imgMax = imgLena.channel(0);
    CImg<unsigned char> imgMin = imgLena.channel(0);

    imgMax.print();

    for (int i = 0; i<imgLena.width() ; i+=3) {
        for (int j = 0; j<imgLena.height() ; j+=3) {

            // Save max and min locations
            int xmax = i;
            int ymax = j;
            int xmin = i;
            int ymin = j;

            // save values
            unsigned char max = imgMax(i,j);
            unsigned char min = imgMin(i,j);

            Euclidean eMax(i, j);
            Euclidean eMin(i, j);

            // 3x3
            for (int k = i; k<i+3 ; k++) {
                for (int l = j; l<j+3 ; l++) {

                    // Max
                    if ((imgMax(k,l) <= max)&&(l!=ymax &&k!=xmax)) {
                        imgMax(k,l) = 0;
                    } else {
                        max = imgMax(k,l);
                        imgMax(xmax,ymax) = 0;
                        xmax = k;
                        ymax = l;

                        eMax.setX(k);
                        eMax.setY(l);
                    }

                    // Min
                    if ((imgMin(k,l) >= min)&&(l!=ymin &&k!=xmin)) {
                        imgMin(k,l) = 0;
                    } else {
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

    // Array of Euclidean distance to the nearest non zero element
    std::vector<Euclidean>::iterator it1, it2;

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

    // Calculate the window size

    // Order filters with source image

    // Calculate the upper envelope

    // Calculate the lower envelope

    // Display images for max and min
    CImgDisplay dispMax(imgMax,"Image de Max");
    CImgDisplay dispMin(imgMin,"Image de Min");

    ///////////////////////////////////////////////////////////////////////////////
    //                        Part 2: Average                                    //
    ///////////////////////////////////////////////////////////////////////////////

    // Calculate the Average
    CImg<unsigned char> imgMoyenne = (imgMax+imgMin)/2;
    CImgDisplay dispMoyenne(imgMoyenne,"Image Moyenne");

    ///////////////////////////////////////////////////////////////////////////////
    //                        Partie 3: Deletion                                 //
    ///////////////////////////////////////////////////////////////////////////////

    CImg<unsigned char> imgFin = imgLena - imgMoyenne;
    CImgDisplay dispFin(imgFin,"Image Finale");

    while (!dispBase.is_closed()) {
        dispBase.wait();
    }

    return 0;
}
