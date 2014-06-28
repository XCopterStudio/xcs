var WaitActionType = ENUM("Click");
var WaitActionState = ENUM("Started", "Stopped");

var WaitAction = Backbone.Model.extend({
    defaults : {
        actionType : WaitActionType.None,
        actionState : WaitActionState.None,
        action : "",
        selector: "",
    },

    initialize : function(selector, actionType, action) {
        if(action) {
            this.set("action", action);
        }
        
        if(actionType) {
            this.set("actionType", actionType);
        }
        
        if(selector) {
            this.set("selector", selector);
        }
    },
    
    isInit : function() {
        return this.get("action") && 
            this.get("selector") && 
            this.get("actionType") && 
            ((this.get("actionType") & WaitActionType.All) > 0); // check for any action
    },
    
    start : function() {
        this.set("actionState", WaitActionState.Started);
    },
    
    stop : function() {
        this.set("actionState", WaitActionState.Stopped);
    },
});