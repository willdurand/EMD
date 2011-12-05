#include "Euclidean.hpp"

double Euclidean::computeDistanceFrom(const Euclidean & euclidian)
{
    double diffx = _x - euclidian.getX();
    double diffy = _y - euclidian.getY();
    double diffx_sqr = pow(diffx, 2);
    double diffy_sqr = pow(diffy, 2);

    return sqrt(diffx_sqr + diffy_sqr);
}
