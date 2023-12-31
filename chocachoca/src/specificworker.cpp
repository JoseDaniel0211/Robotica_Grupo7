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
#include "specificworker.h"

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
    this->startup_check_flag = startup_check;
    // Uncomment if there's too many debug messages
    // but it removes the possibility to see the messages
    // shown in the console with qDebug()
//	QLoggingCategory::setFilterRules("*.debug=false\n");
}
/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
    std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
//	THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = std::make_shared(innermodel_path);
//	}
//	catch(const std::exception &e) { qFatal("Error reading config params"); }






    return true;
}

void SpecificWorker::initialize(int period)
{
    std::cout << "Initialize worker" << std::endl;
    this->Period = period;
    if(this->startup_check_flag)
    {
        this->startup_check();
    }
    else
    {
        // Inicializaciones personales
        viewer = new AbstractGraphicViewer(this, QRectF(-5000,-5000,10000,10000));
        viewer->add_robot(460,480,0,100,QColor("Blue"));
        viewer->show();
        viewer->activateWindow();

        timer.start(Period);
    }

}

void SpecificWorker::compute() {

    RoboCompLidar3D::TData ldata;
    try
    {
        ldata = lidar3d_proxy->getLidarData("bpearl", 0, 360, 1);
        qInfo() << ldata.points.size();
        const auto &points = ldata.points;
        if (points.empty()) return;


    //decltype(ldata.points) filtered_points;
    RoboCompLidar3D::TPoints filtered_points;
    std::ranges::copy_if(ldata.points, std::back_inserter(filtered_points), [](auto &p) { return p.z < 2000;});
    draw_lidar(filtered_points, viewer);
    std::tuple<Estado, RobotSpeed> res;
    switch(estado)
    {
        case Estado::IDLE:
            stop();
            break;
        case Estado::FOLLOW_WALL:
            estado = follow_wall(const_cast<RoboCompLidar3D::TPoints &>(points));
            break;
        case Estado::STRAIGHT_LINE: {
            estado = chocachoca(const_cast<RoboCompLidar3D::TPoints &>(points));

            break;
        }
        case Estado::SPIRAL:
            estado = spiral(const_cast<RoboCompLidar3D::TPoints &>(points));
            break;
    }
    }
    catch (const Ice::Exception &e)
    {
        std::cout << "Error reading from Camera" << e << std::endl;
    }
    /*
    estado = std::get<0>(res);
    auto &[estado_, robotspeed_] = res;
    estado = estado_;
    */
    try
    {
        //omnirobot_proxy->setSpeedBase(0,0,0.5);
    }
    catch(const Ice::Exception &e) {
        std::cout << "Error reading from Camera" << e << std::endl;
    }
}


int SpecificWorker::startup_check() {
    std::cout << "Startup check" << std::endl;
    QTimer::singleShot(200, qApp, SLOT(quit()));
    return 0;
}

void SpecificWorker::draw_lidar(const RoboCompLidar3D::TPoints &points, AbstractGraphicViewer *viewer)
{
    static std::vector<QGraphicsItem*> borrar;
    for(auto &b : borrar) {
        viewer->scene.removeItem(b);
        delete b;
    }
    borrar.clear();

    for(const auto &p : points)
    {
        auto point = viewer->scene.addRect(-50,-50,100, 100, QPen(QColor("Blue")), QBrush(QColor("Blue")));
        point->setPos(p.x, p.y);
        borrar.push_back(point);
    }
}

SpecificWorker::Estado SpecificWorker::chocachoca(RoboCompLidar3D::TPoints &points) {
    int offset = points.size()/2-points.size()/3;
    auto min_elem = std::min_element(points.begin()+offset, points.end()-offset, [](auto  a, auto b)
    { return std::hypot(a.x, a.y) < std::hypot(b.x, b.y); });

    const float MIN_DISTANCE_chocachoca = 700;
    qInfo() << std::hypot(min_elem->x, min_elem->y);
    int random_integer = rand();

    // Normaliza el número entero en un valor entre 0 y 1
    double random_decimal = (double)random_integer / RAND_MAX;
    double random_giro = (double)random_integer / RAND_MAX;
    double random_spiral = (double)random_integer / RAND_MAX;

    if(std::hypot(min_elem->x, min_elem->y) < MIN_DISTANCE_chocachoca)
    {
        if(random_decimal < 0.2){
            return Estado::FOLLOW_WALL;
        }
        else{
            if(random_giro <0.5){
                omnirobot_proxy->setSpeedBase(0, 0, 3);
            }

        }

    }else{

        omnirobot_proxy->setSpeedBase(2, 0, 0);
    }
    return Estado::STRAIGHT_LINE;
}

SpecificWorker::Estado SpecificWorker::follow_wall(RoboCompLidar3D::TPoints &points) {
    int offset = points.size() / 2 - points.size() / 3;
    auto min_elem = std::min_element(points.begin() + offset, points.end() - offset, [](auto a, auto b)
    { return std::hypot(a.x, a.y) < std::hypot(b.x, b.y); });
    srand(time(0));

    // Genera un número entero aleatorio entre 0 y RAND_MAX
    int random_integer = rand();

    // Normaliza el número entero en un valor entre 0 y 1
    double random_decimal = (double)random_integer / RAND_MAX;

    qInfo() <<"x: "<< abs(min_elem->x)<<"y: "<< abs(min_elem->y);

    if ( std::hypot(min_elem->x, min_elem->y) < MIN_DISTANCE) {
        omnirobot_proxy->setSpeedBase(0, 1, 1.2);
        if (random_decimal < 0.15) {
            return Estado::STRAIGHT_LINE;
        }
        if(abs(min_elem->x) > MIN_DISTANCE_X){
            omnirobot_proxy->setSpeedBase(2, 0, 0);
        }

    } else {
        if(abs(min_elem->x) > MIN_DISTANCE_X+15) {
            omnirobot_proxy->setSpeedBase(0, -1, -1);
        }else {
            omnirobot_proxy->setSpeedBase(2, 0, 0);
        }
    }
    MIN_DISTANCE_X = MIN_DISTANCE_X + 1;
    MIN_DISTANCE = MIN_DISTANCE + 1;
    return Estado::FOLLOW_WALL;
}


SpecificWorker::Estado SpecificWorker::spiral(RoboCompLidar3D::TPoints &points) {
    int offset = points.size() / 2 - points.size() / 3;
    auto min_elem = std::min_element(points.begin() + offset, points.end() - offset, [](auto a, auto b)
    { return std::hypot(a.x, a.y) < std::hypot(b.x, b.y); });

    const double DENOMINADOR = 6;
    const float ROTA = 0.80;
    const float MIN_DISTANCE= 400;

    if ( std::hypot(min_elem->x, min_elem->y) < MIN_DISTANCE) {
        return Estado::FOLLOW_WALL;
    }
    else {
        if (DENOMINADOR - delta > 0 && ROTA - rot > 0) {
            omnirobot_proxy->setSpeedBase(M_PI / (DENOMINADOR - delta), 0, ROTA);
        }
        rot = rot + 0.00025;
        delta = delta + (0.0065);


    }
    return Estado::SPIRAL;
}
SpecificWorker::Estado SpecificWorker::stop() {
    omnirobot_proxy->setSpeedBase(0, 0, 0);
    return Estado::IDLE;
}




/**************************************/
// From the RoboCompLidar3D you can call this methods:
// this->lidar3d_proxy->getLidarData(...)

/**************************************/
// From the RoboCompLidar3D you can use this types:
// RoboCompLidar3D::TPoint
// RoboCompLidar3D::TData

