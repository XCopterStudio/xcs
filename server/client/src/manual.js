var socket = io.connect('http://localhost:3000');
socket.on('news', function (data) {
    console.log(data);
    socket.emit('my other event', { my: 'data' });
});

function cmd(data) {
    socket.emit('manual', data);
}

$('#takeoff').click(function() {
    cmd('TakeOff');
    $(this).hide();
    $('#land').show();
});
$('#land').click(function(){
    cmd('Land');
    $(this).hide();
    $('#takeoff').show();
    //alert('Cannot land. The video takes 2:11.');
});