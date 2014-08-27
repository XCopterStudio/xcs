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

    init: function() {

        window.MediaSource = window.MediaSource || window.WebKitMediaSource;

        var idVideo = "video_" + this.attrs.widgetId + "_" + this.attrs.dataId;
        var idCanvas = "canvas_" + this.attrs.widgetId + "_" + this.attrs.dataId;

        this.queue = [];
        this.sourceBuffer = {};
        this.ms = new MediaSource();

        this.createDefaultSettings();
        /*
         * GUI elements
         */
        this.$video = $('#' + idVideo);
        this.elVideo = this.$video.get(0);
        this.elVideo.src = window.URL.createObjectURL(this.ms);

        this.$canvas = $('#' + idCanvas);
        this.elCanvas = this.$canvas.get(0);

        this.canvasContext = this.elCanvas.getContext('2d');

        this.setDimensions(this.width, this.height);

        /*
         * Events
         */
        _.bindAll(this, 'processVideoSegments');
        _.bindAll(this, 'appendSourceBuffer');
        _.bindAll(this, 'initSourceBuffer');
        _.bindAll(this, 'resetSourceBuffer');

        this.ms.addEventListener('sourceopen', this.initSourceBuffer, false);
        this.ms.addEventListener('sourceclose', this.resetSourceBuffer, false);

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
        console.log('MediaSource element initiated.');
        this.queue = [];
        this.sourceBuffer = this.ms.addSourceBuffer('video/webm;codecs="vp8"');
        this.sourceBuffer.addEventListener('update', this.appendSourceBuffer); // Note: Have tried 'updateend'

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
        var clear = true;
        if(this.showCentral) {
            this.drawCross(0, 0, 0, clear, '#444444');
            clear = false;
        }
        if(this.showAttitude) {
            this.drawCross(-data.phi, 0, this.pitchFactor * Math.sin(data.theta), clear);
        }
    },

    /*
     * Utility
     */
    drawCross: function(rotation, shiftX, shiftY, clear, strokeStyle) {
        strokeStyle = (strokeStyle === undefined) ? '#000000' : strokeStyle;

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
        context.strokeStyle = strokeStyle;
        context.stroke();
    },

    setDimensions: function(width, height) {
        // preserve original values when dimension not set
        width = (width === null) ? this.elVideo.width : width;
        height = (height === null) ? this.elVideo.height : height;

        var $container = this.$canvas.parent();

        this.elVideo.width = width;
        this.elCanvas.width = this.elVideo.width;
        $container.width(this.elVideo.width);

        this.elVideo.height = height;
        this.elCanvas.height = this.elVideo.height;
        $container.height(this.elVideo.height);

        this.$video.css({top: 0, left: 0});
        this.$canvas.css({top: 0, left: 0});
    },

    /*
     * Settings
     */
    settings: {
        width: {name: "width (pixels)", type: SettingType.NUMBER, default: 640},
        height: {name: "height (pixels)", type: SettingType.NUMBER, default: 360},
        showCentral: {name: "show central cross", type: SettingType.BOOLEAN, default: true},
        showAttitude: {name: "show attitude cross", type: SettingType.BOOLEAN, default: true},
        pitchFactor: {name: "pitch factor", type: SettingType.NUMBER, default: 300},
    },

    validateSettings: function(settings) {
        var result = [];
        return result; // TODO validation
    },

    setSettings: function(settings) {
        for(var i in settings) {
            var setting = settings[i];
            for(var key in this.settings) {
                var metaSettings = this.settings[key];
                if(metaSettings.name === setting.name) {
                    this[key] = setting.value;
                }
            }
        }

        this.setDimensions(this.width, this.height);
    },
});
