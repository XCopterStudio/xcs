var ConnectionView = Backbone.View.extend({
    
    id: 'connection', // TODO: ids not working? study backbone specs
    
    el: '#connection',
    
    template: ' <div class="nav navbar-nav navbar-right" style="margin-top: 10px; margin-left: 40px;">\
                    <i class="icon-circle" id="led"></i>\
                </div>\
                <div class="nav navbar-nav navbar-right">\
                        <span class="navbar-text">\
                            <span id="lag">LAG:\
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
        console.log('server connected');
        if (!model.get('serverConnected')) {
            this.blinkRed();
            app.Flash.flashError('Server disconnected.');
        }
        else {
            this.shineRed();
            app.Flash.flashInfo('Server connected.')
        }
    },
    
    handleOnboardConnected: function (model) {
        console.log('onboard connected');
        if (model.get('onboardConnected')) {
            this.shineGreen();
            app.Flash.flashInfo('Onboard connected.');
        }
        else {
            this.shineRed();
            app.Flash.flashError('Onboard disconnected.');
        }
    },
    
    handleLatencyChange: function (model) {
        var latency = model.get('latency');
        if (latency == -1) {
            this.$lag.hide();   
        }
        else {
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
