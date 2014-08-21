function ClientConnection(application) {
    var app = application,
        io = require('socket.io')(application.Http);
        client = null;
    
    io.on('connection', handleNewConnection);
    
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
            switch (eventName) {
                case "video":
                    client.emit('video', data);
                    break;
                default:
                    client.emit('data', data);
            }
        }
    }
    
    function handleNewConnection(socket) {
        // overwrite client's socket => last initiated connection is the ACTIVE one
        client = socket;
        socket.on('disconnect', handleDisconnect);
        socket.on('resend', handleRelay); // TODO: refactor event name resend -> relay
        console.log('Client connected.');
    }
    
    function handleDisconnect() {
        client = null;
        console.log('Client disconnected.');
    }
    
    function handleRelay(data) {
        console.log('>>>>>>>>>>>>>>>> Client relaying data: ');
        console.log(data);
        app.Onboard.send(data + '\n');
    }
    
    return {
        start: start,
        send: send,
    };
}

module.exports = ClientConnection;