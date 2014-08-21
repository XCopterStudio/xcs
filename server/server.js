var app = {};

var express = require('express');
var server = express();
var http = require('http').createServer(server);
var config = require('./config.json');

app.Server = server;
app.Http = http;
app.Config = config;

// Configuration
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

app.Onboard = require('./onboard_connection.js')(app);
app.Client = require('./client_connection.js')(app);

app.Client.start(app.Config.clientPort);
app.Onboard.start(app.Config.onboardPort);
app.Onboard.startVideo(app.Config.onboardVideoHost, app.Config.onboardVideoPort);



