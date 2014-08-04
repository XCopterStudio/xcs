var DfgState = ENUM(
    "DFG_STATE_NODES_LOADED", 
    "DFG_STATE_NOTCREATED", "DFG_STATE_CREATED", "DFG_STATE_STARTED", "DFG_STATE_STOPPED" //,    // DFG STATE
    //"DFG_STATE_USER_DFG_LOADED", "DFG_STATE_DEFAULT_DFG_LOADED", "DFG_STATE_NODE_DFG_LOADED"    // LOADED DFG TYPE
    );

var NODE_TYPE = ENUM(
    "PUBLIC",
    "PRIVATE",
    "SPECIAL"
);

var DataFlowGraphView = Backbone.View.extend({
    id : 'data-flow-graph',
    
    el : '#dfg',
        
    dfgGraph : {},
    
    dfgToolboxNodes : {},
    
    dfgModels : {
        addModel: function(id, model) {
            this[id] = model;
        },
        
        removeModel: function(id) {
            if(this[id]) {
                this[id].remove();
                delete this[id];
            }
        },
        
        clear: function() {
            var ps = [];
            for(var p in this) {
                if (this.hasOwnProperty(p) && !_.isFunction(this[p])) {
                    ps.push(p);
                }
            }
            
            for(var i = 0; i < ps.length; ++i) {
                this.removeModel(ps[i]);
            }
        },
        
        count: function() {
            var c = 0;
            for(var p in this) {
                if (this.hasOwnProperty(p) && !_.isFunction(this[p])) {
                    ++c;
                }
            }
            
            return c;
        },
    },
    
    dfgCounter : [],
    
    dfgState_ : DfgState.DFG_STATE_NOTCREATED,
    
    initialize : function() {
        this.model = new DataFlowGraph();
        
        this.onInputChange(app.Onboard);      
        this.listenTo(app.Onboard, "change:onboard", this.onInputChange);
        
        this.listenTo(this.model, "change:ddfg", this.onDdfgChange);
        this.listenTo(this.model.get("xprototype"), "add", this.onPrototypeAdd);
        this.listenTo(this.model.get("xprototypePrivate"), "add", this.onPrototypePrivateAdd);
        this.listenTo(this.model.get("xprototypeSpecial"), "add", this.onPrototypeSpecialAdd);
        this.listenTo(this.model.get("xprototype"), "remove", this.onPrototypeRemove);
        this.listenTo(this.model.get("xprototypePrivate"), "remove", this.onPrototypePrivateRemove);
        this.listenTo(this.model.get("xprototypeSpecial"), "remove", this.onPrototypeSpecialRemove);

        this.listenTo(this.model, "change:savedDfg", this.onSavedDfgChange);
        this.listenTo(this.model, "change:dfgDef", this.onDfgDefChange);
        
        this.initializeDfg();
        
        //this.initializeBlockly();
    },
    
    
    /****************************
    ** DATA FLOW GRAPH SECTION **
    ****************************/
    initializeDfg : function() {
        var self = this;
        
        var loadAction = new WaitAction("#dfgLoad", WaitActionType.Click)
        loadAction.set("action", function() { loadAction.start(); self.dfgLoad(function() { loadAction.stop() }); });
        app.Wait.setWaitAction(loadAction);
        
        var createAction = new WaitAction("#dfgCreate", WaitActionType.Click)
        createAction.set("action", function() { createAction.start(); self.dfgCreate(function() { createAction.stop() }); });
        app.Wait.setWaitAction(createAction);
        
        var startAction = new WaitAction("#dfgStart", WaitActionType.Click)
        startAction.set("action", function() { startAction.start(); self.dfgStart(function() { startAction.stop() }); });
        app.Wait.setWaitAction(startAction);
        
        var stopAction = new WaitAction("#dfgStop", WaitActionType.Click)
        stopAction.set("action", function() { stopAction.start(); self.dfgStop(function() { stopAction.stop() }); });
        app.Wait.setWaitAction(stopAction);
        
        var destroyAction = new WaitAction("#dfgDestroy", WaitActionType.Click)
        destroyAction.set("action", function() { destroyAction.start(); self.dfgDestroy(function() { destroyAction.stop() }); });
        app.Wait.setWaitAction(destroyAction);
        
        var resetAction = new WaitAction("#dfgReset", WaitActionType.Click)
        resetAction.set("action", function() { resetAction.start(); self.dfgReset(true, function() { resetAction.stop() }); });
        app.Wait.setWaitAction(resetAction);
        
        var saveDfgAction = new WaitAction("#dfgSaveDfg", WaitActionType.Click)
        saveDfgAction.set("action", function() { saveDfgAction.start(); self.dfgSaveDfg(function() { saveDfgAction.stop() }); });
        app.Wait.setWaitAction(saveDfgAction);
        
        this.dfgGraph = new joint.dia.Graph;
        this.listenTo(this.dfgGraph, 'change:target', this.setLink);
        this.listenTo(this.dfgGraph, 'change:source', this.setLink);

        this.listenTo(this.dfgGraph, 'remove', this.removeNode);
        
        //debug
//        var flowGraphConsole = $('#flow-graph-console');
//        flowGraphConsole.append('<textarea id="flow-graph-txt" rows="15" cols="150"></textarea>');
//        
//        this.dfgGraph.on('change', function(model) {
//            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
//        });
//        this.dfgGraph.on('add', function(cell) {
//            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
//        });
//        this.dfgGraph.on('remove', function(cell) {
//            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
//        });

        this.initializeDfgToolbox4Drop();
           
        var paper = new joint.dia.Paper({
            el: $('#flow-graph-screen'),
            width: 1024, 
            height: 500, 
            gridSize: 10,   // size of grid in px (how many px reprezents one cell)
            model: this.dfgGraph,
            validateConnection: function(cellViewS, magnetS, cellViewT, magnetT, end, linkView) {
                if(magnetS && magnetT && magnetS.getAttribute("type") == "out" && magnetT.getAttribute("type") == "in") {
                    var idS = magnetS.getAttribute("port");
                    var idT = magnetT.getAttribute("port");

                    var semTT = cellViewT.model.inPortsType[idT].semType;
                    var synTT = cellViewT.model.inPortsType[idT].synType;
                    
                    return self.model.isConnectable(cellViewS.model.outPortsType[idS].synType, cellViewS.model.outPortsType[idS].semType, synTT, semTT);
                }
                
                return false;
            },
            validateMagnet: function(cellView, magnet) {
                return magnet.getAttribute('type') != "in";
            },
            snapLinks: { radius: 45 },
        });
    },
    
    removeNode : function(model) {
        // create widgets
        var prototypeId = model.get("origId");
        var cloneId = model.get("id");
        
        if(prototypeId && cloneId) {
            switch(prototypeId) {
                case "GUI":
                    // for each registerXVar call remove on DataView
                    var viewIds = model.viewIds;
                    for(var k = 0; k < viewIds.length; ++k) {
                        var viewId = viewIds[k];
                        app.DataView.removeView(viewId);
                    }
                    break;
            }
        }
    },
    
    trimId : function(id) {
        return id.replace(/ /g,'');
    },
    
    addNode2DfgToolbox : function(id, title, toolboxItemId) {
        if(!title) {
            title = id;
        }
        
        var toolbox = $('#' + toolboxItemId);
        toolbox.append('\
            <div class="panel-body drag_init" id="' + id + '">  \
                ' + title + '                                   \
            </div>                                              \
        ');
        
        //set number of nodes
        var counter = $('#' + toolboxItemId + '-count');
        counter.html(parseInt(counter.html()) + 1);
        
        this.initializeDfgToolbox4Drag();
    },
    
    removeNodeFromDfgToolbox : function(id, nodeType) {
        var node = $('#' + id);
        node.remove();
        
        //set number of nodes
        var counter = "";
        switch(nodeType) {
            case NODE_TYPE.PUBLIC:
                counter = $('#xnodes-list-count');
                break;
            case NODE_TYPE.PRIVATE:
                counter = $('#xnodes-private-list-count');
                break;
            case NODE_TYPE.SPECIAL:
                counter = $('#xnodes-special-list-count');
                break;
        }
        if(counter != "") {
            counter.html(parseInt(counter.html()) - 1);
        }
    },
    
    initializeDfgToolbox4Drag : function() {
        //Make element draggable
        var dragInit = $(".drag_init");
        dragInit.draggable({
            helper: 'clone',
            containment: '#DFG',
            //stop: function (ev, ui) { }
        });
        
        // remove drag_init class and add just drug_
        dragInit.removeClass("drag_init").addClass("drag_");
    },
    
    initializeDfgToolbox4Drop : function() {
        var self = this;
        
        //Make element droppable
        $("#flow-graph-screen").droppable({
            drop: function (ev, ui) {
                var toolId = ui.draggable.attr('id');                       
                var pos = $(ui.helper).offset();
                var containerPos = $("#flow-graph-screen").offset();           
                self.addNodeToGraph(toolId, pos.left - containerPos.left, pos.top - containerPos.top);
            }
        });
    },
    
    addNodeToGraph : function(nodeId, x, y) {
        if(!this.dfgCounter[nodeId]) {
            this.dfgCounter[nodeId] = 1;
        }
        else {
            ++this.dfgCounter[nodeId];
        }
        
        // get model
        var modelPrototype = this.dfgToolboxNodes[nodeId];
        
        // get xvars and xinput ports and register xvar methods
        var xvars = modelPrototype.get("xvar");
        var xinputPorts = modelPrototype.get("xinputPort");
        var registerXVars = modelPrototype.get("registerXVar");
        
        //get name
        var cloneName;            
        var modelId;
        if(this.dfgCounter[nodeId] == 1) {
            cloneName = modelPrototype.get("name");
            modelId = nodeId;   
        }
        else {
            cloneName = modelPrototype.get("name") + " " + this.dfgCounter[nodeId];
            modelId = nodeId + this.dfgCounter[nodeId];
        }
        cloneName = cloneName.charAt(0).toLowerCase() + cloneName.slice(1);
        modelId = modelId.charAt(0).toLowerCase() + modelId.slice(1);
        
        var m = new (joint.shapes.dfg.DataFlowGraphDefaultModel)({
            position: { x: x, y: y },
        });
        m.setId(modelId);
        m.setOrigId(nodeId);
        m.setLabel(cloneName);
        
        // get all xvars
        if(xvars) {
            xvars.forEach(function(xvar) {
                var xvarName = xvar.get("name");
                var xvarSynType = xvar.get("synType");
                var xvarSemType = xvar.get("semType");   
                m.addOutpuPort(xvarName, xvarSemType, xvarSynType);
            });
        }
        
        // get all xinputports
        if(xinputPorts) {
            xinputPorts.forEach(function(xvar) {
                var xinputPortName = xvar.get("name");
                var xinputPortSynType = xvar.get("synType");
                var xinputPortSemType = xvar.get("semType");
                m.addInputPort(xinputPortName, xinputPortSemType, xinputPortSynType);
            });
        }
        
        // get all register xvar methods
        if(registerXVars) {
            registerXVars.forEach(function(reg) {
                var registerName = reg.get("name");
//                var registerSynType = "*";
//                var registerSemType = "*";
//                m.addInputPort(registerName, registerSemType, registerSynType);
                m.addRegisterXVar(registerName);
            });
        }
        
        m.setAutoSize();
        
        m.setState(NodeState.NOTCREATED);
        
        this.dfgGraph.addCell(m);
        this.dfgModels.addModel(modelId, m);
        
        //set context menu
        var self = this;
        $("#flow-graph-screen .DataFlowGraphDefaultModel").contextMenu({
            parentSelector:     "#dfg",
            menuSelector:       "#dfg-screen-context-menu",
            menuSelected:       function (target, selectedMenuItem) {
                var action = selectedMenuItem.attr("action");
                var modelId = target.attr("model-id");
                var response;
                switch(action) {
                    case "CREATE":
                        self.dfgCreate(response, modelId)
                        break;
                    case "START":
                        self.dfgStart(response, modelId);
                        break;
                    case "STOP":
                        self.dfgStop(response, modelId);
                        break;
                    case "DESTROY":
                        self.dfgDestroy(response, modelId);
                        break;
                    case "DELETE":
                        self.dfgDestroy(
                            function() { 
                                self.dfgModels.removeModel(modelId);  
                                app.Flash.flashSuccess("Node " + modelId + " is deleted.");
                            }, 
                            modelId, false, false);
                        break;
                }
            }
        });
        
        return modelId;
    },
    
    addLinkToGraph : function(sourceId, sourcePort, sourceSelector, targetId, targetPort, targetSelector, vertices) {
        // default value 4 vertices
        vertices = typeof vertices !== 'undefined' ? vertices : [];
        
        var link = new joint.dia.Link({
            source: { id: sourceId, port: sourcePort, selector: sourceSelector },
            target: { id: targetId, port: targetPort, selector: targetSelector },
            vertices: vertices,
        });
        this.dfgGraph.addCell(link);
        this.setLink(link);
    },
    
    setLink : function(link) {   
        //determine if link is pin to some port on both sides
        if(link.attributes.target.x || link.attributes.source.x){
            this.setBadLink(link);
        }
        else {
            this.setWellLink(link);
        }
    },
    
    setBadLink : function(link) {
        link.attr({
            '.connection': { stroke: 'red' },
            //'.marker-source': { fill: 'red', d: 'M 10 0 L 0 5 L 10 10 z' },
            '.marker-target': { fill: 'red', d: 'M 10 0 L 0 5 L 10 10 z' }
        });
    },
    
    setWellLink : function(link) {
        link.attr({
            '.connection': { stroke: 'green' },
            //'.marker-source': { fill: 'red', d: 'M 10 0 L 0 5 L 10 10 z' },
            '.marker-target': { fill: 'green', d: 'M 10 0 L 0 5 L 10 10 z' }
        });
    },
    
    onSavedDfgChange : function(model) {
        var dfgs = model.get("savedDfg");
        
        var loadItems = $('#DFG-saved-items');
        
        //NOTE: there should be no need to unregister old click events - jquery should handle it yourself
        loadItems.html('');
        
        var self = this;
        var loadDfgAction = {};
        for(var i = 0; i < dfgs.length; ++i) {
            var id = "dfgLoadDfg_" + this.trimId(dfgs[i]);
            loadItems.append('<li><a class="btn dfgLoadDfg" id="' + id + '" role="button" filename="' + dfgs[i] + '">' + dfgs[i] + '</a></li>');
            
            loadDfgAction[id] = new WaitAction("#" + id, WaitActionType.Click);
            loadDfgAction[id].set("action", function(event) { 
                var hrefId = $(event.currentTarget).attr("id");
                loadDfgAction[hrefId].start(); 
                self.dfgLoadDfg(event, function() { 
                    loadDfgAction[hrefId].stop(); 
                }); 
            });
            app.Wait.setWaitAction(loadDfgAction[id]);
        }
    },
    
    onDfgDefChange : function(model) {
        // TODO: show some question? 
        
        var graph = model.get("dfgDef");
        if(graph && graph.DFG) {
            this.loadGraph(graph.DFG);
        }
    },
     
    onDdfgChange : function(model) {
        // TODO: show some question? 
        var graph = model.get("ddfg");       
        this.loadGraph(graph);
    },
    
    loadGraph : function(dfg, append) {        
        // default value 4 append is false
        append = typeof append !== 'undefined' ? append : false;
        
        if(!dfg) {
            return;
        }
        
        try {
            if(!append) {
                this.dfgReset(false);
            }
                                    
            var dfgJson = JSON.parse(dfg);
            if(dfgJson.cells) {
                var nodes = {};
                
                // nodes
                for(var i = 0; i < dfgJson.cells.length; ++i) {
                    var cell = dfgJson.cells[i];
                    
                    // node
                    if(cell.origId) {
                        if(this.dfgToolboxNodes[cell.origId]) {
                            nodes[cell.id] = this.addNodeToGraph(cell.origId, cell.position.x, cell.position.y);
                        }
                        else {
                            //TODO: what to do?
                        }
                    }
                }
                
                // links
                for(var i = 0; i < dfgJson.cells.length; ++i) {
                    var cell = dfgJson.cells[i];
                    
                    //cell
                    if(cell.source) {
                        var vertices = ((cell.vertices) ? cell.vertices : []);
                        var sourceSelector = ((cell.source.selector) ? cell.source.selector : "");
                        var targetSelector = ((cell.target.selector) ? cell.target.selector : "");
                        this.addLinkToGraph(nodes[cell.source.id], cell.source.port, sourceSelector, 
                                            nodes[cell.target.id], cell.target.port, targetSelector,
                                            vertices);
                    }
                }
            }
        }
        catch(ex) {
            console.error("...ERROR: " + ex.message);
            this.dfgReset(false);
        }
    },
    
    onPrototypeAdd : function(modelPrototype, nodeType) {
        // default value nodeType is public
        nodeType = typeof NODE_TYPE.getName(nodeType) !== 'undefined' ? nodeType : NODE_TYPE.PUBLIC;

        var responseMethod;
        var nodeListName;
        switch(nodeType) {
            case NODE_TYPE.PUBLIC:
                responseMethod = this.onPrototypeChange;
                nodeListName = "xnodes-list";
                break;
            case NODE_TYPE.PRIVATE:
                responseMethod = this.onPrototypePrivateChange;
                nodeListName = "xnodes-private-list";
                break;
            case NODE_TYPE.SPECIAL:
                responseMethod = this.onPrototypeSpecialChange;
                nodeListName = "xnodes-special-list";
                break;
            default:
                return;
        }
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        var prototypeId = this.trimId(prototypeName);
        
        if(this.dfgToolboxNodes[prototypeId]) {
            console.error("ERROR(onPrototypeAdd): " + prototypeName + " was already loaded!");
            return;
        }
        
        // add 2 toolbox - show to user
        this.addNode2DfgToolbox(prototypeId, prototypeName, nodeListName);
        this.dfgToolboxNodes[prototypeId] = modelPrototype;
        
        //watch 4 changes
        this.listenTo(modelPrototype, "change", responseMethod);
    },
    
    onPrototypePrivateAdd : function(modelPrototype) {
        this.onPrototypeAdd(modelPrototype, NODE_TYPE.PRIVATE);
    },
    
    onPrototypeSpecialAdd : function(modelPrototype) {
        this.onPrototypeAdd(modelPrototype, NODE_TYPE.SPECIAL);
    },
    
    onPrototypeRemove : function(modelPrototype, nodeType) {
        // default value nodeType is public
        nodeType = typeof NODE_TYPE.getName(nodeType) !== 'undefined' ? nodeType : NODE_TYPE.PUBLIC;
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        var prototypeId = this.trimId(prototypeName);
        
        //DEBUG
        console.log("onPrototypeRemove " + prototypeName);
        
        //validation
        if(!this.dfgToolboxNodes[prototypeId]) {
            console.error("ERROR(onPrototypeRemove): " + prototypeName + " was not yet loaded!");
            return;
        }
        
        // stop listening to old model
        this.stopListening(this.dfgToolboxNodes[prototypeId]);
        
        //delete from GUI
        this.removeNodeFromDfgToolbox(prototypeId, nodeType);
        
        // delete old model
        delete this.dfgToolboxNodes[prototypeId];
    },
    
    onPrototypePrivateRemove : function(modelPrototype) {
        this.onPrototypeRemove(modelPrototype, NODE_TYPE.PRIVATE);
    },
    
    onPrototypeSpecialRemove : function(modelPrototype) {
        this.onPrototypeRemove(modelPrototype, NODE_TYPE.SPECIAL);
    },
    
    onPrototypeChange : function(modelPrototype, nodeType) {
        // default value nodeType is public
        nodeType = typeof NODE_TYPE.getName(nodeType) !== 'undefined' ? nodeType : NODE_TYPE.PUBLIC;
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        var prototypeId = this.trimId(prototypeName);
        
        //DEBUG
        console.log("onPrototypeChange: " + prototypeName);
        
        // add 2 toolbox - show to user
        if(!this.dfgToolboxNodes[prototypeId]) {
            console.error("ERROR(onPrototypeChange): " + prototypeName + " was not yet loaded!");
            return;
        }
        
        // stop listening to old model
        this.stopListening(this.dfgToolboxNodes[prototypeId]);
        
        // remove old model and add new one
        this.dfgToolboxNodes[prototypeId] = modelPrototype;
        
        //watch 4 changes
        switch(nodeType) {
            case NODE_TYPE.PUBLIC:
                this.listenTo(modelPrototype, "change", this.onPrototypeChange);
                break;
            case NODE_TYPE.PRIVATE:
                this.listenTo(modelPrototype, "change", this.onPrototypePrivateChange);
                break;
            case NODE_TYPE.SPECIAL:
                this.listenTo(modelPrototype, "change", this.onPrototypeSpecialChange);
                break;
            default:
                return;
        }
    },
    
    onPrototypePrivateChange : function(modelPrototype) {
        this.onPrototypeChange(modelPrototype, true);
    },
    
    onPrototypeSpecialChange : function(modelPrototype) {
        this.onPrototypeChange(modelPrototype, true);
    },
    
    onInputChange : function(xcState) {
        var data = xcState.get("onboard");
        this.model.setData(data);
    },

    dfgLoad : function(response) {
        try {
            var self = this;
            
            //self.model.reset();
            self.model.requestLoad(function(id, responseType, responseData) {
                //show error
                if(responseType == ResponseType.Error) {
                    app.Flash.flashError("Error when try to load nodes: " + responseData + ".");
                }
                
                if(responseType == ResponseType.Done) {
                    if(responseData.prototype) {
                        self.model.setPrototype(responseData.prototype);
                        
                        // show result
                        var successCount = self.model.get("xprototype").length;
                        app.Flash.flashSuccess("" + successCount + " " + (successCount == 1 ? "node is" : "nodes are") + " loaded.");
                    }
                    if(responseData.prototypePrivate) {
                        self.model.setPrototypePrivate(responseData.prototypePrivate);
                    }
                    if(responseData.prototypeSpecial) {
                        self.model.setPrototypeSpecial(responseData.prototypeSpecial);
                    }
                    if(responseData.savedDfg) {
                        self.model.setSavedDfg(responseData.savedDfg);
                    }
                    if(responseData.ddfg) {
                        if(self.dfgModels.count() == 0) {
                            self.model.setDdfg("");
                            self.model.setDdfg(responseData.ddfg);
                        }
                        //debug
                        else {
                            console.log("!!! DDFG will not be loaded - DFG contains " + self.dfgModels.count() + " nodes!");
                        }
                    }
                }
                
                // response
                if(response) {
                    response();
                }
                
                // set state - must be at the end
                if(responseType == ResponseType.Done) {
                    self.setDfgState(DfgState.DFG_STATE_NODES_LOADED);
                }
            });
        }
        catch(ex) {
            //response action
            if(response) {
                response();
            }
            
            // show error
            app.Flash.flashError("Error when try to load nodes: " + ex + "!");
        }
    },
    
    dfgCreate : function(response, modelId) {
        try {
            // load dfg 2 json object
            var jsonDfg = this.dfgGraph.toJSON()
            
            if(jsonDfg.cells) {
                // prepare object 4 dfg info
                var dfg = {
                    prototype: [],
                    link: [],
                    registerXVar: []
                };
                
                var viewNames = [];
                    
                for(var i = 0; i < jsonDfg.cells.length; ++i) {
                    var cell = jsonDfg.cells[i];
                    
                    // nodes
                    if(cell.inPorts && cell.outPorts && cell.id && cell.origId) {
                        // load prototypes info
                        var prototypeId = cell.origId;
                        var cloneId = cell.id;
                        
                        var modelPrototype = this.dfgToolboxNodes[prototypeId];
                        if(!modelPrototype) {                            
                            throw prototypeId + " is not loaded";
                            return;
                        }
                        
                        if(modelId && modelId != cloneId) {
                            continue;
                        }
                        
                        // prepate loaded prototypes info 4 sending
                        dfg.prototype.push({
                            id: cloneId,
                            name: modelPrototype.get("name"),
                        });
                    }
                    
                    // links
                    else if(cell.source && cell.target && cell.source.id && cell.target.id && cell.type && cell.type == "link") {
                        // load clone info
                        var cloneId = cell.target.id;
                        var cloneModel = this.dfgModels[cloneId];
                        if(!cloneModel) {
                            throw cloneId + " is not loaded";
                            return;
                        }
                        
                        // load prototypes info
                        var prototypeId = cloneModel.get("origId");
                        var modelPrototype = this.dfgToolboxNodes[prototypeId];
                        if(!modelPrototype) {
                            throw prototypeId + " is not loaded";
                            return;
                        }
                        
                        //TODO: rename port to xvar/xinputPort/registerXVar
                        var model = modelPrototype.get("registerXVar").findWhere({"name": cell.target.port});
                        if(model) {
                            var targetPort = cell.target.port;
                            if(model.get("realName") != "") {
                                targetPort = model.get("realName");
                            }
                            
                            dfg.registerXVar.push({
                                source: {
                                    id: cell.source.id,
                                    port: cell.source.port,
                                },
                                target: {
                                    id: cell.target.id,
                                    port: targetPort,
                                },
                            });
                            
                            if(cloneModel.get("origId") == "GUI") {
                                viewNames.push({
                                    viewName: cell.target.port,
                                    dataId: cell.source.id + "_" + cell.source.port,
                                    model: cloneModel
                                });
                            }
                        }
                        else {
                            // prepare links info 4 sending
                            dfg.link.push({
                                source: {
                                    id: cell.source.id,
                                    port: cell.source.port,
                                },
                                target: {
                                    id: cell.target.id,
                                    port: cell.target.port,
                                },
                            });
                        }
                    }
                }
                
                // send request
                var self = this;
                self.model.requestCreate(dfg, function(id, responseType, responseData) {
                    //show error
                    if(responseType == ResponseType.Error) {
                        app.Flash.flashError("Error when try to create nodes: " + responseData + "!");
                    }
                    
                    // set nodes states
                    else if(responseData) {
                        var successCount = 0;
                        
                        for(var i = 0; i < responseData.length; ++i) {
                            var model = self.dfgModels[responseData[i]];
                            if(model) {
                                model.setState(NodeState.CREATED);
                                ++successCount;
                                
                                // create widgets
                                if(model.get("origId") == "GUI") {
                                    for(var j = 0; j < viewNames.length; ++j) {
                                        viewNames[j].model.viewIds.push(app.DataView.addViewByName(viewNames[j].viewName, viewNames[j].dataId));
                                    }
                                }
                            }
                        }
                        
                        // show result
                        app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " created.");
                    }
                    
                    //response action
                    if(response) {
                        response();
                    }
                    
                    // set state - must be at the end
                    if(responseType == ResponseType.Done && !modelId) {
                        self.setDfgState(DfgState.DFG_STATE_CREATED);
                    }
                });
            }
        }
        catch(ex) {
            // show error
            app.Flash.flashError("Error when try to create nodes: " + ex + "!");
            
            //response action
            if(response) {
                response();
            }
        }
    },
    
    dfgStart : function(response, modelId) {
        var self = this;
        self.model.requestStart(modelId, function(id, responseType, responseData) {
            //show error
            if(responseType == ResponseType.Error) {
                app.Flash.flashError("Error when try to start nodes: " + responseData + ".");
            }
            
            // set nodes states
            else if(responseData) {
                var successCount = 0;
                
                for(var i = 0; i < responseData.length; ++i) {
                    var model = self.dfgModels[responseData[i]];
                    if(model) {
                        model.setState(NodeState.STARTED);
                        ++successCount;
                    }
                }
                
                // show result
                app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " started.");
            }
            
            //response action
            if(response) {
                response();
            }
            
            // set state - must be at the end
            if(responseType == ResponseType.Done && !modelId) {
                self.setDfgState(DfgState.DFG_STATE_STARTED);
            }
        });
    },
    
    dfgStop : function(response, modelId) {
        var self = this;
        self.model.requestStop(modelId, function(id, responseType, responseData) {
            //show error
            if(responseType == ResponseType.Error) {
                app.Flash.flashError("Error when try to stop nodes: " + responseData + ".");
            }
            
            // set nodes states
            else if(responseData) {
                var successCount = 0;
                
                for(var i = 0; i < responseData.length; ++i) {
                    var model = self.dfgModels[responseData[i]];
                    if(model) {
                        model.setState(NodeState.STOPPED);
                        ++successCount;
                    }
                }
                
                // show result
                app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " stopped.");
            }
            
            if(response) {
                response();
            }
            
            // set state - must be at the end
            if(responseType == ResponseType.Done && !modelId) {
                self.setDfgState(DfgState.DFG_STATE_STOPPED);
            }
        });
    },
    
    dfgDestroy: function(response, modelId, reset, checkDestroyed) {
        try {
            // default value
            reset = typeof reset !== 'undefined' ? reset : false;
            checkDestroyed = typeof checkDestroyed !== 'undefined' ? checkDestroyed : true;
        
            var self = this;
            
            if(reset) {
                self.model.reset();
            }
            
            self.model.requestReset(modelId, function(id, responseType, responseData) {
                //show error
                if(responseType == ResponseType.Error) {
                    app.Flash.flashError("Error when try to destroy nodes: " + responseData + ".");
                }
                
                else if(responseType == ResponseType.Done) {
                    if(reset) {
                        if(responseData.prototype) {
                            self.model.setPrototype(responseData.prototype);
                            
                            // show result
                            var successCount = self.model.get("xprototype").length;
                            app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " loaded.");
                        }
                        if(responseData.prototypePrivate) {
                            self.model.setPrototypePrivate(responseData.prototypePrivate);
                        }
                        if(responseData.prototypeSpecial) {
                            self.model.setPrototypeSpecial(responseData.prototypeSpecial);
                        }
                        if(responseData.savedDfg) {
                            self.model.setSavedDfg(responseData.savedDfg);
                        }
                        if(responseData.ddfg) {
                            self.model.setDdfg("");
                            self.model.setDdfg(responseData.ddfg);
                        }
                    }
                    
                    // set nodes states
                    if(responseData.destroyed) {
                        var successCount = 0;
                        
                        for(var i = 0; i < responseData.destroyed.length; ++i) {
                            var model = self.dfgModels[responseData.destroyed[i]];
                            if(model) {
                                model.setState(NodeState.NOTCREATED);
                                ++successCount;
                                
                                // create widgets
                                self.removeNode(model);
                            }
                        }
                        
                        // show result
                        if(successCount > 0) {
                            app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " destroyed.");
                        }
                        else if(checkDestroyed) {
                            app.Flash.flashError("Error when try to destroy nodes: nothing to destroy.");
                        }
                    }
                }
                
                if(response) {
                    response();
                }
                
                // set state - must be at the end
                if(responseType == ResponseType.Done && !modelId) {
                    self.setDfgState(DfgState.DFG_STATE_NOTCREATED);
                }
            });
        }
        catch(ex) {
            //response action
            if(response) {
                response();
            }
            
            // show error
            app.Flash.flashError("Error when try to destroy nodes: " + ex + "!");
        }
    },
    
    dfgReset : function(all, response) {
        // default value 4 all is true
        all = typeof all !== 'undefined' ? all : true;
        
        this.dfgModels.clear();
        this.dfgGraph.clear();
        
        if(all) {
            var modelId;
            this.dfgDestroy(response, modelId, true, false);
            app.Flash.flashSuccess("Data flow graph is resetted.");
        }
        else if(response) {
            app.Flash.flashSuccess("Data flow graph is resetted.");
            response();
        }
    }, 
    
    dfgSaveDfg : function(response) {
        var inputFilename = $('#dfgSaveDfg-filename');
        
        // read filenam
        var filename = inputFilename.val().trim();

        // validate filename
        var errorMsg = '';
        if(filename == '') {
            errorMsg += 'You must set the filename first! ';    
        }
        
        //TODO: properly show error message
        //show error message
        if(errorMsg != '') {
            console.log(errorMsg);
            
            if(response) {
                response();
            }
            
            return;
        }        

        // clean input 4 filename
        inputFilename.val('');
        
        // load dfg 2 json object
        var jsonDfg = this.dfgGraph.toJSON();
        
        //send request
        var self = this;
        self.model.requestSaveDfg(JSON.stringify(jsonDfg), filename, true, function(id, responseType, responseData) {
            if(responseType == ResponseType.Done) {
                if(responseData.savedDfg) {
                    self.model.setSavedDfg(responseData.savedDfg);
                }
            }
            
            if(response) {
                response();
            }
        });
    },
    
    dfgLoadDfg : function(event, response) {
        console.log("dfgLoadDfg");
        var dfg = $(event.currentTarget);
        var dfgFilename = dfg.attr("filename");
        
        var self = this;
        self.model.requestLoadDfg(dfgFilename, function(id, responseType, responseData) {
            //show error
            if(responseType == ResponseType.Error) {
                app.Flash.flashError("Error when try to load saved data flow graph: " + responseData + ".");
            }
            else if(responseType == ResponseType.Done) {
                if(responseData.DFG && responseData.filename) {
                    var modelId;
                    self.dfgDestroy(
                        function() { 
                            self.model.setDfgDef("");
                            self.model.setDfgDef(responseData);
                            app.Flash.flashSuccess('Saved data flow graph "' + responseData.filename + '" loaded.');
                        }, 
                        modelId, false, false);
                }
            }
            
            if(response) {
                response();
            }
        });
    },
    
    setDfgState : function(state) {
        //debug
        //console.log("set state: " + DfgState.getName(state));
        //console.log("... old states: " + DfgState.getNames(this.dfgState_));
        
        var stateSetted = false;
        switch(state) {
            case DfgState.DFG_STATE_NODES_LOADED:
                this.dfgState_ |= state;
                stateSetted = true;
                break;
            case DfgState.DFG_STATE_NOTCREATED:
                if(((this.dfgState_ & DfgState.DFG_STATE_STOPPED) == DfgState.DFG_STATE_STOPPED) ||
                  ((this.dfgState_ & DfgState.DFG_STATE_CREATED) == DfgState.DFG_STATE_CREATED)) {
                    // remove old state
                    this.dfgState_ &= ~DfgState.DFG_STATE_STOPPED;
                    this.dfgState_ &= ~DfgState.DFG_STATE_CREATED;
                    
                    // add new state
                    this.dfgState_ |= state;
                    stateSetted = true;
                }
                break;
            case DfgState.DFG_STATE_CREATED:
                if(((this.dfgState_ & DfgState.DFG_STATE_NOTCREATED) == DfgState.DFG_STATE_NOTCREATED)) {
                    // remove old state
                    this.dfgState_ &= ~DfgState.DFG_STATE_NOTCREATED;
                    
                    // add new state
                    this.dfgState_ |= state;
                    stateSetted = true;
                }
                break;
            case DfgState.DFG_STATE_STARTED:
                if(((this.dfgState_ & DfgState.DFG_STATE_CREATED) == DfgState.DFG_STATE_CREATED)) {
                    // remove old state
                    this.dfgState_ &= ~DfgState.DFG_STATE_CREATED;
                    
                    // add new state
                    this.dfgState_ |= state;
                    stateSetted = true;
                }
                else if(((this.dfgState_ & DfgState.DFG_STATE_STOPPED) == DfgState.DFG_STATE_STOPPED)) {
                    // remove old state
                    this.dfgState_ &= ~DfgState.DFG_STATE_STOPPED;
                    
                    // add new state
                    this.dfgState_ |= state;
                    stateSetted = true;
                }
                break;
            case DfgState.DFG_STATE_STOPPED:
                if(((this.dfgState_ & DfgState.DFG_STATE_STARTED) == DfgState.DFG_STATE_STARTED)) {
                    // remove old state
                    this.dfgState_ &= ~DfgState.DFG_STATE_STARTED;
                    
                    // add new state
                    this.dfgState_ |= state;
                    stateSetted = true;
                }
                break;
        }
        
        //debug
        //console.log("... new states: " + DfgState.getNames(this.dfgState_));
        
//        if(stateSetted) {
//            this.onStateChanged();    
//        }
    },
    
    onStateChanged : function() {
        var buttons = {
            load: { disabled: false, selector: "#dfgLoad", type: "attr"},
            create: { disabled: false, selector: "#dfgCreate", type: "attr"},
            start: { disabled: false, selector: "#dfgStart", type: "attr"},
            stop: { disabled: false, selector: "#dfgStop", type: "attr"},
            destroy: { disabled: false, selector: "#dfgDestroy", type: "attr"},
            reset: { disabled: false, selector: "#dfgReset", type: "attr"},
            otherAction: { disabled: false, selector: "#dfgOtherAction", type: "attr"},
            saveDfg: { disabled: false, selector: "#dfgSaveDfg", type: "attr"},
            loadDfg: { disabled: false, selector: ".dfgLoadDfg", type: "class"},
        };
        
        if(((this.dfgState_ & DfgState.DFG_STATE_NODES_LOADED) != DfgState.DFG_STATE_NODES_LOADED)) {
            app.Wait.setWaitHtml("#dfgLoad", "Load nodes");
            
            buttons.create.disabled = true;
            buttons.start.disabled = true;
            buttons.stop.disabled = true;
            buttons.destroy.disabled = true;
            buttons.reset.disabled = true;
            buttons.otherAction.disabled = true;
            buttons.saveDfg.disabled = true; 
            buttons.loadDfg.disabled = true; 
        }
        else {
            app.Wait.setWaitHtml("#dfgLoad", "Reload nodes");
            
            buttons.create.disabled = false;
            buttons.start.disabled = false;
            buttons.stop.disabled = false;
            buttons.destroy.disabled = false;
            buttons.reset.disabled = false;
            buttons.otherAction.disabled = false; 
            buttons.saveDfg.disabled = false; 
            buttons.loadDfg.disabled = false;
        
            if(((this.dfgState_ & DfgState.DFG_STATE_NOTCREATED) == DfgState.DFG_STATE_NOTCREATED)) {
                buttons.start.disabled = true;
                buttons.stop.disabled = true;
                buttons.destroy.disabled = true;
            }
            
            else if(((this.dfgState_ & DfgState.DFG_STATE_CREATED) == DfgState.DFG_STATE_CREATED)) {
                buttons.create.disabled = true;
                buttons.stop.disabled = true;
                buttons.loadDfg.disabled = true;
            }
            
            else if(((this.dfgState_ & DfgState.DFG_STATE_STARTED) == DfgState.DFG_STATE_STARTED)) {
                buttons.create.disabled = true;
                buttons.start.disabled = true;
                buttons.destroy.disabled = true;
                buttons.reset.disabled = true;
                buttons.loadDfg.disabled = true;
            }
            
            else if(((this.dfgState_ & DfgState.DFG_STATE_STOPPED) == DfgState.DFG_STATE_STOPPED)) {
                buttons.create.disabled = true;
                buttons.stop.disabled = true;
                buttons.loadDfg.disabled = true;
            }
        }
        
        // enable/disable buttons
        for(var p in buttons){
            if(buttons[p].disabled) {
                if(buttons[p].type == "attr") {
                    $(buttons[p].selector).attr("disabled", "disabled");
                }
                else { // "class"
                    $(buttons[p].selector).addClass("disabled");
                }
            }
            else {
                if(buttons[p].type == "attr") {
                    $(buttons[p].selector).removeAttr("disabled");
                }
                else {
                    $(buttons[p].selector).removeClass("disabled");
                }
            }
        }
    },
    
    /********************
    ** BLOCKLY SECTION **
    *********************/
    initializeBlockly : function() {
        //init style 4 screen
        var blocklyScreen = $('#script-generator-screen');
        blocklyScreen.css({
            "height": "500px",
            "width": "500px",
        });
        
        // add script generator console
        var blocklyConsole = $('#script-generator-console');
        blocklyConsole.append('<textarea id="script-generator-scriptScreen" rows="5" cols="80"></textarea>');
        
        this.registerBlocks();
        
        // generate toolbox
        var toolbox = $("#script-generator-toolbox");
//        toolbox.append( // controls category
//            '<category name="Control">\
//                <block type="controls_if"></block>\
//                <block type="controls_repeat_ext"></block>\
//            </category>');
//        toolbox.append( // logic category
//            '<category name="Logic">\
//            <block type="logic_compare"></block>\
//            <block type="math_number"></block>\
//            <block type="math_arithmetic"></block>\
//            <block type="text"></block>\
//            <block type="text_print"></block>\
//        </category>');
//        // other categories
//        toolbox.append('<category name="Variables" custom="VARIABLE"></category>');
//        toolbox.append('<category name="Functions" custom="PROCEDURE"></category>');
        toolbox.append( // dataflow graf category
            '<category name="Dataflow graf">\
                <block type="connect"></block>\
                <block type="test"></block>\
            </category>');
        
        
        // load blockly
        Blockly.inject(
            document.getElementById('script-generator-screen'),
            {
                  path : './js/blockly/'
                , toolbox : document.getElementById('script-generator-toolbox')
                , scrollbars : false //If false, supress scrollbars that appear if the toolbox has categories. Defaults to true.
                //, collapse : true //Allows blocks to be collapsed or expanded. Defaults to true if the toolbox has categories, false otherwis
                //, maxBlocks : Infinity //Maximum number of blocks that may be created. Useful for student exercises. Defaults to Infinity.
                //, readOnly : false //If true, prevent the user from editing. Supresses the toolbox and trashcan. Defaults to false.
                //, rtl : false //If true, mirror the editor for Arabic or Hebrew locales. See RTL demo. Defaults to false.
                //, trashcan : true // Displays or hides the trashcan. Defaults to true if the toolbox has categories, false otherwise.

            });
        
        //window.setTimeout(loadScript, 0);
        //this.loadScript();
        
        // add change listener 
        Blockly.addChangeListener(this.scriptWriter);  
    },
    
    registerBlocks : function() {
        Blockly.Blocks['test'] = {
            init: function() {
                this.setHelpUrl('');
                this.appendDummyInput().
                    appendField("test");
                this.appendDummyInput()
                    .appendField("   param1")
                    .appendField(new Blockly.FieldTextInput(""), "PARAM1")
                this.setPreviousStatement(true);
                this.setNextStatement(true);
                this.setTooltip('');
            }
        };
        
        Blockly.JavaScript['test'] = function(block) {
            var param1 = block.getFieldValue('PARAM1');
            
            var code = 'test(' + param1 + ');\n';
            
            return code;
        };
    },
    
    scriptWriter: function() {
        //var xml = Blockly.Xml.workspaceToDom(Blockly.getMainWorkspace());
        //var code = Blockly.Xml.domToText(xml)
        var code = Blockly.JavaScript.workspaceToCode();
        document.getElementById('script-generator-scriptScreen').value = code;
    },
    
    loadScript: function(xmlScript) {
        //var savedXml = Blockly.Xml.textToDom('<xml><block type="connect" id="2" x="148" y="385"><field name="XOB1">subject</field><field name="XVAR1">out</field><field name="XOB2">observer</field><field name="XVAR2">in</field></block></xml>');
        var savedXml = Blockly.Xml.textToDom(xmlScript);
        Blockly.Xml.domToWorkspace(Blockly.getMainWorkspace(), savedXml);
    },
});