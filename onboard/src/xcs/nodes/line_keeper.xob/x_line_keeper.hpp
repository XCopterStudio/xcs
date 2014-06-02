#ifndef U_LINE_KEEPER_HPP
#define U_LINE_KEEPER_HPP

#include <cstdint>
#include <chrono>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/line_drawer.xob/x_line_drawer.hpp>


namespace xcs {
namespace nodes {

class XLineKeeper : public XObject {
public:
    /*!
     * Inputs
     */
    XInputPort<xcs::CartesianVector> velocity;
    XVar<double> altitude; // intentionally XVar
    XVar<xcs::EulerianVector> rotation; // intentionally XVar

    /*!
     * Image processing params
     */
    ::urbi::UVar cameraScale;

    /*!
     * Output params
     */
    ::urbi::UVar distanceUVar;
    ::urbi::UVar deviationUVar;

    XLineKeeper(const std::string &);
    
    void init();

    void reset(double distance, double deviation);
    
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

    xcs::nodes::XLineDrawer *lineDrawer_;

    VectorType positionShift_;
    TimePoint lastTimeVx_;
    TimePoint lastTimeVy_;

    double initialDistance_;
    double initialDeviation_;
    double initialDevOffset_;
    
    bool isKeeping_;

    void onChangeVelocity(const xcs::CartesianVector v);

};

}
}

#endif // U_LINE_FINDER_HPP
