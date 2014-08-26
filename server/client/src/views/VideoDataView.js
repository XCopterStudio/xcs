var VideoDataView = AbstractDataView.extend({
    // TODO: video element position fixed so it can overflow widget [name], rethink CSS styling
    template: '\
    <li class="widget-line">\
        <div class="text-center"><%= name %></div>\
        <div class="widget-video-container">\
            <video autoplay class="widget-video" id="video_<%= widgetId %>_<%= dataId %>"></video>\
            <canvas id="canvas_<%= widgetId %>_<%= dataId %>"></canvas>\
        </div>\
    </li>',
    sizeX: 3,
    sizeY: 3,
    width: 640,
    height: 360,
    init: function() {

        window.MediaSource = window.MediaSource || window.WebKitMediaSource;

        var idVideo = "video_" + this.attrs.widgetId + "_" + this.attrs.dataId;
        var idCanvas = "canvas_" + this.attrs.widgetId + "_" + this.attrs.dataId;

        this.queue = [];
        this.sourceBuffer = {};
        this.ms = new MediaSource();

        /*
         * GUI elements
         */
        this.$video = $('#' + idVideo);
        this.elVideo = this.$video.get(0);
        this.elVideo.src = window.URL.createObjectURL(this.ms);
        this.elVideo.width = this.width;
        this.elVideo.height = this.height;
        this.$video.css({top: 0, left: 0});

        this.$canvas = $('#' + idCanvas);
        this.elCanvas = this.$canvas.get(0);
        this.elCanvas.width = this.elVideo.width;
        this.elCanvas.height = this.elVideo.height;
        this.$canvas.css({top: 0, left: 0});
        this.canvasContext = this.elCanvas.getContext('2d');

        var $container = this.$canvas.parent();
        $container.width(this.elVideo.width);
        $container.height(this.elVideo.height);

        /*
         * Events
         */
        _.bindAll(this, 'processVideoSegments');
        _.bindAll(this, 'appendSourceBuffer');
        _.bindAll(this, 'initSourceBuffer');
        _.bindAll(this, 'resetSourceBuffer');

        this.ms.addEventListener('sourceopen', this.initSourceBuffer, false);
        this.ms.addEventListener('sourceclose', this.resetSourceBuffer, false);

//        this.ms.addEventListener('sourceended', function(e) { console.log('sourceended: ' + this.ms.readyState); });
//        this.ms.addEventListener('error', function(e) { console.log('error: ' + this.ms.readyState); });

        this.listenTo(app.Onboard, "sem_update:ROTATION", this.onRotationChange);


        /*
         * Initialization
         */
        // disable context menu
        $(this.elVideo).bind('contextmenu', function() {
            return false;
        });

    },
    setData: function(data) {
        // empty, data retrieved directly from websocket
    },
    /*
     * Buffer handling
     */
    initSourceBuffer: function() {
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
        gSocket.emit('relay', JSON.stringify({type: "onboard", data: "init-video"}));
    },
    resetSourceBuffer: function() {
        // TODO: listener should also be removed on DFG destroy action, 
        // which leads to methods onDestroy, onCreate, onStart, onStop on DataViews for similar purposes
        // onStop method shuould fire this method for live camera view to proceed correctly on next start
        gSocket.removeListener('video', this.processVideoSegments);
        this.elVideo.src = window.URL.createObjectURL(this.ms);
    },
    processVideoSegments: function(data) {
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
    appendSourceBuffer: function() {
        if (this.queue.length > 0 && !this.sourceBuffer.updating) {
            this.sourceBuffer.appendBuffer(this.queue.shift());
        }
    },
    /*
     * Events
     */
    onRotationChange: function(data) {
        this.drawCross(0, 0, 0, true);
        this.drawCross(data.phi, 0, 300 * Math.sin(data.theta), false);
        //console.log(data);
    },
    /*
     * Utility
     */
    drawCross: function(rotation, shiftX, shiftY, clear) {
        var context = this.canvasContext;
        var w2 = this.elCanvas.width / 2;
        var h2 = this.elCanvas.height / 2;

        context.setTransform(1, 0, 0, 1, 0, 0);
        if (clear) {
            context.clearRect(0, 0, 2 * w2, 2 * h2);
        }

        context.translate(w2, h2);
        context.rotate(rotation);
        context.translate(shiftX, shiftY);


        context.beginPath();
        context.moveTo(-w2, 0);
        context.lineTo(w2, 0);
        context.moveTo(0, -h2);
        context.lineTo(0, h2);
        context.stroke();
    }


});
