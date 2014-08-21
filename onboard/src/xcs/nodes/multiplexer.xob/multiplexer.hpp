#ifndef X_MULTIPLEXER_H
#define X_MULTIPLEXER_H

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
  
    /*! \brief Input channel for incoming dat in to the XMultiplexer. */
    template<class type> 
    class Channel : public XObject{
        unsigned int id_;
        std::atomic<unsigned int>& channel_;
        std::atomic<bool>& stopped_;
        xcs::nodes::XVar<type>& output_;
    public:
        /// Urbi input port with incomming data
        xcs::nodes::XInputPort<type> input;

        /*! Initialize private variables.

            \param id channel unique identifier
            \param channel selector
            \param output channel
            \param stopped stop data redirection
        */
        Channel(const unsigned int id, std::atomic<unsigned int> &channel,
            xcs::nodes::XVar<type>& output, std::atomic<bool>& stopped);

        /*! Redirect data from input port to the output when Channel id is equal to the channel.

            Urbi invoke this method when encounter some changes on input 
            port associated with this channel
        */
        void onChangeInput(const type data);
    };

    template<class type>
    Channel<type>::Channel(const unsigned int id, std::atomic<unsigned int> &channel,
        xcs::nodes::XVar<type>& output, std::atomic<bool>& stopped)
        : XObject(std::string()), id_(id), input("*"), channel_(channel), stopped_(stopped),
        output_(output){
        UNotifyChange(dynamic_cast<urbi::UVar&>(input), &Channel<type>::onChangeInput);
    }

    template<class type>
    void Channel<type>::onChangeInput(const type data){
        if (id_ == channel_ && !stopped_){
            output_ = data;
        }
    }

    /*! \brief Simple multiplexer which listen on many input channels and get on output only data from selected channel.

        For syntax specialization go in to the x_multiplexer.cpp an add your specialization.
    */
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
        /// Set which channel will be redirected to the output_
        xcs::nodes::XInputPort<unsigned int> setChannel;
        /// Redirected data from selected input channel
        xcs::nodes::XVar<type> output;

        //! Initialize private variables.
        XMultiplexer(const std::string& name);

        /*! Create input channels according to the channelCount and register they in Urbi as XMultiplexer inputs.
        
            Bind output and setChannel in Urbi and set notifier onChangeChannel on setChannel input port.
            \param channelCount how much input channel will be created
        */
        void init(const unsigned int channelCount);
    };

    template<class type>
    void XMultiplexer<type>::onChangeChannel(unsigned int channel){
        if (channel < channelCount_){
            channel_ = channel;
        }
        else{
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
        }
        else{
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