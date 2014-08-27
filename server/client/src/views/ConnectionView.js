var ConnectionView = Backbone.View.extend({

    id: 'connection', // TODO: ids not working? study backbone specs

    el: '#connection',

    template: ' <div class="nav navbar-nav navbar-right" style="margin-top: 10px; margin-left: 40px;">\
                    <i class="icon-circle" id="led"></i>\
                </div>\
                <div class="nav navbar-nav navbar-right">\
                        <span class="navbar-text" xtitle="Sum of latencies between client–server and server–onboard.">\
                            <span id="lag">Lag:\
                                <span class="value" style="padding-left: 10px;"></span>\
                            ms</span>\
                        </span>\
                </div>',

    colors: {
        red: "#E57272",
        yellow: "#E5D172",
        green: "#5BB75B"
    },

    ledSize: 30,

    initialize: function () {
        this.model = app.Connection;

        _.bindAll(this, 'blink');

        var compiledTemplate = _.template(this.template);
        this.$el.html(compiledTemplate);

        this.$led = this.$('#led');
        this.$led.css({ fontSize: this.ledSize });
        this.$lag = this.$('#lag');
        //this.$lag.css({ fontSize: this.ledSize });
        this.$lag.hide();

        this.listenTo(this.model, 'change:serverConnected', this.handleServerConnected);
        this.listenTo(this.model, 'change:onboardConnected', this.handleOnboardConnected);
        this.listenTo(this.model, 'change:latency', this.handleLatencyChange);
    },

    /*
     * Led switchers
     */
    blinkRed: function () {
        this.stopBlink();
        this.setColor('red');
        this.startBlink();
    },

    shineRed: function () {
        this.stopBlink();
        this.setColor('red');
    },

    shineGreen: function () {
        this.stopBlink();
        this.setColor('green');
    },

    /*
     * Handlers
     */
    handleServerConnected: function (model) {
        if (!model.get('serverConnected')) {
            this.blinkRed();
            this.$lag.hide();
            app.Flash.flashError(this.setLedTitle('Server disconnected.'));
        } else {
            this.shineRed();
            app.Flash.flashInfo(this.setLedTitle('Server connected.'))
        }
    },

    handleOnboardConnected: function (model) {
        if (model.get('onboardConnected')) {
            this.shineGreen();
            app.Flash.flashInfo(this.setLedTitle('Onboard connected.'));
        } else {
            this.shineRed();
            app.Flash.flashError(this.setLedTitle('Onboard disconnected.'));
        }
    },

    handleLatencyChange: function (model) {
        var latency = model.get('latency');
        if (latency == -1) {
            this.$lag.hide();
        } else {
            this.$lag.show();
            $('.value', this.$lag).html(latency);
        }
    },

    /*
     * Helper functions
     */
    setColor: function (color) {
        if (color in this.colors) {
            this.$led.css({ color: this.colors[color] });
        }
    },

    setLedTitle: function(title) {
        this.$led.attr('xtitle', title).xtooltip();
        return title; // NOTE: this is hack to co-implementation with flash message
    },

    blink: function () {
        this.$led.fadeOut(100).fadeIn(); // TODO: this seems to take much CPU time
    },

    startBlink: function () {
        this.blinker = setInterval(this.blink, 1000);
    },

    stopBlink: function () {
        clearInterval(this.blinker);
    },
});
