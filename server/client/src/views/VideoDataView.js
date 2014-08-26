var VideoDataView = AbstractDataView.extend({
    // TODO: video element position fixed so it can overflow widget [name], rethink CSS styling
    template: '\
    <li class="widget-line">\
        <div class="text-center"><%= name %></div>\
        <video autoplay width="640" height="480" class="widget-video" id="video_<%= widgetId %>_<%= dataId %>"></video>\
    </li>',

    sizeX: 3,
    
    sizeY: 3,
    
    init: function () {

        window.MediaSource = window.MediaSource || window.WebKitMediaSource;

        this.queue = [];
        this.sourceBuffer = {};
        this.ms = new MediaSource();
        this.video = document.querySelector('video');
        this.video.src = window.URL.createObjectURL(this.ms);
        
        _.bindAll(this, 'processVideoSegments');
        _.bindAll(this, 'appendSourceBuffer');
        _.bindAll(this, 'initSourceBuffer');
        _.bindAll(this, 'resetSourceBuffer');
        
        this.ms.addEventListener('sourceopen', this.initSourceBuffer, false);
        this.ms.addEventListener('sourceclose', this.resetSourceBuffer, false);
        
//        this.ms.addEventListener('sourceended', function(e) { console.log('sourceended: ' + this.ms.readyState); });
//        this.ms.addEventListener('error', function(e) { console.log('error: ' + this.ms.readyState); });

        // disable context menu
        var videoSelector = "#video_" + this.attrs.widgetId + "_" + this.attrs.dataId;
        $(videoSelector).bind('contextmenu',  function () { return false; });
    },
    
    setData: function(data) {
        // empty, data retrieved directly from websocket
    },
    
    initSourceBuffer: function () {
        console.log("MediaSource OPENED.");
        this.queue = [];
        //sourceBuffer = ms.addSourceBuffer('video/mp4;codecs="avc1.4D401F"');
        this.sourceBuffer = this.ms.addSourceBuffer('video/webm;codecs="vp8"');
        this.sourceBuffer.addEventListener('update', this.appendSourceBuffer); // Note: Have tried 'updateend'
        // debugging
//        this.sourceBuffer.addEventListener('updatestart', function(e) { console.log('updatestart: ' + this.ms.readyState); });
//        this.sourceBuffer.addEventListener('update', function(e) { console.log('update: ' + this.ms.readyState); });
//        this.sourceBuffer.addEventListener('updateend', function(e) { console.log('updateend: ' + this.ms.readyState); });
//        this.sourceBuffer.addEventListener('error', function(e) { console.log('error: ' + this.ms.readyState); });
//        this.sourceBuffer.addEventListener('abort', function(e) { console.log('abort: ' + this.ms.readyState); });

        gSocket.on('video', this.processVideoSegments);
        gSocket.emit('relay', JSON.stringify({ type: "onboard", data: "init-video" }));
    },
    
    resetSourceBuffer: function () {
        // TODO: listener should also be removed on DFG destroy action, 
        // which leads to methods onDestroy, onCreate, onStart, onStop on DataViews for similar purposes
        // onStop method shuould fire this method for live camera view to proceed correctly on next start
        gSocket.removeListener('video', this.processVideoSegments);
        this.video.src = window.URL.createObjectURL(this.ms);
    },
    
    processVideoSegments: function (data) {
        this.queue.push(new Uint8Array(data));
        this.appendSourceBuffer();
    //  debugging
//        var i,
//            len,
//            ranges = sourceBuffer.buffered;
//
//        console.log("--STATUS-------");
//        console.log("CURRENT TIME: " + video.currentTime);
//        console.log("BUFFERED RANGES: " + ranges.length);
//        for (i = 0, len = ranges.length; i < len; i += 1) {
//            console.log("RANGE: " + ranges.start(i) + " - " + ranges.end(i));
//        }
//        console.log("---------------");
    },
    
    appendSourceBuffer: function () {
        if (this.queue.length > 0 && !this.sourceBuffer.updating) {
            this.sourceBuffer.appendBuffer(this.queue.shift());
        }
    },
    
});
