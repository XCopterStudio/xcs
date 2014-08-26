function OnboardConnection(application, pingInterval) {

    var app_ = application,
        net_ = require('net'),
        onboard_ = null,
        com_ = net_.createServer(handleNewConnection),
        ping_ = require('./ping_monitor.js'),
        connected_ = false;

    /*
     * Methods
     */
    function start(port) {
        com_.listen(port, function () {
            console.log('XCS onboard service running on port ' + port);
        });
    };

    function startVideo(port) {
        var dgram = require('dgram'),
            server = dgram.createSocket('udp4');

        server.on('listening', function () {
            var address = server.address();
            console.log('XCS video UDP Server listening on port ' + address.port);
        });

        server.on('message', function (message, remote) {
            app_.Client.send(message, 'video');
        });

        server.bind(port);
    };

    function send(data) {
        if (onboard_) {
            onboard_.write(data);
        }
    };

    function isConnected() {
        return connected_;
    };

    /*
     * Handlers
     */
    function handleNewConnection(socket) {
        if (onboard_) {
            console.log('Attempting onboard connect. There\'s one connection already!');
            return;
        }
        onboard_ = socket;
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

    var bufferData_ = '';
    function handleData(data) {
        try {
            var parsed = JSON.parse(data);
            bufferData_ = '';
        } catch(e1) {
            try {
                console.log('JSON from onboard - try use buffered data.');
                bufferData_ += data;
                parsed = JSON.parse(bufferData_);
                bufferData_ = '';
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
            app_.Client.send(parsed, 'data');
            console.log(parsed);
        }
    };

    function handleEnd() {
        onboard_ = null;
        ping_.stopHeartbeat();
        connected_ = false
        notifyClient(connected_);
        console.log('Onboard disconnected.');
    };

    function notifyClient(connected) {
        app_.Client.send({ connected: connected }, 'onboard');
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
