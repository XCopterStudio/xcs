/* global gSocket */

var DataFlowGraphNodeIO = Backbone.Model.extend({
    defaults : {
        "name" : "",
        "synType" : "",
        "semType" : "",
        "realName" : ""
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
        
        // init property xinputPort
        if(this.has("registerXVar")) {
            this.set("registerXVar", this.get("registerXVar").clone());
        }
        else {
            this.set("registerXVar", new Backbone.Collection([], { model : DataFlowGraphNodeIO }));
        }
        
        // propagate add on xvar and xinputPort like change
        this.listenTo(this.get("xvar"), "add", this.onXvarAdd);
        this.listenTo(this.get("xinputPort"), "add", this.onXinputPortAdd);
        this.listenTo(this.get("registerXVar"), "add", this.onRegisterXVarAdd);
    },
    
    onXvarAdd : function() {
        this.trigger("change", this);
        this.trigger("change:xvar", this);
    },
    
    onXinputPortAdd : function() {
        this.trigger("change", this);
        this.trigger("change:xinputPort", this);
    },
    
    onRegisterXVarAdd : function() {
        this.trigger("change", this);
        this.trigger("change:registerXVar", this);
    },
});

var ResponseType = ENUM("Done", "Error");

var DataFlowGraph = Backbone.Model.extend({    
    /*
     * destination type => list of source types
     */
    syntacticCompatibility_: {
        "urbi::UImage": ['xcs::BitmapType']
    },
    /*
     * destination type => list of source types
     */
    semanticCompatibility_: {
        
    },
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
        "ddfg" : "",
        "xprototype" : new Backbone.Collection([], { model : DataFlowGraphNode }),
        "xprototypePrivate" : new Backbone.Collection([], { model : DataFlowGraphNode }),
        "xprototypeSpecial" : new Backbone.Collection([], { model : DataFlowGraphNode })
    },
    
    initialize : function() { 
    },

    isConnectable: function (srcSyn, srcSem, dstSyn, dstSem) {
        var sameSyn = (dstSyn === '*') || (srcSyn === dstSyn);
        var sameSem = (dstSem === '*') || (srcSem === dstSem);
        
        sameSyn = sameSyn || (this.syntacticCompatibility_[dstSyn] !== undefined && this.syntacticCompatibility_[dstSyn].indexOf(srcSyn) !== -1);
        sameSem = sameSem || (this.semanticCompatibility_[dstSem] !== undefined && this.semanticCompatibility_[dstSem].indexOf(srcSem) !== -1);
        
        return sameSyn && sameSem;
    },

    setData: function(data) {
        //DEBUG
        //console.log("setData: " + JSON.stringify(data));

        if(data.response) {         
            console.log("RESPONSE: " + JSON.stringify(data.response));
            
            if(data.response.id && data.response.respondType && (data.response.respondData || data.response.respondData == "") && data.response.requestId) {
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
        if(data.ddfg) {
            //TODO: merge current dataFlowGtraph with recieved data.ddfg
            this.set("ddfg", data.ddfg);
        }
        
        // set prototypes
        if(data.prototype) {
            this.setPrototype(data.prototype);
        }
    },
    
    setDdfg: function(ddfg) {
        this.set("ddfg", ddfg);
    },
    
    setDfgDef: function(dfgDef) {
        this.set("dfgDef", dfgDef);
    },
    
    setSavedDfg: function(savedDfg) {
        this.set("savedDfg", savedDfg);
    },
    
    setPrototypePrivate: function(prototype) {
        this.setPrototype(prototype, "xprototypePrivate");
    },
        
    setPrototypeSpecial: function(prototype) {
        for(var j = 0; j < prototype.length; ++j) { 
            var p = prototype[j];

            if(p.name) { 
                switch(p.name) {
                    case "GUI":
                        if(p.registerXVar && p.registerXVar.length == 1) {
                            var registerXVar = p.registerXVar.pop();
                            
                            // set widgets
                            var ports = app.DataView.getViewNames();
                            for(var i = 0; i < ports.length; ++i) {
                                p.registerXVar.push({
                                    name : ports[i],
                                    realName : registerXVar
                                });
                            }
                        }
                        break;
                }
            }
        }
        
        this.setPrototype(prototype, "xprototypeSpecial");
    },
    
    setPrototype: function(prototype, prototypeName) {
        // default value 4 prototypeName is "xprototype"
        prototypeName = typeof prototypeName !== 'undefined' ? prototypeName : "xprototype";
        
        for(var j = 0; j < prototype.length; ++j) {
            var p = prototype[j];
            //console.log("..." + JSON.stringify(p));
            
            if(p.name) {  
                // get old prototype or create new
                var prot;
                var oldProt = this.get(prototypeName).findWhere({"name": p.name});
                if(oldProt) {
                    prot = oldProt;
                }
                else {
                    //create new prototype node
                    prot = new DataFlowGraphNode();
                    prot.set("name", p.name);
                    
                    // add prototype to collection
                    this.get(prototypeName).add(prot);
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
                
                // read registerXVars
                if(p.registerXVar) {
                    for(var i = 0; i < p.registerXVar.length; ++i) {
                        var oldRegister = prot.get("registerXVar").findWhere({"name": p.registerXVar[i]});
                        if(!oldRegister) {
                            if(p.registerXVar[i].name && p.registerXVar[i].realName) {
                                prot.get("registerXVar").add(new DataFlowGraphNodeIO({
                                    name : p.registerXVar[i].name,
                                    synType : "",
                                    semType : "",
                                    realName : p.registerXVar[i].realName
                                }));
                            }
                            else {
                                prot.get("registerXVar").add(new DataFlowGraphNodeIO({
                                    name : p.registerXVar[i],
                                    synType : "",
                                    semType : ""
                                }));
                            }
                        }
                    }
                }
            }
        }
        
        // check deleted protypes
        var p2Del = [];
        //console.log("..." + this.get("xprototype").length);
        for(var j = 0; j < this.get(prototypeName).length; ++j) {
            var p = this.get(prototypeName).at(j);
            //console.log("... " + p.get("name"));
            var prot = $.grep(prototype, function(item){ return item.name == p.get("name"); });
            
            // prototype was deleted
            if(prot.length == 0) {
                p2Del.push(p);
            }
        }
        
        // remove deleted prototypes
        for(var j = 0; j < p2Del.length; ++j) {
            this.get(prototypeName).remove(p);
        }
    },
    
    reset: function() {
        // remove special prototypes
        while(this.get("xprototypeSpecial").length > 0) {
            this.get("xprototypeSpecial").pop();
        }
        
        // remove private prototypes
        while(this.get("xprototypePrivate").length > 0) {
            this.get("xprototypePrivate").pop();
        }
        
        // remove prototypes
        while(this.get("xprototype").length > 0) {
            this.get("xprototype").pop();
        }
    },
    
    requestLoad: function(response) {
        this.sendRequest("SAVED_DFG", "", response);
        this.sendRequest("DFG_LOAD_METADATA", "", response);
    },
    
    requestCreate: function(dfg, response) {
        this.sendRequest("DFG_CREATE", dfg, response);
    },
    
    requestStart: function(modelId, response) {
        this.sendRequest("DFG_START", modelId, response);
    },
    
    requestStop: function(modelId, response) {
        this.sendRequest("DFG_STOP", modelId, response);
    },
    
    requestReset: function(modelId, response) {
        this.sendRequest("DFG_RESET", modelId, response);
    },
    
    requestSaveDfg: function(dfg, filename, rewrite, response) {
        // default value 4 rewrite is false
        rewrite = typeof rewrite !== 'undefined' ? rewrite : false;
        
        var data = {
            DFG : dfg,
            filename : filename,
            rewrite : rewrite
        };
        
        this.sendRequest("DFG_SAVE", data, response);
    },
    
    requestLoadDfg: function(dfgFilename, response) {
        this.sendRequest("DFG_LOAD", dfgFilename, response);
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
