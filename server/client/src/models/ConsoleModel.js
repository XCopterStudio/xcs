/* global gSocket */

var ConsoleModel = Backbone.Model.extend({
    defaults: {
        channelUrbiscript: 'execUrbiscript',
        channelControl: 'execControl',
        responseTimeout: 2000,
        timeoutRef: null,
        error: null,
        output: null,
    },
    initialize: function() {
        this.set('state', ConsoleModel.State.IDLE); // cannot be in default because of ConsoleModel.State "visibility"
        this.listenTo(app.Onboard, "sem_update:EXECUTION_STATE", this.onStateChange);
        this.listenTo(app.Onboard, "sem_update:EXECUTION_ERROR", this.onErrorChange);
        this.listenTo(app.Onboard, "sem_update:EXECUTION_OUTPUT", this.onOutputChange);
        this.listenTo(app.DataFlowGraph.model, 'nodeStop', this.onNodeStop);
    },
    onStateChange: function(value) {
        if (this.get('timeoutRef')) {
            window.clearTimeout(this.get('timeoutRef'));
        }
        // trigger on update, not only change
        this.set('state', value, {silent: true});
        this.trigger('change:state', this);
    },
    onErrorChange: function(value) {
        // trigger on update, not only change (error may stay same)
        this.set('error', value, {silent: true});
        this.trigger('change:error', this);
    },
    onOutputChange: function(value) {
        // trigger on update, not only change (output may stay same)
        this.set('output', value, {silent: true});
        this.trigger('change:output', this);
    },    
    onNodeStop: function(model) {
        var prototypeName = model.get('origId');
        if(prototypeName === 'Executor') {
            this.set('state', ConsoleModel.State.IDLE);
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
            app.Flash.flashError('Onboard console is not responding. Check that you have an Executor node created and it\'s running.');
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
