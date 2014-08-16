var StateView = Backbone.View.extend({
    id : 'state',
    
    el : 'body',
    
    initialize : function() {
        window.onbeforeunload = function() {
            //TODO: return context text by x-copter state?
            return "X-copter plan execution will be stopped!"; //Are you sure you want to navigate away?
        }
        
        window.onunload = function() {
            //TODO: send message to onboard?
            console.log("DISCONNECTING FROM ONBOARD!");
        }
    },
});