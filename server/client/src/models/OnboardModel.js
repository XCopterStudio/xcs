/* global gSocket */

var OnboardModel = Backbone.Model.extend({

    defaults : {
        "connection" : "Disconnected",
        "flash" : "",
        "data" : "",
        "onboard" : ""
    },

    initialize : function() {
        var model = this;
        gSocket.on('data', function(json) {
            if (json.type == "flash") {
                console.log('>>> Setting flash data');
                model.set('flash', json.data);
            } else if (json.type == "data") {
                model.set('data', json.data);
                //console.log("incoming");
            } else if (json.type == "onboard") {
                model.set('onboard', json.data);
            }
        });
    },

    setMode: function (mode) {
        data = {
            "type": "onboard",
            "data": { "mode": mode }
        };
        gSocket.emit('resend', JSON.stringify(data));
    }
});
