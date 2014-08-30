/*global Backbone*/

var AppView = Backbone.View.extend({

    el: 'body',

    settings: {
        latencyTreshold: { value: 1000 },
    },

    // TODO: rewrite Settings model to object with keys instead of list of objects
    // see settings above
    settings_: [
        { name: 'notify latency', type: SettingType.NUMBER, default: 1000 },
    ],

    initialize: function() {
        var app = this;

        // create models
        app.Connection = new Connection();
        app.Onboard = new OnboardModel();

        // disable app if session with server is occupied
        this.listenTo(app.Connection, 'change:sessionOccupied', function (model) {
            if (model.get('sessionOccupied')) {
                app.ModalView.showModal('#modal-session-occupied', { backdrop: 'static', keyboard: false });
                window.onbeforeunload = null;
            } else {
                $('#modal-sesion-occupied').modal('hide');
            }
        });

        // set warning dialog on refreshing or leaving page
        window.onbeforeunload = function() {
            return "X-copter plan execution will be stopped!"; //Are you sure you want to navigate away?
        }

        // init dropdowns
        this.$('.dropdown-toggle').dropdown();
        this.$('.dropdown-menu input, .dropdown-menu label, .dropdown-menu .dropdown-header').click(function(e) {
            e.stopPropagation();
        });

        _.bindAll(this, 'setSettings');

        this.settingsModel = new Setting('settings', this.settings_, this.setSettings, this.validateSettings);
        this.settingsModel.load();

        this.$settings = this.$('#settings');

        var that = this;
        this.$settings.click(function () {
            var model = new Setting('settings', that.settings_, that.setSettings, that.validateSettings);
            that.SettingView.show(model);
        });

    },

    initViews: function () {
        var app = this;

        //initialize views
        app.Wait = new WaitView();
        app.Flash = new FlashMessagesView();
        app.StateView = new StateView();
        app.ModalView = new ModalView();
        app.SettingView = new SettingView();
        app.ConnectionView = new ConnectionView();

        app.FlyControlView = new FlyControlView();
        app.DataView = new DataView();
        app.DataFlowGraph = new DataFlowGraphView();
        app.ConsoleView = new ConsoleView();

        app.BottomToolbarView = new BottomToolBarView();
        app.BottomToolbarView.addView("dfg-trigger", "dfg", app.DataFlowGraph);
        app.BottomToolbarView.addView("console-trigger", "console");
    },

    setSettings: function (data) {
        for (var i = 0; i < data.length; i++) {
            var setting = data[i];
            switch (setting.name) {
                case 'notify latency':
                    this.settings.latencyTreshold.value = setting.value;
                    this.settings_[0].default = setting.value; // TODO: see todo above settings_
                    break;
                default:
                    break;
            }
        }
    },

    validateSettings: function (data) {
        var error = [];
        for (var i = 0; i < data.length; i++) {
            var setting = data[i];
            if (setting.name == 'notify latency' && setting.value <= 0) {
                error.push({
                    name: setting.name,
                    msg: 'Notifications for latency lower than 1ms makes no sense.'
                });
            }
        }
        return error;
    },

    setEnabledOnboard: function (enabled) {
        if (enabled) {
            $('#bottom-toolbar button').removeClass('disabled');
        }
        else {
            $('#bottom-toolbar button').addClass('disabled');
        }
    }

});