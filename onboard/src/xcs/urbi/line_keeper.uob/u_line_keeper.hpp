#ifndef U_LINE_KEEPER_HPP
#define U_LINE_KEEPER_HPP

#include <cstdint>
#include <chrono>
#include <urbi/uobject.hh>
#include <xcs/urbi/line_drawer.uob/u_line_drawer.hpp>


namespace xcs {
namespace urbi {

class ULineKeeper : public ::urbi::UObject {
public:
    /*!
     * Inputs
     */
    ::urbi::InputPort vx;
    ::urbi::InputPort vy;
    ::urbi::UVar altitude; // intentionally UVar
    ::urbi::UVar psi;

    /*!
     * Image processing params
     */
    ::urbi::UVar cameraScale;

    /*!
     * Output params
     */
    ::urbi::UVar distanceUVar;
    ::urbi::UVar deviationUVar;

    ULineKeeper(const std::string &);
    
    void init();

    void start(double distance, double deviation);
    
    void stop();

    /*!
     * Urbi period handler
     */
    virtual int update();

    void setLineDrawer(UObject *drawer);
private:

    struct Point2f {

        Point2f(double x = 0, double y = 0) : x(x), y(y) {
        }
        double x;
        double y;
    };
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    typedef Point2f VectorType; // TODO replace with OpenCV type or own syntactic type


    const static size_t REFRESH_PERIOD;

    xcs::urbi::ULineDrawer *lineDrawer_;

    VectorType positionShift_;
    TimePoint lastTimeVx_;
    TimePoint lastTimeVy_;

    double initialDistance_;
    double initialDeviation_;
    double initialDevOffset_;
    
    bool isKeeping_;

    void onChangeVx(double vx);
    void onChangeVy(double vy);

};

}
}

#endif // U_LINE_FINDER_HPP
