#include "Euclidean.hpp"

double Euclidean::computeDistanceFrom(const Euclidean & euclidean) const
{
    double diffx = _x - euclidean.getX();
    double diffy = _y - euclidean.getY();
    double diffx_sqr = pow(diffx, 2);
    double diffy_sqr = pow(diffy, 2);

    return sqrt(diffx_sqr + diffy_sqr);
}

double Euclidean::max(const std::vector<Euclidean> vect)
{
    double max = (*vect.begin()).getDistance();
    std::vector<Euclidean>::const_iterator it;

    for (it = vect.begin() + 1; it != vect.end(); it++) {
        if ((*it).getDistance() > max) {
            max = (*it).getDistance();
        }
    }

    return max;
}

double Euclidean::min(const std::vector<Euclidean> vect)
{
    double min = (*vect.begin()).getDistance();
    std::vector<Euclidean>::const_iterator it;

    for (it = vect.begin() + 1; it != vect.end(); it++) {
        if ((*it).getDistance() < min) {
            min = (*it).getDistance();
        }
    }

    return min;
}
