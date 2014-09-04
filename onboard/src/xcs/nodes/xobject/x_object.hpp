#ifndef XOBJECT_HPP
#define XOBJECT_HPP

#include <urbi/uobject.hh>
#include <string>
#include <map>
#include <list>
#include "x_type.hpp"
#include <xcs/nodes/xobject/xobject_export.h>

namespace xcs {
namespace nodes {

class XOBJECT_EXPORT XObject : public urbi::UObject {
public:

    enum State {
        STATE_CREATED = 0, STATE_STARTED = 1, STATE_STOPPED = 2,
    };
    typedef std::list<std::string> StringList;
    /*!
     * \param name This is parameter required by Urbi implementation, your constructor must have same signature and pass this parameter.
     */
    XObject(const std::string& name);
    virtual ~XObject();
    /*!
     * Called to start the dataflow.
     * 
     * \note Do not override this method, rather implement xcs::nodes::XObject::stateChanged method.
     */
    void start();
    /*!
     * Called to stop the dataflow.
     * 
     * \note Do not override this method, rather implement xcs::nodes::XObject::stateChanged method.
     */
    void stop();
    /*!
     * \return Syntactic type name of XVar/XInputPort with given name.
     */
    const std::string getSynType(const std::string& xVarName) const;
    /*!
     * \return Semantic type name of XVar/XInputPort with given name.
     */
    const std::string getSemType(const std::string& xVarName) const;
    /*!
     * Metadata about the XObject.
     * 
     * \return List of names of declared XVars.
     */
    StringList getXVars() const;
    /*!
     * Metadata about the XObject.
     * 
     * \return List of names of declared XInputPorts.
     */
    StringList getXInputPorts() const;
protected:
    /*!
     * It's used to fill XObject metadata.
     * 
     * \note It has different usage that xcs::nodes::XDatalogger::registerXVar.
     */
    bool registerXVar(const std::string& xVarName, const XType& type);
    /*!
     * It's used to fill XObject metadata.
     *      
     */
    bool registerXInputPort(const std::string& xVarName, const XType& type);
    /*!
     * Override this method to handle XObjects state change.
     * 
     * \note This method will never be called with STATE_CREATED parameter.
     */
    virtual void stateChanged(XObject::State state);
    const XObject::State getState() const;
private:
    typedef std::map<const std::string, XType> XTypesType;
    StringList getXChilds(const XType::DataFlowType dataFlowType) const;
    bool registerXChild(const std::string& xVarName, const XType& type, const XType::DataFlowType dataFlowType);
    void setState(XObject::State state);
    XTypesType xVarsType_;
    XObject::State state_;

    friend class SimpleXVar;
    friend class SimpleXInputPort;
};

}
}

#endif