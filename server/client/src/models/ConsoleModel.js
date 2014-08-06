/* global gSocket */

var ConsoleModel = Backbone.Model.extend({
    defaults: {
        channelState: 'executor_state',
        channelUrbiscript: 'execUrbiscript',
        channelControl: 'execControl'
    },
    initialize: function() {
        this.set('state', ConsoleModel.State.IDLE);
        this.listenTo(app.Onboard, "change:data", this.onDataChange);
    },
    onDataChange: function(onboard) {
        var data = onboard.get('data');
        var key = this.get('channelState');

        if (!(key in data) || !data[key]) {
            return;
        }

        var value = data[key];
        this.set('state', value);
    },
    /* Executive functions */
    start: function() {
        var codeData = {};
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
    }
});

ConsoleModel.State = {
    IDLE: 'idle',
    WAITING: 'waiting',
    RUNNING: 'running',
    FREEZED: 'freezed',
    DEAD: 'dead'
};
