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

#define MAX_ITERATIONS 3

// Variance delta
#define DELTA 50

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
    ///////////////////////////////////////////////////////////////////////////////
    //              Part 1: Finding minimas and maximas                          //
    ///////////////////////////////////////////////////////////////////////////////

    std::vector<Euclidean> vectEMax, vectEMin;
    CImg<float> imgMax(input.width(), input.height());
    CImg<float> imgMin(input.width(), input.height());
    for (int i = 0; i < input.width(); i += SIZE)
        for (int j = 0; j < input.height(); j += SIZE)
        {
            // Save max and min locations
            int xmin=i, xmax=i, ymin=j, ymax=j;
            float min = input(i,j), max = input(i,j);
            imgMax(i,j) = input(i,j);
            imgMin(i,j) = input(i,j);

            // SIZExSIZE
            for (int k = i; k < i + SIZE; k++)
                for (int l = j; l < j + SIZE; l++)
                {
                    // Max
                    if ((input(k, l) > max) && (l != ymax || k != xmax))
                    {
                        imgMax(xmax, ymax) = 0;
                        max = input(k, l);
                        imgMax(k,l) = max;
                        xmax = k;
                        ymax = l;
                    }

                    // Min
                    if ((imgMin(k, l) < min) && (l != ymin || k != xmin))
                    {
                        imgMax(xmax, ymax) = 0;
                        min = imgMin(k, l);
                        imgMax(k,l) = max;
                        xmin = k;
                        ymin = l;
                    }
                }

            vectEMax.push_back(Euclidean(xmax,ymax));
            vectEMin.push_back(Euclidean(xmin,ymin));
        }

    // Array of Euclidean distance to the nearest non zero element
    std::vector<Euclidean>::iterator it1, it2;

    for (it1 = vectEMax.begin(); it1 != vectEMax.end(); it1++)
        for (it2 = it1 + 1; it2 != vectEMax.end(); it2++)
        {
            double dist = (*it1).computeDistanceFrom(*it2);

            if ((*it1).getDistance() == 0 || dist < (*it1).getDistance())
            {
                (*it1).setDistance(dist);
                (*it1).setNearest(*it2);
            }

            if ((*it2).getDistance() == 0 || dist < (*it2).getDistance())
            {
                (*it2).setDistance(dist);
                (*it2).setNearest(*it1);
            }
        }

    for (it1 = vectEMin.begin(); it1 != vectEMin.end(); it1++)
        for (it2 = it1 + 1; it2 != vectEMin.end(); it2++)
        {
            double dist = (*it1).computeDistanceFrom(*it2);

            if ((*it1).getDistance() == 0 || dist < (*it1).getDistance())
            {
                (*it1).setDistance(dist);
                (*it1).setNearest(*it2);
            }

            if ((*it2).getDistance() == 0 || dist < (*it2).getDistance())
            {
                (*it2).setDistance(dist);
                (*it2).setNearest(*it1);
            }
        }

    // Calculate the window size
    double d = MAX(Euclidean::max(vectEMax), Euclidean::max(vectEMin));
    int wmax = 2*((int)d/2)+1;

    // Order filters with source image
    std::vector<float> vectFilterMax, vectFilterMin;
    for(int unsigned i = 0; i < vectEMax.size(); i++)
    {
        float max = 0;
        for (int k = vectEMax[i].getX() - ((wmax - 1) / 2); k < vectEMax[i].getX() + ((wmax + 1) / 2); k++)
        {
            for (int l = vectEMax[i].getY() - ((wmax - 1) / 2); l < vectEMax[i].getY() + ((wmax + 1) / 2); l++)
            {
                if( (k >= 0 && k < input.width()) && (l >= 0  && l < input.height()) )
                {
                    if (input(k, l) > max)
                    {
                        max = input(k, l);
                    }
                }
            }
        }
        vectFilterMax.push_back(max);
    }

    for(int unsigned i = 0; i < vectEMin.size(); i++)
    {
        float min = 255;
        for (int k = vectEMin[i].getX() - ((wmax - 1) / 2); k <= vectEMin[i].getX() + ((wmax + 1) / 2); k++)
        {
            for (int l = vectEMin[i].getY() - ((wmax - 1) / 2); l < vectEMin[i].getY() + ((wmax + 1) / 2); l++)
            {
                if( (k >= 0 && k < input.width()) && (l >= 0  && l < input.height()) )
                {
                    if (input(k, l) < min) {
                        min = input(k, l);
                    }
                }
            }
        }
        vectFilterMin.push_back(min);
    }

    // Calculate the upper envelope
    CImg<float> newImgMax(imgMax.width(), imgMax.height());
    for(int unsigned i = 0; i < vectEMax.size(); i++)
    {
        for (int k = vectEMax[i].getX() - ((wmax - 1) / 2); k < vectEMax[i].getX() + ((wmax + 1) / 2); k++)
        {
            for (int l = vectEMax[i].getY() - ((wmax - 1) / 2); l < vectEMax[i].getY() + ((wmax + 1) / 2); l++)
            {
                if ((k >= 0 && k < input.width()) && (l >= 0  && l < input.height()))
                {
                    if (imgMax(k, l) == 0)
                        imgMax(k, l) = vectFilterMax[i];
                    else
                        imgMax(k, l) = (int)((imgMax(k, l) + vectFilterMax[i]) / 2);
                }
            }
        }
    }

    // Smooth of the upper envelope
    for (int k = 0; k < input.width(); k++)
    {
        for (int l = 0; l < input.height(); l++) {
            if( (k >= 0 && k < input.width()) && (l >= 0  && l < input.height()) )
            {
                newImgMax(k, l) = (int)sum(imgMax, k, l, wmax) / (wmax * wmax);
            }
        }
    }

    // Calculate the lower envelope
    CImg<float> newImgMin(imgMin.width(), imgMin.height());
    for(int unsigned i = 0; i < vectEMin.size(); i++) {
        for (int k = vectEMin[i].getX() - ((wmax - 1) / 2); k < vectEMin[i].getX() + ((wmax + 1) / 2); k++)
        {
            for (int l = vectEMin[i].getY() - ((wmax - 1) / 2); l < vectEMin[i].getY() + ((wmax + 1) / 2); l++)
            {
                if( (k >= 0 && k < input.width()) && (l >= 0  && l < input.height()) )
                {
                    if( imgMin(k, l) == 0 )
                        imgMin(k, l) = vectFilterMin[i];
                    else
                        imgMin(k, l) = (int)((imgMin(k, l) + vectFilterMin[i]) / 2);
                }
            }
        }
    }

    // Smooth of the lower envelope
    for (int k = 0; k < input.width(); k++)
    {
        for (int l = 0; l < input.height(); l++)
        {
            if( (k >= 0 && k < input.width()) && (l >= 0  && l < input.height()) )
            {
                newImgMin(k, l) = (int)sum(imgMin, k, l, wmax) / (wmax * wmax);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    //                       Part 2: Average                                     //
    ///////////////////////////////////////////////////////////////////////////////

    // Calculate the Average
    CImg<float> imgMoyenne(input.width(), input.height());
    for (int i = 0; i < input.width(); i++)
        for (int j = 0; j < input.height(); j++)
            imgMoyenne(i, j) = (newImgMin(i, j) + newImgMax(i, j)) /2;

    ///////////////////////////////////////////////////////////////////////////////
    //                         Partie 3: Deletion                                //
    ///////////////////////////////////////////////////////////////////////////////

    return input - imgMoyenne;
}

/*******************************************************************************
  Main
 *******************************************************************************/
int main(int argc, char **argv)
{
    char modeTitle[30], residueTitle[50];
    double variance = 1000000;

    if (argc != 2) {
        std::cout << "Usage: ./emd <image>" << std::endl;
        return 1;
    }

    CImgDisplay disp[MAX_ITERATIONS * 2 + 1];

    CImg<float> inputImg(argv[1]), imgMode;
    disp[0].assign(inputImg, "Source Image");

    for (int i = 1; i < MAX_ITERATIONS + 1; i++) {
        sprintf(modeTitle, "BEMC-%d", i);
        std::cout << "Decomposing " << modeTitle << std::endl;

        // Process
        imgMode = decompose(inputImg);
        inputImg = inputImg - imgMode;

        // Display BEMC i
        disp[i].assign(imgMode, modeTitle);

        // Display residue
        sprintf(residueTitle, "Residue %s", modeTitle);
        disp[MAX_ITERATIONS + i].assign(inputImg, residueTitle);

        // Get variance
        std::cout << "Variance: " << inputImg.variance() << std::endl;

        if (fabs(variance - inputImg.variance()) < DELTA) {
            std::cout << "Ended at iteration " << i << std::endl;
            break;
        } else {
            variance = inputImg.variance();
        }
    }

    while (!disp[0].is_closed()) {
        disp[0].wait();
    }

    return 0;
}
