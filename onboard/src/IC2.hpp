#ifndef XCI_H
#define XCI_H

#include <string>
#include <vector>

typedef (void *controlHandler)(double roll, double pitch, double yaw, double gaz);
typedef (void *urbiHandler)(std::string command, std::string param);
typedef (void *configurationHandler)(std::string key, std::string value);

//! Onboard side of the interface C (between onboard and comm).

class IC2 {
public:
    //
    // Sending part of the interface
    //

    //! Sends sensor data to comm, interpretation of (void *) depends on the sensor.
    virtual void sendSensorData(Sensor sensor, void *data) = 0;

    virtual void sendConfiguration(const std::string &key, const std::string &value) = 0;

    //! We'll see later what this should be precisely.
    virtual void sendUrbiStatus(void *data) = 0;


    //
    // Receiving part of the interface
    //
    virtual void registerControlHandler(controlHandler handler);
    virtual void registerUrbiHandler(urbiHandler handler);
    virtual void registerConfigurationHandler(configurationHandler handler);

};

#endif
