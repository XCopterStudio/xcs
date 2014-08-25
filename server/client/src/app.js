/*global $ */
/*jshint unused:false */
var app = app || {};

gSocket = io.connect('http://' + location.host);

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
    
    //initialize views
    app.Wait = new WaitView();
    app.Flash = new FlashMessagesView();
    app.StateView = new StateView();
    app.ModalView = new ModalView();
    app.SettingView = new SettingView();
    
    app.FlyControlView = new FlyControlView();
    app.DataView = new DataView();
    app.DataFlowGraph = new DataFlowGraphView();
    app.ConsoleView = new ConsoleView();
    
    app.BottomToolbarView = new BottomToolBarView();
    app.BottomToolbarView.addView("dfg-trigger", "dfg", app.DataFlowGraph);
    app.BottomToolbarView.addView("consoleTrigger", "console");
});