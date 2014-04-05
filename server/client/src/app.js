/*global $ */
/*jshint unused:false */
var app = app || {};

gSocket = io.connect('http://localhost:3000');

var router = Backbone.Router.extend({
    routes : {
        "manual": "manual"
    },

    manual : function () {
	var view = new ManualControl({model: null});
    }
});

$(function () {
    'use strict';

    app = new AppView();
    app.XcopterState = new XcopterState();
    app.Flash = new FlashMessagesView();
    app.Navdata = new AbstractNavdataView();
    app.ScriptGeneratorView = new ScriptGeneratorView();
//    app.Navdata.onDataChange({ type: "data", data: { altitude: "2.3" }});
//    app.Navdata.onDataChange({ type: "data", data: { phi: "4.3" }});
//    app.Navdata.onDataChange({ type: "data", data: { psi: "1.3" }});
//    app.Navdata.onDataChange({ type: "data", data: { theta: "0.3" }});
//    app.Navdata.onDataChange({ type: "data", data: { theta: "6.2" }});
});