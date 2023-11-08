/*
 *    Copyright (C) 2023 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
	\brief
	@author authorname
*/



#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#include <genericworker.h>
#include <abstract_graphic_viewer/abstract_graphic_viewer.h>
#include <ranges>

class SpecificWorker : public GenericWorker
{
Q_OBJECT
public:
    SpecificWorker(TuplePrx tprx, bool startup_check);
    ~SpecificWorker();
    bool setParams(RoboCompCommonBehavior::ParameterList params);



public slots:
    void compute();
    int startup_check();
    void initialize(int period);


private:
    bool startup_check_flag;
    AbstractGraphicViewer *viewer;
    void draw_lidar(const RoboCompLidar3D::TPoints &points, AbstractGraphicViewer *viewer);
    tuple<RoboCompLidar3D::TPoints, RoboCompLidar3D::TPoints,RoboCompLidar3D::TPoints> DetectarPuertas(const RoboCompLidar3D::TPoints &points);
    double alturaZ1_MIN = 400;
    double alturaZ1_MAX = 600;
    double alturaZ2_MIN = 700;
    double alturaZ2_MAX = 900;
    double alturaZ3_MIN = 1100;
    double alturaZ3_MAX = 1300;
};

#endif