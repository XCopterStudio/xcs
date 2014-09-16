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
    XInputPort<double> altitude;
    XInputPort<xcs::EulerianVector> rotation;

    XInputPort<double> initialDistance;
    XInputPort<double> initialDeviation;

    /*!
     * Image processing params
     */
    ::urbi::UVar cameraScale;

    /*!
     * Output params
     */
    XVar<double> distance;
    XVar<double> deviation;

    XLineKeeper(const std::string &);

    void init();
protected:
    virtual void stateChanged(XObject::State state);
private:

    struct Point2f {

        Point2f(double x = 0, double y = 0) : x(x), y(y) {
        }
        double x;
        double y;
    };
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
    typedef Point2f VectorType; // TODO replace with OpenCV type or own syntactic type


    VectorType positionShift_;
    TimePoint lastTimeVx_;
    TimePoint lastTimeVy_;

    double initialDistCache_;

    double initialDistance_;
    double initialDeviation_;
    double initialDevOffset_;

    double altitude_;
    xcs::EulerianVector rotation_;

    bool isKeeping_;

    void onChangeVelocity(const xcs::CartesianVector v);

    void onChangeAltitude(const double altitude) {
        altitude_ = altitude;
    }

    void onChangeRotation(const xcs::EulerianVector rotation) {
        rotation_ = rotation;
    }

    void onChangeInitialDistance(const double value) {
        initialDistCache_ = value;
    }

    void onChangeInitialDeviation(const double value) {
        reset(initialDistCache_, value);
    }

    void updateOutput();
    void reset(const double distance, const double deviation);
};

}
}

#endif // U_LINE_FINDER_HPP
