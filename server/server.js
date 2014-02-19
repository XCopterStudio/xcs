var express = require('express');
var server = express();
var http = require('http').createServer(server);
var io = require('socket.io').listen(http);
var net = require('net');

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

// Listen for Onboard
var connection;
var com = net.createServer(function (conn) {
    if (connection) {
	console.log('Attempting Onboard connect. There\'s one connection already!');
    }
    connection = conn;
    console.log('Onboard CONNECTED');
    conn.on('end', function () {
	connection = null;
	console.log('Onboard DISCONNECTED');
    });
    // Routing data from onboard to client
    conn.on('data', function (data) {
	try {
	    var parsed = JSON.parse(data);
	} catch (e) {
	    parsed = null
	}
	
	if (client && parsed)
	    client.emit('data', parsed);
        console.log(parsed);
    });
});
com.listen(1234, function () {
    console.log('XCS onboard service running on port ' + 1234);
});

// WebSockets logic
var client = null;
io.sockets.on('connection', function (socket) {
    client = socket;
    // handling manual control
    socket.on('init_manual', function () {
	
    });
    socket.on('manual', function (cmd) {
	if (connection)
	    connection.write(cmd + '\n');
	console.log('Client COMMAND: ' + cmd);
    });
    socket.on('term_manual', function () {
	
    });
});


