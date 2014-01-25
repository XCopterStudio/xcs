var express = require('express');
var server = express();

// Configuration
server.set('port', 3000);
server.set('views', __dirname + '/client/views');
server.set('view engine', 'hjs');

// Set paths
server.use(server.router);
server.use('/css', express.static(__dirname + '/client/css'));
server.use('/js',  express.static(__dirname + '/client/js'));
server.use('/',    express.static(__dirname + '/client/static'));

server.get('/', function(req, res) {
    var partials = { partials: { styles: 'styles', scripts: 'scripts', main_panel: 'main_panel' }};
    res.render('index.hjs', partials);
});

server.listen(server.get('port'), function () {
    console.log('XCS node server running on port ' + server.get('port'));
});