/* global gSocket */

var ConsoleModel = Backbone.Model.extend({
    allScriptsMap_: {},
    defaults: {
        channelUrbiscript: 'execUrbiscript',
        channelControl: 'execControl',
        responseTimeout: 2000,
        timeoutRef: null,
        error: null,
        output: null,
        scriptModified: false,
        scriptName: null,
        allScripts: null
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
        if (prototypeName === 'Executor') {
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
    saveScript: function(script, name, onFinished) {
        var data = {
            name: name,
            code: script
        };
        var that = this;
        // TODO check rename (or in view?)
        app.Onboard.sendOnboardRequest("SCRIPT_SAVE", data, function(id, responseType, responseData) {
            if (responseType === ResponseType.Error) {
                app.Flash.flashError('Could not save script "' + name + '".');
            } else if (responseType === ResponseType.Done) {
                that.set('scriptName', name);
                that.set('scriptModified', false);

                var scripts = that.get('allScripts');
                if (scripts !== null) {
                    scripts.push(data);
                    that.setScripts_(scripts);
                }
                app.Flash.flashSuccess('Saved script "' + name + '".');
            }
            onFinished();
        });
    },
    loadScript: function(name) {
        if (!name in this.allScriptsMap_) {
            throw ('Unknown script name "' + name + '".');
        }
        this.set('scriptName', name);
        this.set('scriptModified', false);
        return this.allScriptsMap_[name].code;
    },
    deleteScript: function(name, onFinished) {
        var data = {
            name: name,
        };
        var that = this;
        app.Onboard.sendOnboardRequest("SCRIPT_DELETE", data, function(id, responseType, responseData) {
            if (responseType === ResponseType.Error) {
                app.Flash.flashError('Could not delete script "' + name + '".');
                onFinished();
            } else if (responseType === ResponseType.Done) {
                var scripts = that.get('allScripts');
                var idx = null;
                for (var i in scripts) {
                    if (scripts[i].name === name) {
                        idx = i;
                        break;
                    }
                }

                onFinished(); // must be called prior setScripts_
                if (idx !== null) {
                    scripts.splice(idx, 1);
                    that.setScripts_(scripts);
                }

                app.Flash.flashSuccess('Deleted script "' + name + '".');
            }

        });
    },
    loadScripts: function(onFinished) {
        if (this.get('allScripts') !== null) {
            onFinished();
            return;
        }
        var that = this;
        app.Onboard.sendOnboardRequest("SCRIPTS_LOAD", null, function(id, responseType, responseData) {
            if (responseType === ResponseType.Error) {
                app.Flash.flashError('Could not load scripts.');
            } else if (responseType === ResponseType.Done) {
                that.setScripts_(responseData);
                app.Flash.flashInfo('Loaded stored scripts.');
            }
            onFinished();
        });

    },
    setScripts_: function(allScripts) {
        this.allScriptsMap_ = {};
        for (var i in allScripts) {
            var item = allScripts[i];
            this.allScriptsMap_[item.name] = item;
        }
        this.set('allScripts', allScripts);
        this.trigger('change:allScripts', this);
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
