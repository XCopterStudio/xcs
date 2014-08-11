var express = require('express');
var server = express();
var http = require('http').createServer(server);
var io = require('socket.io').listen(http);
var OnboardConnection = require('./onboard_connection.js');

// Configuration
server.set('port', 3000);
server.set('views', __dirname + '/client/views');
server.set('view engine', 'hjs');

// Set paths
server.use(server.router);
server.use('/src', express.static(__dirname + '/client/src'));
server.use('/css', express.static(__dirname + '/client/css'));
server.use('/js',  express.static(__dirname + '/client/js'));
server.use('/',    express.static(__dirname + '/client/static'));

server.get('/', function (req, res) {
    var partials = { partials: { styles: 'styles', scripts: 'scripts', main_panel: 'main_panel' }};
    res.render('index.hjs', partials);
});

// Listen for Clients
http.listen(server.get('port'), function () {
    console.log('XCS client service running on port ' + server.get('port'));
});

OnboardConnection.start(1234);
OnboardConnection.startVideoListener(1235);

// WebSockets logic
var client = null;
io.sockets.on('connection', function (socket) {
    client = socket;
    // handling states
    socket.on('disconnect', function () {
        client = null;
    })
    // just resend data from client to onboard
    socket.on('resend', function (cmd) {
        OnboardConnection.send(cmd + '\n');
        console.log('Client COMMAND: ' + cmd);
    });
});

OnboardConnection.on('data', function (data) { if (client) client.emit('data', data); });
OnboardConnection.on('video', function (data) { if (client) client.emit('video', data); });


