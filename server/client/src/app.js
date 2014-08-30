/*global $ */
/*jshint unused:false */
var app = app || {};

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

    //create and initialize app
    app = new AppView();
    app.initViews();

});