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

#include "xcs/xci/XCI.hpp"
#include "xcs/xci/DataReceiver.hpp"

namespace xcs {
namespace urbi {

class UXci : public ::urbi::UObject {
public:
    ::urbi::InputPort roll;
    ::urbi::InputPort pitch;
    ::urbi::InputPort yaw;
    ::urbi::InputPort gaz;

    UXci(const std::string &name);

    /*!
     * \param driver Name of the library with XCI implementation.
     */
    void init(const std::string &driver);

    void xciInit();

    void command(const std::string &command);

    void flyParam(double roll, double pitch, double yaw, double gaz);


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

    void keepFlyParam();
    /*!
     * How often is fly param sent to XCI (in ms).
     */
    unsigned int flyParamPersistence_;
    std::thread flyParamThread_;

    void initOutputs();
};

}
}

#endif	/* UXCI_HPP */
