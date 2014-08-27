var StateView = Backbone.View.extend({
    id : 'state',

    el : 'body',

    initialize : function() {
        window.onbeforeunload = function() {
            return "X-copter plan execution will be stopped!"; //Are you sure you want to navigate away?
        }

        window.onunload = function() {
            // empty... possible actions for leaving GUI
        }

        window.onload = function() {
            // show all x-copter tooltips
            $("[xtitle]").xtooltip();
        }
    },
});
