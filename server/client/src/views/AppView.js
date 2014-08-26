/*global Backbone*/

var AppView = Backbone.View.extend({

    el : 'body',

    initialize : function() {
        this.$('.dropdown-toggle').dropdown();
        this.$('.dropdown-menu input, .dropdown-menu label, .dropdown-menu .dropdown-header').click(function(e) {
            e.stopPropagation();
        });
    },

});