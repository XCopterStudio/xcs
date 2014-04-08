var util = require('util');
var events = require('events');
var net = require('net');

function OnboardConnection() {
    events.EventEmitter.call(this);
}
util.inherits(OnboardConnection, events.EventEmitter);

var OnboardConnection = new OnboardConnection();

var socket = null;

var com = net.createServer(handleConnection);

OnboardConnection.start = function (port) {
    com.listen(port, function () {
        console.log('XCS onboard service running on port ' + port);
    });
};

OnboardConnection.send = function (data) {
    if (socket) {
        socket.write(data);
    }
};

// make sure OnboardConnection is Singleton
module.exports = OnboardConnection;


/*
 * Handler functions
 */

function handleData(data) {
    try {
        var parsed = JSON.parse(data);
    } catch (e) {
        parsed = null;
        console.log(e.message);
    }

    if (parsed) {
        console.log("JSON from onboard parsed.");
        //client.emit('data', parsed);
        OnboardConnection.emit('data', parsed);
        console.log(parsed);
    }
};

function handleEnd() {
    socket = null;
    console.log('Onboard DISCONNECTED');
};

function handleConnection(conn) {
    if (socket) {
        console.log('Attempting Onboard connect. There\'s one connection already!');
        return;
    }
    socket = conn;
    console.log('Onboard CONNECTED');

    socket.on('end', handleEnd);
    socket.on('data', handleData);
};