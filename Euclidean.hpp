#ifndef __EUCLIDIAN_HPP__
#define __EUCLIDIAN_HPP__

#include <math.h>
#include <vector>

/**
 * Euclidean class.
 *
 * @author William Durand <william.durand1@gmail.com>
 * @author Guillaume Bernard <guill.bernard1@gmail.com>
 */
class Euclidean
{
    public:
        /**
         * Constructor
         *
         * @param const int x   Coordinate X
         * @param const int y   Coordinate Y
         */
        Euclidean(const int x, const int y) : _x(x), _y(y) {}

        /**
         * @param const int x   Coordinate X
         */
        void setX(const int x) { _x = x; }

        /**
         * @return const int
         */
        int getX() const { return _x; }

        /**
         * @param const int y   Coordinate Y
         */
        void setY(const int y) { _y = y; }

        /**
         * @return const int
         */
        int getY() const { return _y; }

        /**
         * @param const double distance    A distance.
         */
        void setDistance(const double distance) { _distance = distance; }

        /**
         * @return const double
         */
        double getDistance() const { return _distance; }

        /**
         * @param Euclidean & nearest
         */
        void setNearest(Euclidean & nearest) { _nearest = &nearest; }

        /**
         * @return const Euclidean*
         */
        Euclidean* getNearest() const { return _nearest; }

        /**
         * @param const Euclidean & euclidean
         * @return const double
         */
        double computeDistanceFrom(const Euclidean & euclidean) const;

        /**
         * @param const std::vector<Euclidean>
         * @return double
         */
        static double max(const std::vector<Euclidean> vect);

        /**
         * @param const std::vector<Euclidean>
         * @return double
         */
        static double min(const std::vector<Euclidean> vect);

    private:

        int _x;

        int _y;

        double _distance;

        Euclidean *_nearest;
};

#endif
