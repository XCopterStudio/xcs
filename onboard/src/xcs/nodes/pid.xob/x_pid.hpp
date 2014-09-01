#ifndef X_PID_H
#define X_PID_H

#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include <xcs/nodes/xobject/x_var.hpp>

#include <xcs/pid.hpp>

#include <atomic>

namespace xcs{
namespace nodes{
namespace pid{

    //! XObject wrapper around PID controller
    class XPID : public XObject{
        //! PID parameters type
        typedef double PidType;
        //! PID controller
        PID<PidType,double> pid_;
        //! desire value of the controlled process
        double desireValue_;

        //! if is XPID stopped
        std::atomic<bool> stopped_;

        /*! \brief Change proportional parameter in pid_ (PID controller).
            XCS invoke this method every time when arrive new data on the P XInputPort.
            \param p new PID proportional parameter
            \see onChangeI
            \see onChangeD
        */
        void onChangeP(const PidType p);
        /*! \brief Change integral parameter in pid_ (PID controller).
            XCS invoke this method every time when arrive new data on the I XInputPort.
            \param i new PID integral parameter
            \see onChangeP
            \see onChangeD
        */
        void onChangeI(const PidType i);
        /*! \brief Change derivative parameter in pid_ (PID controller).
            XCS invoke this method every time when arrive new data on the D XInputPort.
            \param d new PID derivative parameter
            \see onChangeP
            \see onChangeI
        */
        void onChangeD(const PidType d);
        /*! \brief Pass new value from the controlled process in to the PID controller and publish new computed control value from it at control XVar.
            XCS invoke this method every time when arrive new data on the actualValue XInputPort.
            \param actualValue controlled process actual value
        */
        void onChangeActualValue(const double actualValue);
        /*! \brief Change desire value of the controlled process.
            \param desireValue new desire value of the controlled process
        */
        void onChangeDesireValue(const double desireValue);
    protected:
        virtual void stateChanged(XObject::State state);
    public:
        //! PID proportional parameter
        XInputPort<PidType> P;
        //! PID integral parameter
        XInputPort<PidType> I;
        //! PID derivative parameter
        XInputPort<PidType> D;

        //! controlled process desire value
        XInputPort<double> desireValue;
        //! controlled process actual value
        XInputPort<double> actualValue;

        //! new control for controlled process base on the desire and actual values
        XVar<double> control;

        //! Bind XVar, XInputPorts and callback function in XCS
        XPID(const std::string& name);
    };

}}}

#endif