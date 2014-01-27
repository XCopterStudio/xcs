var socket = io.connect('http://localhost:3000');
socket.on('news', function(data) {
    console.log(data);
    socket.emit('my other event', {my: 'data'});
});

function cmd(data) {
    socket.emit('manual', data);
}

function flyCmd() {
    var data = '('
            + flyParams.roll + ','
            + flyParams.pitch + ','
            + flyParams.yaw + ','
            + flyParams.gaz +
            ')';
    cmd(data);
}

var flying = false;

var flyParams = {
    roll: 0.0,
    pitch: 0.0,
    yaw: 0.0,
    gaz: 0.0
};

var keyMap = {
    65: ['roll', -1.0], // A
    68: ['roll', 1.0], // D
    83: ['pitch', -0.5], // S
    87: ['pitch', 0.5], // W
    37: ['yaw', -1.0], // left
    39: ['yaw', 1.0], // right
    38: ['gaz', 0.2], // up
    40: ['gaz', -0.2] // down
};
var down = {};


$('#takeoff').click(function() {
    cmd('TakeOff');
//    $(this).hide();
//    $('#land').show();
    flying = true;
});
$('#land').click(function() {
    cmd('Land');
//    $(this).hide();
//    $('#takeoff').show();
    flying = false;
});

$('body').keydown(function(e) {
    if (keyMap[e.which] === undefined) {
        return;
    }
    // uncomment for not-repeated sending of fly params
    //if(down[e.which]) return;
    down[e.which] = true;

    var coor = keyMap[e.which][0];
    var value = keyMap[e.which][1];
    flyParams[coor] = value;
    flyCmd();
});
$('body').keyup(function(e) {
    if (keyMap[e.which] === undefined) {
        return;
    }
    down[e.which] = false;

    var coor = keyMap[e.which][0];
    var value = keyMap[e.which][1];
    flyParams[coor] = 0;
    flyCmd();
});

// uncomment to send zeroes periodically
//window.setInterval(function() {
//    if (flying) {
//        flyCmd();
//    }
//}, 50);