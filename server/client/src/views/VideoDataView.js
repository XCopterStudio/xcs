var ms,
    sourceBuffer,
    video,
    queue = [];


var VideoDataView = AbstractDataView.extend({
    template: '<video autoplay width="640" height="480"></video>',
    
    initialize: function () {
        AbstractDataView.prototype.initialize.call(this);
        this.sizeY = 3;
        this.sizeY = 2;
        
        // init video
        window.MediaSource = window.MediaSource || window.WebKitMediaSource;

        ms = new MediaSource();

        video = document.querySelector('video');
        video.src = window.URL.createObjectURL(ms);

        ms.addEventListener('webkitsourceopen', initSourceBuffer, false); // TODO: not supported any more?
        ms.addEventListener('sourceopen', initSourceBuffer, false);
        
        // debugging
        ms.addEventListener('webkitsourceclose', function(e) { console.log('webkitsourceclose: ' + ms.readyState); }, false);
        ms.addEventListener('webkitsourceopen', function(e) { console.log('webkitsourceopen: ' + ms.readyState); }, false);
        
        ms.addEventListener('sourceopen', function(e) { console.log('sourceopen: ' + ms.readyState); });
        ms.addEventListener('sourceclose', function(e) { console.log('sourceclose: ' + ms.readyState); });
        ms.addEventListener('sourceended', function(e) { console.log('sourceended: ' + ms.readyState); });
        ms.addEventListener('error', function(e) { console.log('error: ' + ms.readyState); });

        
        // incomming video processing
        gSocket.on('video', function (data) {
            
            if (sourceBuffer.updating || queue.length > 0) {
                queue.push(new Uint8Array(data));
            } else {
                sourceBuffer.appendBuffer(new Uint8Array(data));
            }
            
            // debugging
            var i,
                len,
                ranges = sourceBuffer.buffered;

            console.log("--STATUS-------");
            console.log("CURRENT TIME: " + video.currentTime);
            console.log("BUFFERED RANGES: " + ranges.length);
            for (i = 0, len = ranges.length; i < len; i += 1) {
                console.log("RANGE: " + ranges.start(i) + " - " + ranges.end(i));
            }
            console.log("---------------");
            
        });
    },
    
    setData: function(data) {
    },
    
    
});

function initSourceBuffer() {

    console.log("MediaSource OPENED.");

    //sourceBuffer = ms.addSourceBuffer('video/mp4;codecs="avc1.4D401F"');
    sourceBuffer = ms.addSourceBuffer('video/webm;codecs="vp8"');
    
    // debugging
//    sourceBuffer.addEventListener('updatestart', function(e) { console.log('updatestart: ' + ms.readyState); });
//    sourceBuffer.addEventListener('update', function(e) { console.log('update: ' + ms.readyState); });
//    sourceBuffer.addEventListener('updateend', function(e) { console.log('updateend: ' + ms.readyState); });
    sourceBuffer.addEventListener('error', function(e) { console.log('error: ' + ms.readyState); });
    sourceBuffer.addEventListener('abort', function(e) { console.log('abort: ' + ms.readyState); });
    
    sourceBuffer.addEventListener('update', function() { // Note: Have tried 'updateend'
        if (queue.length > 0 && !sourceBuffer.updating) {
            sourceBuffer.appendBuffer(queue.shift());
        }
    });
}