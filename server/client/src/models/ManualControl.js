/*global gSocket*/

function Controller() {
    function cmd(data) {
        gSocket.emit('resend', data);
    }
    this.takeOff = function() {
        data = {
            type : "data",
            data : {
                command : "TakeOff"
            }
        }
        cmd(JSON.stringify(data));
    };
    this.land = function() {
        data = {
            type : "data",
            data : {
                command : "Land"
            }
        }
        cmd(JSON.stringify(data));
    };
    this.fly = function(rollValue, pitchValue, yawValue, gazValue) {
        data = {
            type : "data",
            data : {
                roll : rollValue,
                pitch : pitchValue,
                yaw : yawValue,
                gaz : gazValue
            }
        }
        cmd(JSON.stringify(data));
        console.log({
            roll : rollValue,
            pitch : pitchValue,
            yaw : yawValue,
            gaz : gazValue
        });
    };
}

var ManualControl = Backbone.Model.extend({

    initialize : function() {
        this.xcopter = new Controller();
        // flying paramters
        this.roll = 0.0;
        this.pitch = 0.0;
        this.yaw = 0.0;
        this.gaz = 0.0;
        this.sendFlyingCommand = function() {
            this.xcopter.fly(this.roll, this.pitch, this.yaw, this.gaz);
        };
    },

    takeOff : function() {
        this.xcopter.takeOff();
    },

    land : function() {
        this.xcopter.land();
    },

    setRoll : function(value, model) {
        model.roll = value;
        model.sendFlyingCommand();
    },

    setPitch : function(value, model) {
        model.pitch = value;
        model.sendFlyingCommand();
    },

    setYaw : function(value, model) {
        model.yaw = value;
        model.sendFlyingCommand();
    },

    setGaz : function(value, model) {
        model.gaz = value;
        model.sendFlyingCommand();
    },

    setFly : function(roll, pitch, yaw, gaz, model){
        model.roll = roll;
        model.pitch = pitch;
        model.yaw = yaw;
        model.gaz = gaz;
        model.sendFlyingCommand();
    }
});
