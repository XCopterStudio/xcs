var StateView = Backbone.View.extend({
    id : 'state',

    el : 'body',

    initialize : function() {

        window.onunload = function() {
            // empty... possible actions for leaving GUI
        }

        window.onload = function() {
            // show all x-copter tooltips
            $("[xtitle]").xtooltip();
        }
    },
});
