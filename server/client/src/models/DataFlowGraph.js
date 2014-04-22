/* global gSocket */

var DataFlowGraphNodeIO = Backbone.Model.extend({
    defaults : {
        "name" : "",
        "synType" : "",
        "semType" : ""
    }
});

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
        
        // propagate add on xvar and xinputPort like change
        this.listenTo(this.get("xvar"), "add", this.onXvarAdd);
        this.listenTo(this.get("xinputPort"), "add", this.onXinputPortAdd);
    },
    
    onXvarAdd : function() {
        this.trigger("change", this);
        this.trigger("change:xvar", this);
    },
    
    onXinputPortAdd : function() {
        this.trigger("change", this);
        this.trigger("change:xinputPort", this);
    }
});

var DataFlowGraph = Backbone.Model.extend({    
    defaults : {
        "dataFlowGraph" : "",
        "xprototype" : new Backbone.Collection([], { model : DataFlowGraphNode }),
        "xclone" : new Backbone.Collection([], { model : DataFlowGraphNode })
    },
    
    initialize : function() { 
    },

    setData: function (data) {
        // TODO: parse data and fill properties on this

        //DEBUG
        console.log("setData...........: " + JSON.stringify(data));
        
        // set default dataFlowGraph
        if(data.dataFlowGraph) {
            //TODO: merge current dataFlowGtraph with recieved data.dataFlowGraph
            this.set("dataFlowGraph", data.dataFlowGraph);
        }
        
    }
});
