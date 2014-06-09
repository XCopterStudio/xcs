/*global Backbone*/

var AppView = Backbone.View.extend({

    el : 'body',

    initialize : function() {
        this.keyboard = new KeyboardControllerView();
        this.mobile = new MobileControllerView();
    },

});