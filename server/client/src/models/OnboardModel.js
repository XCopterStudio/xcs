/* global gSocket */

var OnboardModel = Backbone.Model.extend({

    defaults : {
        "connection" : "Disconnected",
        "flash" : "",
        "data" : "",
        "onboard" : ""
    },
    /*
     * Receiving data with such semantic types triggers sem_update:'the semantic type' event.
     * It's limited with this enumaration for performance purposes.
     */
    notifiedSemanticTypes_: {
        'EXECUTION_STATE': true,
        'EXECUTION_ERROR': true,
        'EXECUTION_OUTPUT': true
    },

    initialize : function() {
        var model = this;
        gSocket.on('data', function(json) {
            if (json.type === "flash") {
                model.set('flash', json.data);
            } else if (json.type === "data") {
                model.set('data', json.data, {silent: true});
                for(var key in json.metadata) {
                    if(json.metadata[key] in model.notifiedSemanticTypes_) {
                        model.trigger('sem_update:' + json.metadata[key], json.data[key]);
                    }
                }
                model.trigger('change:data', model);
            } else if (json.type === "onboard") {
                model.set('onboard', json.data);
            }
        });
    },

    sendData: function (data) {
        var message = { type: 'data', data: data };
        gSocket.emit('resend', JSON.stringify(message));
    }
});
