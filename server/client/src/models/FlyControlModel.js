/*global app*/

var FlyControlModel = Backbone.Model.extend({
    defaults: {
        activeController: 'off',
        roll: 0,
        pitch: 0,
        yaw: 0,
        gaz: 0
    },
    initialize: function() {
    },
    takeOff: function() {
        app.Onboard.sendData({command: 'TakeOff'});
    },
    land: function() {
        app.Onboard.sendData({command: 'Land'});
    },
    setControl: function(controller, control, value) {
        if (controller !== this.get('activeController')) {
            return;
        }
        this.set(control, value);
        this.sendFlyControl_();
    },
    sendFlyControl_: function() {
        var data = {
            flyControl: {
                roll: this.get('roll'),
                pitch: this.get('pitch'),
                yaw: this.get('yaw'),
                gaz: this.get('gaz')
            }
        };
        app.Onboard.sendData(data);
    }
});
