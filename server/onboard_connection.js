function OnboardConnection(application) {
    
    var app = application,
        net = require('net'),
        onboard = null,
        com = net.createServer(handleNewConnection);
    
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
    }

    function send(data) {
        if (onboard) {
            onboard.write(data);
        }
    };
    
    function handleNewConnection(socket) {
        if (onboard) {
            console.log('Attempting onboard connect. There\'s one connection already!');
            return;
        }
        onboard = socket;
        socket.on('end', handleEnd);
        socket.on('data', handleData);
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
            console.log('<<<<<<<<<<<<<<<< JSON from onboard parsed and relaying:');
            app.Client.send(parsed);
            console.log(parsed);
        }
    };

    function handleEnd() {
        onboard = null;
        console.log('Onboard disconnected.');
    };
    
    return {
        start: start,
        startVideo: startVideo,
        send: send,
    }
}

module.exports = OnboardConnection;
