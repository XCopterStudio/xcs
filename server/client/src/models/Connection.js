var gSocket;

var Connection = Backbone.Model.extend({

    defaults: {
        "sessionOccupied": false,
        "serverConnected": false,
        "onboardConnected" : false,
        "latency": -1,
    },

    responseCallbacks: {},

    latencyMonitor: null,

    latencyMonitorInterval: 3000, //ms

    initialize: function () {
        var model = this;

        _.bindAll(this, 'setLatency');

        // connect to server
        gSocket = io.connect('http://' + location.host);

        // set event handlers
        gSocket.on('connect', function () {
            // empty
        });
        gSocket.on('disconnect', function () {
            clearInterval(model.latencyMonitor);
            model.set('serverConnected', false);
        });
        gSocket.on('session acquired', function () {
            model.set('serverConnected', true);
            // init latency monitoring
            clearInterval(model.latencyMonitor);
            model.latencyMonitor = setInterval(function () {
                model.call('getLatency', model.setLatency);
            }, model.latencyMonitorInterval);
            // ask server if onboard is connected
            model.call('isOnboardConnected', function (connected) {
                model.set('onboardConnected', connected);
            });
        });
        gSocket.on('session occupied',  function () {
            model.set('sessionOccupied', true);
        });
        // reflect server's ping
        gSocket.on('ping', function(payload) {
            gSocket.emit('ping echo', payload);
        });
        // handle returned RPC-like calls
        gSocket.on('call return', function (data) {
            var methodName = data['method'],
                returned = data['returned'];
            if (methodName in model.responseCallbacks) {
                model.responseCallbacks[methodName](returned);
                delete model.responseCallbacks[methodName];
            }
            else {
                console.warn('Unknown method: ' + methodName);
            }
        });
        // handle push notificaions from server's OnboardConnection
        gSocket.on('onboard', function (info) {
            if ('connected' in info) {
                model.set('onboardConnected', info['connected']);
            }
        });
    },

    call: function (methodName, responseCallback) {
        // register callback
        this.responseCallbacks[methodName] = responseCallback;
        gSocket.emit('call', methodName);
    },

    setLatency: function (latency) {
        this.set('latency', latency);
    },
});