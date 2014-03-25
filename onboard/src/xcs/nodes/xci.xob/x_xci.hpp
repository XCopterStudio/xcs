/*
 * File:   XXci.hpp
 * Author: michal
 *
 * Created on January 31, 2014, 11:07 PM
 */

#ifndef XXCI_HPP
#define	XXCI_HPP

//#include <urbi/uobject.hh>
#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <xcs/xci/xci.hpp>
#include <xcs/xci/data_receiver.hpp>
#include <xcs/nodes/xobject/x_object.hpp>
#include <xcs/nodes/xobject/x_var.hpp>
#include <xcs/nodes/xobject/x_input_port.hpp>
#include "fly_param.hpp"

namespace xcs {
namespace nodes {

class XXci : public xcs::nodes::XObject {
public:
    xcs::nodes::XVar<int> flyParamPersistence;

    xcs::nodes::XInputPort<double> roll;
    xcs::nodes::XInputPort<double> pitch;
    xcs::nodes::XInputPort<double> yaw;
    xcs::nodes::XInputPort<double> gaz;

    xcs::nodes::XVar<FlyParam> fly;

    xcs::nodes::XInputPort<std::string> command;

    XXci(const std::string& name);

    /*!
     * \param driver Name of the library with XCI implementation.
     */
    void init(const std::string& driver);

    void xciInit();

    void doCommand(const std::string& command);

    void flyParam(double roll = 0, double pitch = 0, double yaw = 0, double gaz = 0);

    std::string getConfiguration(const std::string& key);
    xcs::xci::InformationMap dumpConfiguration();

    void setConfiguration(const std::string& key, const std::string& value);

    virtual ~XXci();
private:
    xcs::xci::XCI* xci_;

    xcs::xci::DataReceiver dataReceiver_;

    //! Guard to prevent double initialization.
    bool inited_;

    void onChangeFly(FlyParam fp);

    //! InputPort doesn't provide its current value, we keep it ourselves
    double roll_;
    void onChangeRoll(double roll);

    //! InputPort doesn't provide its current value, we keep it ourselves
    double pitch_;
    void onChangePitch(double pitch);

    //! InputPort doesn't provide its current value, we keep it ourselves
    double yaw_;
    void onChangeYaw(double yaw);

    //! InputPort doesn't provide its current value, we keep it ourselves
    double gaz_;
    void onChangeGaz(double gaz);

    /*!
     * Setter for flyParam persistence.
     * It blocks/unblock the persistence thread (when set to 0).
     *
     * \note It may take up to 'previous value' time until the new persistence
     * is set.
     */
    void setFlyParamPersistence(unsigned int value);

    /*!
     * Enables/disables persistence (waking/suspending the thread).
     */
    void setFlyParamActive(bool value = true);

    void keepFlyParam();

    void sendFlyParam();


    /*!
     * Whether fly param persistence is active.
     */
    bool flyParamActive_;
    
    /*!
     * Variable that controls fly param keeping thread.
     */
    volatile bool flyParamAlive_;

    /*!
     * How often is fly param sent to XCI (in ms).
     * When set to 0, no persistence is enforced.
     */
    unsigned int flyParamPersistence_;

    std::thread flyParamThread_;

    std::mutex flyParamMtx_;

    std::condition_variable flyParamCond_;

    void initOutputs();
    
    /*!
     * Correctly terminates keep fly params thread.
     */
    void stopFlyParamsThread();
};

}
}

#endif	/* XXCI_HPP */

