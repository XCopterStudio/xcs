function ClientConnection(application, pingInterval) {
    var app = application,
        io = require('socket.io')(application.Http),
        client = null,
        ping_ = require('./ping_monitor.js');
    
    io.on('connection', handleNewConnection);
    
    /*
     * Client's interface for RPC-like calls
     */
    var clientIface = {
     
        getLatency: function () {
            var onbLatency = app.Onboard.ping.getLatency(),
                srvLatency = ping_.getLatency();
            if (!app.Onboard.isConnected() || onbLatency == -1 || srvLatency == -1) {
                return -1;   
            }
            return srvLatency + onbLatency;
        },
        
        isOnboardConnected: function () {
            return app.Onboard.isConnected();
        },
        
    }
    
    /*
     * Methods
     */
    function start(port) {
        app.Http.listen(port, function () {
            console.log('XCS client service running on port ' + port);
        });
    }
    
    function send(data, eventName) {
        if (client) {
            if (eventName) {
                client.emit(eventName, data);
            }
            else {
                client.emit('data', data);
            }
        }
    }
    
    /*
     * Handlers
     */
    function handleNewConnection(socket) {
        // overwrite client's socket => last initiated connection is the ACTIVE one
        client = socket;
        // set event handlers
        socket.on('disconnect', handleDisconnect);
        socket.on('resend', handleRelay); // TODO: refactor event name resend -> relay
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
        client = null;
        ping_.stopHeartbeat();
        console.log('Client disconnected.');
    }
    
    function handleRelay(data) {
        console.log('>>>>>>>>>>>>>>>> Client relaying data: ');
        console.log(data);
        app.Onboard.send(data + '\n');
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