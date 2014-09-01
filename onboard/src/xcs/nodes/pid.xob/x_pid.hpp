#ifndef X_PID_H
#define X_PID_H

//! Define base XObject class from which all nodes in XCS have to inherited
#include <xcs/nodes/xobject/x_object.hpp>
//! Define XInputPort<Syntactic type> for input data in to a node from others nodes
#include <xcs/nodes/xobject/x_input_port.hpp>
//! Define XVar<Syntactic type> for publishing data from node to the XCS
#include <xcs/nodes/xobject/x_var.hpp>

#include <xcs/pid.hpp>

#include <atomic>

namespace xcs{
namespace nodes{
namespace pid{

    /*! \brief XObject wrapper around PID controller.

        It have to be inherited from XObject class.
        Constructor have to have only one parameter "const std::string& name". This parameter use for call the base constructor.
        If programmer wants pass some input values when class is constructed then define function init with whatever parameters instead of constructor.
    */
    class XPID : public XObject{
        //! PID parameters type
        typedef double PIDType;
        //! PID controller
        PID<PIDType,double> pid_;
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
        void onChangeP(const PIDType p);
        /*! \brief Change integral parameter in pid_ (PID controller).
            XCS invoke this method every time when arrive new data on the I XInputPort.
            \param i new PID integral parameter
            \see onChangeP
            \see onChangeD
        */
        void onChangeI(const PIDType i);
        /*! \brief Change derivative parameter in pid_ (PID controller).
            XCS invoke this method every time when arrive new data on the D XInputPort.
            \param d new PID derivative parameter
            \see onChangeP
            \see onChangeI
        */
        void onChangeD(const PIDType d);
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
        /*! \brief XCS invoke this method when node is stopped or started from GUI

            \param state actual action on node (stopped, started)
        */
        virtual void stateChanged(XObject::State state);
    public:
        //! PID proportional parameter
        XInputPort<PIDType> P;
        //! PID integral parameter
        XInputPort<PIDType> I;
        //! PID derivative parameter
        XInputPort<PIDType> D;

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