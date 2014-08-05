var DataFlowGraphNode = Backbone.Model.extend({
    initialize : function() {
        // init property name
        if(!this.has("name")) {
            this.set("name", "");
        }
        
        // init property xvar
        if(this.has("xvar")) {
            this.set("xvar", this.get("xvar").clone());
        }
        else {
            this.set("xvar", new Backbone.Collection([], { model : DataFlowGraphNodeIO }));
        }
        
        // init property xinputPort
        if(this.has("xinputPort")) {
            this.set("xinputPort", this.get("xinputPort").clone());
        }
        else {
            this.set("xinputPort", new Backbone.Collection([], { model : DataFlowGraphNodeIO }));
        }
        
        // init property xinputPort
        if(this.has("registerXVar")) {
            this.set("registerXVar", this.get("registerXVar").clone());
        }
        else {
            this.set("registerXVar", new Backbone.Collection([], { model : DataFlowGraphNodeIO }));
        }
        
        // propagate add/remove on xvar and xinputPort like change
        this.listenTo(this.get("xvar"), "add", this.onXvarChange);
        this.listenTo(this.get("xinputPort"), "add", this.onXinputPortChange);
        this.listenTo(this.get("registerXVar"), "add", this.onRegisterXVarChange);
        this.listenTo(this.get("xvar"), "remove", this.onXvarChange);
        this.listenTo(this.get("xinputPort"), "remove", this.onXinputPortChange);
        this.listenTo(this.get("registerXVar"), "remove", this.onRegisterXVarChange);
    },
    
    onXvarChange : function() {
        this.trigger("change", this);
        this.trigger("change:xvar", this);
    },
    
    onXinputPortChange : function() {
        this.trigger("change", this);
        this.trigger("change:xinputPort", this);
    },
    
    onRegisterXVarChange : function() {
        this.trigger("change", this);
        this.trigger("change:registerXVar", this);
    },
});