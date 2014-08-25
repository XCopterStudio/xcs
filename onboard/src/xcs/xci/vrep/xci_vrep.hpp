#ifndef XCI_VREP_H
#define XCI_VREP_H

#include <xcs/xci/xci.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>
#include <thread>
#include <atomic>
#include <cmath>
#include <string>

extern "C" {
#include "vrep/extApi.h"
}

namespace xcs {
namespace xci {
namespace vrep{

    /*! \brief Xci implementation for quadricopter in v-rep simulator.*/
    class XciVrep : public virtual Xci {
        static const float POS_MULTI;
        static const unsigned int ATTEMPT_COUNT;

        std::atomic<bool> inited_;
        int clientID_;
        std::string droneName_;
        std::string frontCameraName_;
        std::string bottomCameraName_;
        std::string targetName_;
        std::string address_;

        int portNumber_;        
        int droneHandler_;
        int frontCameraHandler_;
        int bottomCameraHandler_;
        int targetHandler_;

        InformationMap configuration_;

        bool holdPosition_;
        bool holdAltitude_; 
        bool holdYaw_;

        xcs::CartesianVector targetPosition_;
        float targetYaw_;

        xcs::CartesianVector dronePosition_;
        xcs::EulerianVector droneRotation_;

        std::atomic_bool endAll_;

        std::thread updateThread_;
        std::thread videoThread_;

        void updateSensors();
        void updateImages();
    public:
        /*! Initialize private variables
            
            \param droneName name of quadricopter object in v-rep simulator
            \param frontCameraName name of front camera object on quardricopter in v-rep simulator
            \param bottomCameraName name of bottom camera object on quardricopter in v-rep simulator
            \param targetName name of target location object in v-rep simulator
            \param address ip address where v-rep simulator runs
            \param portNumber port for remote api connection with v-rep simulator
        */
        XciVrep(DataReceiver& dataReceiver, 
            std::string droneName = "Quadricopter",
            std::string frontCameraName = "Quadricopter_frontCamera",
            std::string bottomCameraName = "Quadricopter_floorCamera",
            std::string targetName = "Quadricopter_target",
            std::string address = "127.0.0.1", 
            int portNumber = 19997);
        /*! Close connection with v-rep simulator and end all running threads.*/
        ~XciVrep();

        /*! Name of xci implementation. 
            
            \return name of xci implementation
        */
        std::string name() override;

        //! Return list of all available sensors on quadricopter in v-rep simulator
        xcs::xci::SensorList sensorList() override;

        //! Return quadricopter configuration
        /*!
        \param key name of xci_vrep configuration field
        \return value of desired key or empty string
        */
        std::string configuration(const std::string &key) override;

        //! Return quadricopter configuration
        /*!
        \return All available configuration fields with values as map of pairs <name,value>
        */
        xcs::xci::InformationMap configuration() override;

        //! Return list of quadricopter special commands 
        /*!
        \sa command()
        \return Names of all special command which user can invoke on quadricopter in v-rep simulator.
        */
        xcs::xci::SpecialCMDList specialCMD() override;

        //! Take new quadricopter configuration and set this configuration to the quadricopter
        /*!
        \param key name of configuration field
        \param value desire value which xci set on quadricopter
        */
        void configuration(const std::string &key, const std::string &value) override;

        // !Take new quadricopter configuration and set this configuration to the quadricopter
        /*!
            \param configuration map of pairs <name,value> which xci set on quadricopter
        */
        void configuration(const xcs::xci::InformationMap &configuration) override;

        //! Take command from list of quadricopter special commands and invoke it on the quadricopter
        /*!
        \sa specialCMD()
        \param command quadricopter special command
        */
        void command(const std::string &command) override;

        //! Take four values for controlling quadricopter in v-rep simulator roll, pitch, yaw and gaz
        /*!
        All values have to be in range <-1,1> or It will be trimmed on this range. Parrot 2.0 mapped this range on <-max angle, max angle>.
        \param roll tilt on sides, -1 mean left, 1 right
        \param pitch nose tilt, -1 mean forward, 1 backward
        \param yaw rotation velocity, -1 mean nose rotation on left side, 1 mean nose rotation on right side
        \param gaz rise up or ascension speed, -1 mean max ascension speed, 1 mean max rise up speed
        */
        void flyControl(float roll, float pitch, float yaw, float gaz) override;

        //! Initialize Xci for use. Connect to the v-rep simulator.
        void init() override;
        
        //!Stop producing data
        void stop() override;
    };

}}}


#endif