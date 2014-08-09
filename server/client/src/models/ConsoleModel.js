/* global gSocket */

var ConsoleModel = Backbone.Model.extend({
    defaults: {
        channelState: 'executor_state',
        channelError: 'executor_error',
        channelOutput: 'executor_output',
        channelUrbiscript: 'execUrbiscript',
        channelControl: 'execControl',
        responseTimeout: 2000,
        timeoutRef: null,
        error: null,
        output: null,
    },
    initialize: function() {
        this.set('state', ConsoleModel.State.IDLE); // cannot be in default because of ConsoleModel.State "visibility"
        this.listenTo(app.Onboard, "change:data", this.onDataChange);
    },
    onDataChange: function(onboard) {
        var data = onboard.get('data');

        // update state
        var stateKey = this.get('channelState');
        if ((stateKey in data) && data[stateKey]) {
            if (this.get('timeoutRef')) {
                window.clearTimeout(this.get('timeoutRef'));
            }
            var value = data[stateKey];
            // trigger on update, not only change
            this.set('state', value, {silent: true});
            this.trigger('change:state', this);
        }

        // update error
        var errorKey = this.get('channelError');
        if ((errorKey in data) && data[errorKey]) {
            // trigger on update, not only change (error may stay same)
            this.set('error', data[errorKey], {silent: true});
            this.trigger('change:error', this);
        }
        // update output
        var outputKey = this.get('channelOutput');
        if ((outputKey in data) && data[outputKey]) {
            // trigger on update, not only change (output may stay same)
            this.set('output', data[outputKey], {silent: true});
            this.trigger('change:output', this);
        }
    },
    /* Executive functions */
    start: function() {
        var codeData = {};
        this.set('error', null);
        codeData[this.get('channelUrbiscript')] = this.get('urbiscript');
        app.Onboard.sendData(codeData);

        var controlData = {};
        controlData[this.get('channelControl')] = 'run';
        app.Onboard.sendData(controlData);
        this.wait_();
    },
    pause: function() {
        var controlData = {};
        controlData[this.get('channelControl')] = 'freeze';
        app.Onboard.sendData(controlData);
        this.wait_();
    },
    stop: function() {
        var controlData = {};
        controlData[this.get('channelControl')] = 'stop';
        app.Onboard.sendData(controlData);
        this.wait_();
    },
    resume: function() {
        var controlData = {};
        controlData[this.get('channelControl')] = 'unfreeze';
        app.Onboard.sendData(controlData);
        this.wait_();
    },
    wait_: function() {
        this.set('state', ConsoleModel.State.WAITING);
        var that = this;
        this.set('timeoutRef', window.setTimeout(function() {
            app.Flash.flashError('Onboard console is not responding.');
            that.set('state', ConsoleModel.State.IDLE);
        }, this.get('responseTimeout')));
    }
});

ConsoleModel.State = {
    IDLE: 'idle',
    WAITING: 'waiting',
    RUNNING: 'running',
    FREEZED: 'freezed'
};
