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
  
    /*! \brief Input channel for incoming dat in to the XMultiplexer. */
    template<class type> 
    class Channel : public XObject{
        unsigned int id_;
        std::atomic<unsigned int>& channel_;
        std::atomic<bool>& stopped_;
        std::atomic<bool>& defaultSetChannel_;
        std::atomic<unsigned int>& minChannel_;
        xcs::nodes::XVar<type>& output_;
        /// Urbi input port with incomming data
        std::shared_ptr< xcs::nodes::XInputPort<type> > input_;
    public:
        /*! Initialize private variables.

            \param id channel unique identifier
            \param channel selector
            \param output channel
            \param stopped stop data redirection
        */
        Channel(const unsigned int id, std::atomic<unsigned int> &channel,
            xcs::nodes::XVar<type>& output, std::atomic<bool>& stopped, std::atomic<bool>& defaultSetChannel, std::atomic<unsigned int>& minChanne, 
            std::shared_ptr< xcs::nodes::XInputPort<type> > input);

        /*! Redirect data from input port to the output when Channel id is equal to the channel.

            Urbi invoke this method when encounter some changes on input 
            port associated with this channel
        */
        void onChangeInput(const type data);
    };

    template<class type>
    Channel<type>::Channel(const unsigned int id, std::atomic<unsigned int> &channel, xcs::nodes::XVar<type>& output, std::atomic<bool>& stopped,
        std::atomic<bool>& defaultSetChannel, std::atomic<unsigned int>& minChannel, std::shared_ptr< xcs::nodes::XInputPort<type> > input) :
        XObject(std::string()),
        id_(id), channel_(channel), stopped_(stopped), defaultSetChannel_(defaultSetChannel), minChannel_(minChannel),
        output_(output), 
        input_(input) {

        XNotifyChange((*input_), &Channel<type>::onChangeInput);
    }

    template<class type>
    void Channel<type>::onChangeInput(const type data){
        if (defaultSetChannel_) {
            if (id_ <= minChannel_) {
                output_ = data;
                minChannel_ = id_;
            }
        }
        else {
            if (id_ == channel_ && !stopped_) {
                output_ = data;
            }
        }
    }

    /*! \brief Simple multiplexer which listen on many input channels and get on output only data from selected channel.

        For syntax specialization go in to the x_multiplexer.cpp an add your specialization.
    */
    template<class type>
    class XMultiplexer : public XObject{
        typedef std::list<std::shared_ptr<Channel<type> > > Channels;
        typedef std::list< std::shared_ptr< xcs::nodes::XInputPort<type> > > InputPorts;

        unsigned int channelCount_;
        std::atomic<unsigned int> channel_;
        std::atomic<bool> stopped_;

        Channels channels_;

        std::atomic<bool> defaultSetChannel_;
        std::atomic<unsigned int> minChannel_;

        void onChangeChannel(unsigned int channel);
        void onChangeInput(const type data);

        InputPorts inputs_;
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
            \param defaultSetChannel flag if default set channle should be used
        */
        void init(const unsigned int channelCount, const bool defaultSetChannel);
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

            if (defaultSetChannel_) {
                minChannel_ = channelCount_ - 1;
            }

            break;
        }
    }

    template<class type>
    XMultiplexer<type>::XMultiplexer(const std::string& name) : XObject(name),
    setChannel("CHANNEL"),
    output("*"){
        stopped_ = true;

        XBindVar(output);

        XBindFunction(XMultiplexer<type>, init);
    }

    template<class type>
    void XMultiplexer<type>::init(const unsigned int channelCount, const bool defaultSetChannel) {
        if (channelCount < 1){
            channelCount_ = 1;
            XCS_LOG_WARN("None input channel is not allowed. Multiplexer have been created with one.");
        } else {
            channelCount_ = channelCount;
        }

        defaultSetChannel_ = defaultSetChannel;
        if (!defaultSetChannel_) {
            XBindVarF(setChannel, &XMultiplexer<type>::onChangeChannel);
        }

        minChannel_ = channelCount_ - 1;

        for (int i = 0; i < channelCount_; ++i){
            std::shared_ptr< xcs::nodes::XInputPort<type> > input = std::shared_ptr< xcs::nodes::XInputPort<type> >(new xcs::nodes::XInputPort<type>("*"));
            inputs_.push_back(input);

            std::stringstream name;
            name << "input";
            name << i + 1;

            XBindVarRename((*input), name.str());

            std::shared_ptr<Channel<type> > channel = std::shared_ptr<Channel<type> >(
                new Channel<type>(i, channel_, output, stopped_, defaultSetChannel_, minChannel_, input));

            channels_.push_back(channel); 
        }
    }

    template<class type>
    void XMultiplexer<type>::onChangeInput(const type data) {
        XCS_LOG_WARN("input changed");
    }

}}}

#endif