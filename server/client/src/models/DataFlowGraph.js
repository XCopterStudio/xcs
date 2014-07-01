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

var ResponseType = ENUM("Done", "Error");

var DataFlowGraph = Backbone.Model.extend({    
    requestId_ : 1,
    
    maxRequestId_ : 1000,
    
    responses : {
        addResponse : function(requestId, response) {
            this[requestId] = response;
        },
        
        getAndRemoveResponse : function(requestId) {
            if(this[requestId]) {
                var response = this[requestId];
                delete this[requestId];
                return response;
            }
        },
    },
    
    defaults : {
        "dfgDef" : "",
        "savedDfg" : "",
        "dataFlowGraph" : "",
        "xprototype" : new Backbone.Collection([], { model : DataFlowGraphNode }),
        "xclone" : new Backbone.Collection([], { model : DataFlowGraphNode })
    },
    
    initialize : function() { 
    },

    setData: function(data) {
        //DEBUG
        //console.log("setData: " + JSON.stringify(data));

        if(data.response) {
            
            console.log("RESPONSE: " + JSON.stringify(data.response));
            
            if(data.response.id && data.response.respondType && data.response.respondData && data.response.requestId) {
                var id = data.response.id 
                var responseTypeText = data.response.respondType;
                var responseData = data.response.respondData;
                var requestId = data.response.requestId;
                
                var responseType;
                switch(responseTypeText) {
                    case "Done": responseType = ResponseType.Done; break;
                    case "Error": responseType = ResponseType.Error; break;
                    default: responseType = ResponseType.None; break;
                }
                
                // run response action
                var responseAction = this.responses.getAndRemoveResponse(requestId);
                if(responseAction) {
                    responseAction(id, responseType, responseData);
                }
            }
            else {
                //TODO: bad format data  - what to do?
            }
        }
        
        // set dfg definition
        if(data.dfgDef) {
            this.setDfgDef(data.dfgDef);
        }
        
        // set availabel dfg for load 
        if(data.savedDfg) {
            this.setSavedDfg(data.savedDfg);
        }
        
        // set default dataFlowGraph
        if(data.dataFlowGraph) {
            //TODO: merge current dataFlowGtraph with recieved data.dataFlowGraph
            this.set("dataFlowGraph", data.dataFlowGraph);
        }
        
        // set prototypes
        if(data.prototype) {
            this.setPrototype(data.prototype);
        }
    },
    
    setDfgDef: function(dfgDef) {
        this.set("dfgDef", dfgDef);
    },
    
    setSavedDfg: function(savedDfg) {
        this.set("savedDfg", savedDfg);
    },
    
    setClone: function(clone) {
        //TODO: set clones
    },
    
    setPrototype: function(prototype) {
        for(var j = 0; j < prototype.length; ++j) {
            var p = prototype[j];
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
            var prot = $.grep(prototype, function(item){ return item.name == p.get("name"); });
            
            // prototype was deleted
            if(prot.length == 0) {
                p2Del.push(p);
            }
        }
        
        // remove deleted prototypes
        for(var j = 0; j < p2Del.length; ++j) {
            this.get("xprototype").remove(p);
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
    
    requestLoad: function(response) {
        //TODO: rename DFG_LOAD
        this.sendRequest("SAVED_DFG", "", response);
        this.sendRequest("DFG_LOAD", "", response);
    },
    
    requestCreate: function(dfg, response) {
        this.sendRequest("DFG_CREATE", dfg, response);
    },
    
    requestStart: function(response) {
        this.sendRequest("DFG_START", "", response);
    },
    
    requestStop: function(response) {
        this.sendRequest("DFG_STOP", "", response);
    },
    
    requestReset: function(response) {
        this.sendRequest("DFG_RESET", "", response);
    },
    
    requestSaveDfg: function(dfg, filename, rewrite, response) {
        // default value 4 rewrite is false
        rewrite = typeof rewrite !== 'undefined' ? rewrite : false;
        
        var data = {
            DFG : dfg, //JSON.stringify(dfg),
            filename : filename,
            rewrite : rewrite
        };
        
        this.sendRequest("DFG_SAVE", data, response);
    },
    
    requestLoadDfg: function(dfgFilename, response) {
        //TODO: rename to DFG_LOAD
        this.sendRequest("LOAD_DFG", dfgFilename, response);
    },
    
    sendRequest: function(request, data, response) {
        if(!data) {
            data = "";
        }
        
        // set reaquest id and prepare new one
        var rId = this.requestId_;
        if(++this.requestId_ > this.maxRequestId_) {
            this.requestId_ = 1;
        }
        
        // register response action 4 current request id
        if(response) {
            this.responses.addResponse(rId, response);
        }
        
        //create request message
        var requestData = {
            type: "onboard",
            data: { 
                request: {
                    id: request,
                    requestData: data,
                    requestId: rId,
                }
            }
        };
        
        // try send request
        try {
            console.log("REQUEST: " + JSON.stringify(requestData));
            
            gSocket.emit('resend', JSON.stringify(requestData));
        }
        catch(ex) {
            // TODO: log exception
            
            // remove response action
            this.responses.getAndRemoveResponse(rId);
        }
        
        return rId;
    },
});
