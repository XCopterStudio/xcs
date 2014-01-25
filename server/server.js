var express = require('express');
var server = express();

// Configuration
server.set('port', 3000);
server.set('views', __dirname + '/client/views');
server.set('view engine', 'hjs');

// Set paths
server.use('/',    express.static(__dirname + '/client/static'));
server.use('/css', express.static(__dirname + '/client/css'));
server.use('/js',  express.static(__dirname + '/client/js'));

server.get('/', function(req, res) {
    res.render('index');
});

server.listen(server.get('port'), function () {
    console.log('XCS node server running on port ' + server.get('port'));
});