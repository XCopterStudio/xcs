function ClientConnection(application, pingInterval) {
    var app_ = application,
        io_ = require('socket.io')(application.Http),
        client_ = null,
        ping_ = require('./ping_monitor.js');
    
    io_.on('connection', handleNewConnection);
    
    /*
     * Client's interface for RPC-like calls
     */
    var clientIface = {
     
        getLatency: function () {
            var onbLatency = app_.Onboard.ping.getLatency(),
                srvLatency = ping_.getLatency();
            if (!app_.Onboard.isConnected() || onbLatency == -1 || srvLatency == -1) {
                return -1;   
            }
            return srvLatency + onbLatency;
        },
        
        isOnboardConnected: function () {
            return app_.Onboard.isConnected();
        },
        
    }
    
    /*
     * Methods
     */
    function start(port) {
        app_.Http.listen(port, function () {
            console.log('XCS client service running on port ' + port);
        });
    }
    
    function send(data, eventName) {
        if (client_) {
            if (eventName) {
                client_.emit(eventName, data);
            }
            else {
                client_.emit('data', data);
            }
        }
    }
    
    /*
     * Handlers
     */
    function handleNewConnection(socket) {
        // overwrite client's socket => last initiated connection is the ACTIVE one
        client_ = socket;
        // set event handlers
        socket.on('disconnect', handleDisconnect);
        socket.on('relay', handleRelay);
        socket.on('call', handleCall);
        // set ping monitor
        socket.on('ping echo', function (payload) {
            ping_.setEcho(payload);
        });
        ping_.startHeartbeat(function (payload) {
            send(payload, 'ping');
        });
        console.log('Client connected.');
    }
    
    function handleDisconnect() {
        client_ = null;
        ping_.stopHeartbeat();
        console.log('Client disconnected.');
    }
    
    function handleRelay(data) {
        console.log('>>>>>>>>>>>>>>>> Client relaying data: ');
        console.log(data);
        app_.Onboard.send(data + '\n');
    }
    
    function handleCall(methodName) {
        if (methodName in clientIface) {
            var returned = clientIface[methodName]();
            var data = { method: methodName, returned: returned };
            send(data, 'call return');
        }
    }
    
    return {
        start: start,
        send: send,
    };
}

module.exports = ClientConnection;