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

    //create app
    app = new AppView();
    
    //set models 
    app.Onboard = new OnboardModel();
    
    //set standard views
    app.Wait = new WaitView();
    app.Flash = new FlashMessagesView();
    
    //set dfg view
    app.DataFlowGraph = new DataFlowGraphView();

    //set view for widgets
    app.DataView = new DataView();
    app.DataView.addView("raw", "battery");
    app.DataView.addView("raw", "altitude");
    app.DataView.addView("raw", "phi");
    app.DataView.addView("raw", "psi");
    app.DataView.addView("raw", "theta");
});