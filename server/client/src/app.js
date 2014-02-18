/*global $ */
/*jshint unused:false */
var app = app || {};

gSocket = io.connect('http://localhost:3000');

var router = Backbone.Router.extend({
    routes : {
	"manual	": "manual"
    },
    
    manual : function () {
	var view = new ManualControl({model: null});
    }
});

$(function () {
    'use strict';
    
    app = new AppView();
});