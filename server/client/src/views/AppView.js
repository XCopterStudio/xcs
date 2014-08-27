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
    }

});