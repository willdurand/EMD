#ifndef __EUCLIDIAN_HPP__
#define __EUCLIDIAN_HPP__

#include <math.h>

class Euclidean
{
    public:
        Euclidean(const int x, const int y) : _x(x), _y(y) {}

        void setX(const int x) { _x = x; }

        int getX() const { return _x; }

        void setY(const int y) { _y = y; }

        int getY() const { return _y; }

        void setDistance(const double distance) { _distance = distance; }

        double getDistance() const { return _distance; }

        void setNearest(Euclidean & nearest) { _nearest = &nearest; }

        Euclidean* getNearest() const { return _nearest; }

        double computeDistanceFrom(const Euclidean & euclidian);

        static double max(const std::vector<Euclidean> vect);

        static double min(const std::vector<Euclidean> vect);

    private:

        int _x;

        int _y;

        double _distance;

        Euclidean *_nearest;
};

#endif
