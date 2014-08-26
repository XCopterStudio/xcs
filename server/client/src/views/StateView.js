var StateView = Backbone.View.extend({
    id : 'state',

    el : 'body',

    initialize : function() {
        window.onbeforeunload = function() {
            return "X-copter plan execution will be stopped!"; //Are you sure you want to navigate away?
        }

        window.onunload = function() {
            //TODO: send message to onboard?
            console.log("DISCONNECTING FROM ONBOARD!");
        }

        window.onload = function() {
            // show all x-copter tooltips
            $("[xtitle]").xtooltip();
        }
    },
});
