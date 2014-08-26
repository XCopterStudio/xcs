function OnboardConnection(application, pingInterval) {
    
    var app = application,
        net = require('net'),
        onboard = null,
        com = net.createServer(handleNewConnection),
        ping_ = require('./ping_monitor.js'),
        connected_ = false;
    
    /*
     * Methods
     */
    function start(port) {
        com.listen(port, function () {
            console.log('XCS onboard service running on port ' + port);
        });
    };

    function startVideo(host, port) {
        var dgram = require('dgram'),
            server = dgram.createSocket('udp4');

        server.on('listening', function () {
            var address = server.address();
            console.log('XCS video UDP Server listening on ' + address.address + ":" + address.port);
        });

        server.on('message', function (message, remote) {
            app.Client.send(message, 'video');
        });

        server.bind(port, host);
    };

    function send(data) {
        if (onboard) {
            onboard.write(data);
        }
    };
    
    function isConnected() {
        return connected_;
    };
    
    /*
     * Handlers
     */
    function handleNewConnection(socket) {
        if (onboard) {
            console.log('Attempting onboard connect. There\'s one connection already!');
            return;
        }
        onboard = socket;
        socket.on('end', handleEnd);
        socket.on('data', handleData);
        // set ping monitor
        ping_.startHeartbeat(function (payload) {
            var chunk = { type: 'ping', data: payload };
            send(JSON.stringify(chunk));
        });
        connected_ = true;
        notifyClient(connected_);
        console.log('Onboard connected.');
    };
    
    var bufferData = '';
    function handleData(data) {
        try {
            var parsed = JSON.parse(data);
            bufferData = '';
        } catch(e1) {
            try {
                console.log('JSON from onboard - try use buffered data.');
                bufferData += data;
                parsed = JSON.parse(bufferData);
                bufferData = '';
            } catch (e2) {
                parsed = null;
                console.log('ERROR (parse incoming data)' + e1.message);
            }
        }

        if (parsed) {
            if (parsed['type'] == 'pong') {
                ping_.setEcho(parsed['data']);
                return;
            }
            console.log('<<<<<<<<<<<<<<<< JSON from onboard parsed and relaying:');
            app.Client.send(parsed, 'data');
            console.log(parsed);
        }
    };

    function handleEnd() {
        onboard = null;
        ping_.stopHeartbeat();
        connected_ = false
        notifyClient(connected_);
        console.log('Onboard disconnected.');
    };
    
    function notifyClient(connected) {
        app.Client.send({ connected: connected }, 'onboard');   
    };
    
    return {
        start: start,
        startVideo: startVideo,
        send: send,
        ping: ping_,
        isConnected: isConnected,
    };
}

module.exports = OnboardConnection;
