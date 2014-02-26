/* 
 * File:   UXci.hpp
 * Author: michal
 *
 * Created on January 31, 2014, 11:07 PM
 */

#ifndef UXCI_HPP
#define	UXCI_HPP

#include <urbi/uobject.hh>
#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>

#include <xcs/xci/xci.hpp>
#include <xcs/xci/data_receiver.hpp>

namespace xcs {
namespace urbi {

class UXci : public ::urbi::UObject {
public:
    ::urbi::UVar flyParamPersistence;

    ::urbi::InputPort roll;
    ::urbi::InputPort pitch;
    ::urbi::InputPort yaw;
    ::urbi::InputPort gaz;

    ::urbi::InputPort command;

    UXci(const std::string &name);

    /*!
     * \param driver Name of the library with XCI implementation.
     */
    void init(const std::string &driver);

    void xciInit();

    void doCommand(const std::string &command);

    void flyParam(double roll = 0, double pitch = 0, double yaw = 0, double gaz = 0);

    virtual ~UXci();
private:
    xcs::xci::XCI* xci_;

    xcs::xci::DataReceiver dataReceiver_;

    //! Guard to prevent double initialization.
    bool inited_;

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
     * How often is fly param sent to XCI (in ms).
     * When set to 0, no persistence is enforced.
     */
    unsigned int flyParamPersistence_;

    std::thread flyParamThread_;

    std::mutex flyParamMtx_;

    std::condition_variable flyParamCond_;

    void initOutputs();
};

}
}

#endif	/* UXCI_HPP */

