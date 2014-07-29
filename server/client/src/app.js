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

    //set gridster param
    $(".gridster > ul").gridster({
        widget_margins : [ 10, 10 ],
        widget_base_dimensions : [ 270, 150 ]
    });
    app.DataViews = {};
    app.DataViews.battery = new RawDataView("battery", {name: "Battery"});
    app.DataViews.altitude = new RawDataView("altitude", {name: "Altitude"});
    app.DataViews.phi = new RawDataView("phi", {name: "Phi"});
    app.DataViews.psi = new RawDataView("psi", {name: "Psi"});
    app.DataViews.theta = new RawDataView("theta", {name: "Theta"});
});