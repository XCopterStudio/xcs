/* global gSocket */
var ResponseType = ENUM("Done", "Error");

var OnboardModel = Backbone.Model.extend({
    requestId_: 1,
    maxRequestId_: 1000,
    responses: {
        addResponse: function(requestId, response) {
            this[requestId] = response;
        },
        getAndRemoveResponse: function(requestId) {
            if (this[requestId]) {
                var response = this[requestId];
                delete this[requestId];
                return response;
            }
        },
    },
    defaults: {
        "connection": "Disconnected",
        "flash": "",
        "data": "",
        "onboard": ""
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
    initialize: function() {
        var that = this;
        gSocket.on('data', function(json) {
            if (json.type === "flash") {
                that.set('flash', json.data);
            } else if (json.type === "data") {
                that.set('data', json.data, {silent: true});
                for (var key in json.metadata) {
                    if (json.metadata[key] in that.notifiedSemanticTypes_) {
                        that.trigger('sem_update:' + json.metadata[key], json.data[key]);
                    }
                }
                that.trigger('change:data', that);
            } else if (json.type === "onboard") {
                that.receiveOnboardData(json.data);
            }
        });
    },
    sendData: function(data) {
        var message = {type: 'data', data: data};
        gSocket.emit('resend', JSON.stringify(message));
    },
    sendOnboardRequest: function(request, data, response) {
        if (!data) {
            data = "";
        }

        // set reaquest id and prepare new one
        var rId = this.requestId_;
        if (++this.requestId_ > this.maxRequestId_) {
            this.requestId_ = 1;
        }

        // register response action 4 current request id
        if (response) {
            this.responses.addResponse(rId, response);
        }

        //create request message
        var requestData = {
            type: "onboard",
            data: {
                request: {
                    id: request,
                    requestData: data,
                    requestId: rId,
                }
            }
        };

        // try send request
        try {
            gSocket.emit('resend', JSON.stringify(requestData));
        }
        catch (ex) {
            // TODO: log exception

            // remove response action
            this.responses.getAndRemoveResponse(rId);
        }

        return rId;
    },
    receiveOnboardData: function(data) {
        if (data.response) {
            if (data.response.id && data.response.respondType && data.response.requestId) {
                var id = data.response.id
                var responseTypeText = data.response.respondType;
                var responseData = data.response.respondData;
                var requestId = data.response.requestId;

                var responseType;
                switch (responseTypeText) {
                    case "Done":
                        responseType = ResponseType.Done;
                        break;
                    case "Error":
                        responseType = ResponseType.Error;
                        break;
                    default:
                        responseType = ResponseType.None;
                        break;
                }

                // run response action
                var responseAction = this.responses.getAndRemoveResponse(requestId);
                if (responseAction) {
                    responseAction(id, responseType, responseData);
                }
            }
            else {
                //TODO: bad format data  - what to do?
            }
        }
        this.set('onboard', data);
    }
});
