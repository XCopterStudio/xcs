var KeyboardControllerView = Backbone.View.extend({

    el : 'body',

    events : {
        "click #takeoff" : "takeOff",
        "click #land" : "land",
        "keydown" : "keydown",
        "keyup" : "keyup"
    },

    initialize : function() {
        this.model = new ManualControl();
        this.keyMap = {
                65: { action: this.model.setRoll, value: -0.2 }, // A
                68: { action: this.model.setRoll, value: 0.2 }, // D
                83: { action: this.model.setPitch, value: 0.2 }, // S
                87: { action: this.model.setPitch, value: -0.2 }, // W
                37: { action: this.model.setYaw, value: -1.0 }, // left
                39: { action: this.model.setYaw, value: 1.0 }, // right
                38: { action: this.model.setGaz, value: 0.2 }, // up
                40: { action: this.model.setGaz, value: -0.2 } // down
            };
        this.down = {};
    },

    render : function() {

    },

    takeOff : function() {
        this.model.takeOff();
    },

    land : function() {
        this.model.land();
    },

    keydown : function(e) {
        if (e.which == 32 && e.ctrlKey) { // ctrl+space
            app.XcopterState.setMode("free_flight");
        }
        if (e.which == 13 && e.ctrlKey) { // ctrl+enter
            app.XcopterState.setMode("scripting");
        }

        if (this.keyMap[e.which] === undefined) {
            return;
        }
        // comment condition for repeated sending of fly params
        if (this.down[e.which])
            return;
        this.down[e.which] = true;

        command = this.keyMap[e.which]['action'];
        value = this.keyMap[e.which]['value'];
        command(value, this.model);
    },

    keyup : function(e) {
        if (this.keyMap[e.which] === undefined) {
            return;
        }
        this.down[e.which] = false;

        command = this.keyMap[e.which]['action'];
        command(0.0, this.model);
    }

});
