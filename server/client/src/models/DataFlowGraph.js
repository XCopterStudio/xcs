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

    setData: function(data) {
        // TODO: parse data and fill properties on this

        //DEBUG
        //console.log("setData: " + JSON.stringify(data));
        
        // set default dataFlowGraph
        if(data.dataFlowGraph) {
            //TODO: merge current dataFlowGtraph with recieved data.dataFlowGraph
            this.set("dataFlowGraph", data.dataFlowGraph);
        }
        
        // set prototypes
        if(data.prototype) {
            //console.log("..." + data.prototype.length);
            for(var j = 0; j < data.prototype.length; ++j) {
                var p = data.prototype[j];
                //console.log("..." + JSON.stringify(p));
                
                if(p.name) {  
                    // get old prototype or create new
                    var prot;
                    var oldProt = this.get("xprototype").findWhere({"name": p.name});
                    if(oldProt) {
                        prot = oldProt;
                    }
                    else {
                        //create new prototype node
                        prot = new DataFlowGraphNode();
                        prot.set("name", p.name);
                        
                        // add prototype to collection
                        this.get("xprototype").add(prot);
                    }
                    
                    //read vars
                    if(p.var) {
                        for(var i = 0; i < p.var.length; ++i) {
                            var oldXVar = prot.get("xvar").findWhere({"name": p.var[i].name});
                            if(!oldXVar) {
                                prot.get("xvar").add(new DataFlowGraphNodeIO({
                                    name : p.var[i].name,
                                    synType : p.var[i].synType,
                                    semType : p.var[i].semType
                                }));
                            }
                        }
                    }
                    
                    // read inputports
                    if(p.inputPort) {
                        for(var i = 0; i < p.inputPort.length; ++i) {
                            var oldXIPort = prot.get("xinputPort").findWhere({"name": p.inputPort[i].name});
                            if(!oldXIPort) {
                                prot.get("xinputPort").add(new DataFlowGraphNodeIO({
                                    name : p.inputPort[i].name,
                                    synType : p.inputPort[i].synType,
                                    semType : p.inputPort[i].semType
                                }));
                            }
                        }
                    }
                }
            }
            
            //TODO: what about removing last prototype?
            // check deleted protypes
            var p2Del = [];
            //console.log("..." + this.get("xprototype").length);
            for(var j = 0; j < this.get("xprototype").length; ++j) {
                var p = this.get("xprototype").at(j);
                //console.log("... " + p.get("name"));
                var prot = $.grep(data.prototype, function(item){ return item.name == p.get("name"); });
                
                // prototype was deleted
                if(prot.length == 0) {
                    p2Del.push(p);
                }
            }
            
            // remove deleted prototypes
            for(var j = 0; j < p2Del.length; ++j) {
                this.get("xprototype").remove(p);
            }
        }
    },
    
    reset: function() {
        // remove clones
        while(this.get("xclone").length > 0) {
            this.get("xclone").pop();
        }
        
        // remove prototypes
        while(this.get("xprototype").length > 0) {
            this.get("xprototype").pop();
        }
    },
    
    requestLoad: function() {
        this.sendRequest("DFG_LOAD");
    },
    
    requestCreate: function(dfg) {
        this.sendRequest("DFG_CREATE", dfg);
    },
    
    requestStart: function() {
        this.sendRequest("DFG_START");
    },
    
    requestStop: function() {
        this.sendRequest("DFG_STOP");
    },
    
    requestReset: function() {
        this.sendRequest("DFG_RESET");
    },
    
    sendRequest: function(request, data) {
        if(!data) {
            data = "";
        }
        
        var requestData = {
            type: "onboard",
            data: { 
                request: {
                    id: request,
                    requestData: data,
                }
            }
        };
        
        gSocket.emit('resend', JSON.stringify(requestData));
    },
});
