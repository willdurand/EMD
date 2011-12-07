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

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

using namespace cimg_library;

/*
   double min(std::vector<Euclidean> vect) {
   double min = (*vect.begin()).getDistance();
   std::vector<Euclidean>::iterator it;

   for (it = vect.begin() + 1; it != vect.end(); it++) {
   if ((*it).getDistance() < min) {
   min = (*it).getDistance();
   }
   }

   return min;
   }

   double max(std::vector<Euclidean> vect) {
   double max = (*vect.begin()).getDistance();
   std::vector<Euclidean>::iterator it;

   for (it = vect.begin() + 1; it != vect.end(); it++) {
   if ((*it).getDistance() > max) {
   max = (*it).getDistance();
   }
   }

   return max;
   }
*/

/*******************************************************************************
  Main
 *******************************************************************************/
int main()
{
    CImg<unsigned char> imgLena("lena.bmp");

    CImgDisplay dispBase(imgLena,"Image de base");

    std::vector<Euclidean> vectEMax, vectEMin;
    std::vector<int> w;

    ///////////////////////////////////////////////////////////////////////////////
    //                        Part 1: Finding minimas and maximas                //
    ///////////////////////////////////////////////////////////////////////////////
    CImg<unsigned char> imgMax = imgLena.channel(0);
    CImg<unsigned char> imgMin = imgLena.channel(0);

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

    // Calculate the windows sizes
    for(unsigned int i = 0; i < vectEMin.size(); i++) {
        double d1 = MIN(vectEMax[i].getDistance(), vectEMin[i].getDistance());
        double d2 = MAX(vectEMax[i].getDistance(), vectEMin[i].getDistance());
        double d3 = MIN(vectEMax[i].getDistance(), vectEMin[i].getDistance());
        double d4 = MAX(vectEMax[i].getDistance(), vectEMin[i].getDistance());

        int wi = (int)ceil(MIN(MIN(d1, d2), MIN(d3, d4)));
        wi = wi % 2 ? wi + 1 : wi;
        w.push_back(wi);
    }

    CImg<unsigned char> imgSource = imgLena.channel(0);

    // Order filters with source image
    std::vector<unsigned char> vectFilterMax, vectFilterMin;

    for(unsigned int i = 0; i < vectEMax.size(); i++) {
        unsigned char max = 0;
        for (int k = vectEMax[i].getX() - ((w[i] - 1) / 2); k < vectEMax[i].getX() + ((w[i] + 1) / 2); k++) {
            for (int l = vectEMax[i].getY() - ((w[i] - 1) / 2); l < vectEMax[i].getY() + ((w[i] + 1) / 2); l++) {
                if (imgSource(k, l) > max) {
                    max = imgSource(k, l);
                }
            }
        }
        vectFilterMax.push_back(max);
    }

    for(unsigned int i = 0; i < vectEMin.size(); i++) {
        unsigned char min = 199;
        for (int k = vectEMin[i].getX() - ((w[i] - 1) / 2); k < vectEMin[i].getX() + ((w[i] + 1) / 2); k++) {
            for (int l = vectEMin[i].getY() - ((w[i] - 1) / 2); l < vectEMin[i].getY() + ((w[i] + 1) / 2); l++) {
                if (imgSource(k, l) < min) {
                    min = imgSource(k, l);
                }
            }
        }
        vectFilterMin.push_back(min);
    }

	CImg<unsigned char> newImgMax(imgMax.width(), imgMax.height());

    // Calculate the upper envelope
	for(unsigned int i = 0; i < vectEMax.size(); i++) {
        for (int k = vectEMax[i].getX() - ((w[i] - 1) / 2); k < vectEMax[i].getX() + ((w[i] + 1) / 2); k++) {
            for (int l = vectEMax[i].getY() - ((w[i] - 1) / 2); l < vectEMax[i].getY() + ((w[i] + 1) / 2); l++) {
                if( (k == vectEMax[i].getX() && l == vectEMax[i].getY()) || imgMax(k, l) == 0 ) {
					newImgMax(k, l) = vectFilterMax[i];
				}
				else {
					newImgMax(k, l) = (imgMax(k, l) + vectFilterMax[i]) / 2;
				}
            }
        }
    }

	CImg<unsigned char> newImgMin(imgMin.width(), imgMin.height());	

    // Calculate the lower envelope
	for(unsigned int i = 0; i < vectEMin.size(); i++) {
        for (int k = vectEMin[i].getX() - ((w[i] - 1) / 2); k < vectEMin[i].getX() + ((w[i] + 1) / 2); k++) {
            for (int l = vectEMin[i].getY() - ((w[i] - 1) / 2); l < vectEMin[i].getY() + ((w[i] + 1) / 2); l++) {
                if( (k == vectEMin[i].getX() && l == vectEMin[i].getY()) || imgMin(k, l) == 0 ) {
					newImgMin(k, l) = vectFilterMin[i];
				}
				else {
					newImgMin(k, l) = (imgMin(k, l) + vectFilterMin[i]) / 2;
				}
            }
        }
    }

    // Display images for max and min
    CImgDisplay dispMax(newImgMax,"Image de Max");
    CImgDisplay dispMin(newImgMin,"Image de Min");

    ///////////////////////////////////////////////////////////////////////////////
    //                        Part 2: Average                                    //
    ///////////////////////////////////////////////////////////////////////////////

    // Calculate the Average
    CImg<unsigned char> imgMoyenne = (newImgMax+newImgMin)/2;
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
