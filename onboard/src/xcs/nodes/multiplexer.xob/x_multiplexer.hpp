#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

// xcs logging library
#include <xcs/logging.hpp>

// necessary headers files
#include <xcs/nodes/xobject/x_object.hpp>
// xcs input port
#include <xcs/nodes/xobject/x_input_port.hpp>
// xcs output port
#include <xcs/nodes/xobject/x_var.hpp>
// xcs macros for binding XVars and InputPorts to the urbi
#include <xcs/nodes/xobject/x.h>

// std library header
#include <atomic>
#include <list>
#include <sstream>

namespace xcs{
namespace nodes{
namespace multiplexer{
  
    template<class type> 
    class Channel : public XObject{
        unsigned int id_;
        std::atomic<unsigned int>& channel_;
        std::atomic<bool>& stopped_;
        xcs::nodes::XVar<type>& output_;
    public:
        xcs::nodes::XInputPort<type> input;

        Channel(const unsigned int id, std::atomic<unsigned int> &channel,
            xcs::nodes::XVar<type>& output, std::atomic<bool>& stopped);

        // XCS invoke this method when encounter some changes on input 
        // port associated with this channel
        void onChangeInput(const type data);
    };

    template<class type>
    Channel<type>::Channel(const unsigned int id, std::atomic<unsigned int> &channel,
        xcs::nodes::XVar<type>& output, std::atomic<bool>& stopped)
        : XObject(std::string()), id_(id), channel_(channel), stopped_(stopped),
        output_(output), input("*"){
        XBindVarF(input, &Channel<type>::onChangeInput);
    }

    template<class type>
    void Channel<type>::onChangeInput(const type data){
        if (id_ == channel_ && !stopped_){
            output_ = data;
        }
    }

    template<class type>
    class XMultiplexer : public XObject{
        typedef std::list<std::shared_ptr<Channel<type> > > Channels;
        typedef std::list<xcs::nodes::XInputPort<type> > InputPorts;

        unsigned int channelCount_;
        std::atomic<unsigned int> channel_;
        std::atomic<bool> stopped_;

        Channels channels_;

        void onChangeChannel(unsigned int channel);
    protected:
        virtual void stateChanged(XObject::State state);
    public:
        // Input data
        xcs::nodes::XInputPort<unsigned int> setChannel;
        // Output data
        xcs::nodes::XVar<type> output;

        XMultiplexer(const std::string& name);
        void init(const unsigned int channelCount);
    };

    template<class type>
    void XMultiplexer<type>::onChangeChannel(unsigned int channel){
        if (channel < channelCount_){
            channel_ = channel;
        } else {
            XCS_LOG_WARN("Channel number out of range");
        }
    }

    template<class type>
    void XMultiplexer<type>::stateChanged(XObject::State state){
        switch (state) {
        case XObject::STATE_STARTED:
            stopped_ = false;
            break;
        case XObject::STATE_STOPPED:
            stopped_ = true;
            break;
        }
    }

    template<class type>
    XMultiplexer<type>::XMultiplexer(const std::string& name) : XObject(name),
    setChannel("CHANNEL"),
    output("*"){
        stopped_ = true;

        XBindVarF(setChannel, &XMultiplexer<type>::onChangeChannel);

        XBindVar(output);

        XBindFunction(XMultiplexer<type>, init);
    }

    template<class type>
    void XMultiplexer<type>::init(const unsigned int channelCount){
        if (channelCount < 1){
            channelCount_ = 1;
            XCS_LOG_WARN("None input channel is not allowed. Multiplexer have been created with one.");
        } else {
            channelCount_ = channelCount;
        }

        for (int i = 0; i < channelCount_; ++i){
            std::shared_ptr<Channel<type> > channel = std::shared_ptr<Channel<type> >(
                new Channel<type>(i, channel_, output, stopped_));

            std::stringstream name;
            name << "input";
            name << i;
            XBindVarRename(channel->input, name.str());

            channels_.push_back(channel); 
        }
    }

}}}

#endif