/* global gSocket */
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
        "xprototype" : new Backbone.Collection([], { model : DataFlowGraphPrototypeNode }),
        "xprototypePrivate" : new Backbone.Collection([], { model : DataFlowGraphPrototypeNode }),
        "xprototypeSpecial" : new Backbone.Collection([], { model : DataFlowGraphPrototypeNode })
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
        if(data.response) { 
            var errorMsg;
            try {
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
                    errorMsg = "Incomming response has bad format: " + JSON.stringify(data.response) + "!";
                }
            }
            catch(ex) {
                errorMsg = "Error when process incomming response: " + ex + "!";
            }
            
            if(errorMsg) {
                console.log(errorMsg);
                app.Flash.flashError(errorMsg);
            }
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
    
    dfgExist: function(dfgName) {
        var dfgs = this.get("savedDfg");
        if(dfgs){
            for(var i = 0; i < dfgs.length; ++i) {
                if(dfgs[i] == dfgName) {
                    return true;
                }
            }
        }
        
        return false;
    },
    
    setPrototypePrivate: function(prototype) {
        this.setPrototype(prototype, "xprototypePrivate");
    },
        
    setPrototypeSpecial: function(prototype) {
        for(var j = 0; j < prototype.length; ++j) { 
            var p = prototype[j];

            if(p.name) { 
                switch(p.name) {
                    case "Gui":
                        if(p.registerXVar && p.registerXVar.length == 1) {
                            var registerXVar = p.registerXVar.pop();
                            
                            // set widgets
                            var ports = app.DataView.getViewNames();
                            for(var i = 0; i < ports.length; ++i) {
                                p.registerXVar.push({
                                    name : ports[i],
                                    realName : registerXVar,
                                });
                            }
                           
                            // default port
                            p.registerXVar.push({
                                name : "default",
                                realName : registerXVar
                            });
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
        
        prototype = _.sortBy(prototype, function(r) { return r.name; });
        
        for(var j = 0; j < prototype.length; ++j) {
            var p = prototype[j];
            
            if(p.name) {  
                // get old prototype or create new
                var prot;
                var oldProt = this.get(prototypeName).findWhere({"name": p.name});
                if(oldProt) {
                    prot = oldProt;
                }
                else {
                    //create new prototype node
                    prot = new DataFlowGraphPrototypeNode();
                    prot.set("name", p.name);
                    
                    // add prototype to collection
                    this.get(prototypeName).add(prot);
                }
                
                // create new vars
                if(p.var) {
                    for(var i = 0; i < p.var.length; ++i) {
                        var oldXVar = prot.get("xvar").findWhere({"name": p.var[i].name});
                        if(!oldXVar) {
                            prot.get("xvar").add(new DataFlowGraphPrototypeNodeIO({
                                name: p.var[i].name,
                                synType: p.var[i].synType,
                                semType: p.var[i].semType
                            }));
                        }
                    }
                }
                
                // create new inputports
                if(p.inputPort) {
                    for(var i = 0; i < p.inputPort.length; ++i) {
                        var oldXIPort = prot.get("xinputPort").findWhere({"name": p.inputPort[i].name});
                        if(!oldXIPort) {
                            prot.get("xinputPort").add(new DataFlowGraphPrototypeNodeIO({
                                name: p.inputPort[i].name,
                                synType: p.inputPort[i].synType,
                                semType: p.inputPort[i].semType
                            }));
                        }
                    }
                }
                
                // remove old vars
                var xvars = prot.get("xvar");
                var xvarsToRemove = [];
                for(var i = 0; i < xvars.length; ++i) {
                    var newXVar = _.findWhere(p.var, {"name": xvars.at(i).get("name")});
                    if(!newXVar) {
                        xvarsToRemove.push(xvars.at(i));
                    }
                }
                for(var i = 0; i < xvarsToRemove.length; ++i) {
                    prot.get("xvar").remove(xvarsToRemove[i]);
                }
                
                // remove old input ports
                var xinputPorts = prot.get("xinputPort");
                var xinputPortsToRemove = [];
                for(var i = 0; i < xinputPorts.length; ++i) {
                    var newXInputPort = _.findWhere(p.inputPort, {"name": xinputPorts.at(i).get("name")});
                    if(!newXInputPort) {
                        xinputPortsToRemove.push(xinputPorts.at(i));
                    }
                }
                for(var i = 0; i < xinputPortsToRemove.length; ++i) {
                    prot.get("xinputPort").remove(xinputPortsToRemove[i]);
                }
                
                // create new  registerXVars
                if(p.registerXVar) {
                    for(var i = 0; i < p.registerXVar.length; ++i) {
                        var oldRegister = prot.get("registerXVar").findWhere({"name": (p.registerXVar[i].name ? p.registerXVar[i].name : p.registerXVar[i])});
                        if(!oldRegister) {
                            if(p.registerXVar[i].name && p.registerXVar[i].realName) {
                                prot.get("registerXVar").add(new DataFlowGraphPrototypeNodeIO({
                                    name: p.registerXVar[i].name,
                                    synType: "",
                                    semType: "",
                                    realName: p.registerXVar[i].realName,
                                }));
                            }
                            else {
                                prot.get("registerXVar").add(new DataFlowGraphPrototypeNodeIO({
                                    name: p.registerXVar[i],
                                    synType: "",
                                    semType: ""
                                }));
                            }
                        }
                    }
                }
            }
        }
        
        // check deleted protypes
        var p2Del = [];
        for(var j = 0; j < this.get(prototypeName).length; ++j) {
            var p = this.get(prototypeName).at(j);
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
        this.sendRequest("SAVED_DFG", "", response);
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
    
    requestRemoveDfg: function(dfgFilename, response) {
        this.sendRequest("DFG_REMOVE", dfgFilename, response);
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
            gSocket.emit('resend', JSON.stringify(requestData));
        }
        catch(ex) {
            // log
            var errorMsg = "Error when try send request: " + ex + "!";
            app.Flash.flashError(errorMsg);
            console.log(errorMsg);
            
            // remove response action
            this.responses.getAndRemoveResponse(rId);
        }
        
        return rId;
    },
});
