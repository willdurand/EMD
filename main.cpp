////////////////////////////////////////////////////////////////////////////////
//                      DM : Empirical Mode Decomposition                     //
// BERNARD Guillaume                                                          //
// DURAND William                                                             //
// ZZ3F2 ISIMA                                                                //
////////////////////////////////////////////////////////////////////////////////

#include "CImg.h"
#include <math.h>

using namespace cimg_library;

/*******************************************************************************

                                    Main

*******************************************************************************/
int main()
{
    // Ouverture de l'image
    CImg<unsigned char> imgLena("lena.bmp");

    // Affichage de l'image de base
    CImgDisplay dispBase(imgLena,"Image de base");
    
///////////////////////////////////////////////////////////////////////////////
//                        Partie 1: Cherche les extremums                    //
///////////////////////////////////////////////////////////////////////////////
    CImg<unsigned char> imgMax = imgLena;
    CImg<unsigned char> imgMin = imgLena;
    
    // Parcours de l'image
    for (int i = 0; i<imgLena.width() ; i+=3) {
        for (int j = 0; j<imgLena.height() ; j+=3) {
            
            // Sauvegarde de la place du max et du min
            int xmax = i;
            int ymax = j;
            int xmin = i;
            int ymin = j;
            
            // Sauvegarde des valeurs 
            unsigned char max = imgMax(i,j);
            unsigned char min = imgMin(i,j);
            
            // Parcours en 3x3
            for (int k = i; k<i+3 ; k++) {
                for (int l = j; l<j+3 ; l++) {
                   
                    // Recherche du max
                    if ((imgMax(k,l) <= max)&&(l!=ymax &&k!=xmax)) {
                        imgMax(k,l) = 0;
                    }
                    else
                    {
                        max = imgMax(k,l);
                        imgMax(xmax,ymax) = 0;
                        xmax = k;
                        ymax = l;
                    }
                    
                    // Recherche du min
                    if ((imgMin(k,l) >= min)&&(l!=ymin &&k!=xmin)) {
                        imgMin(k,l) = 0;
                    }
                    else
                    {
                        min = imgMin(k,l);
                        imgMin(xmin,ymin) = 0;
                        xmin = k;
                        ymin = l;
                    }
                    
                }
            }
            
        }
    }
    
    // Affichage de l'image de max et min
    CImgDisplay dispMax(imgMax,"Image de Max");
    CImgDisplay dispMin(imgMin,"Image de Min");
    
///////////////////////////////////////////////////////////////////////////////
//                        Partie 2: Moyenne                                  //
///////////////////////////////////////////////////////////////////////////////

    CImg<unsigned char> imgMoyenne = (imgMax+imgMin)/2;
    
    CImgDisplay dispMoyenne(imgMoyenne,"Image Moyenne");
    
///////////////////////////////////////////////////////////////////////////////
//                        Partie 3: Suppression                              //
///////////////////////////////////////////////////////////////////////////////
    
    CImg<unsigned char> imgFin = imgLena - imgMoyenne;
    
    CImgDisplay dispFin(imgFin,"Image Finale");
    
    while (!dispBase.is_closed())
    {
        dispBase.wait();
    }
    return 0;
}
