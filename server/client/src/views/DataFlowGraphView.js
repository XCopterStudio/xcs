var NODE_TYPE = ENUM(
    "PUBLIC",
    "PRIVATE",
    "SPECIAL"
);

var DataFlowGraphView = Backbone.View.extend({
    id : 'data-flow-graph',
    
    el : 'body',
    
    events : {
        "click #dfgLoadDfg": "dfgShowFiles",
        "keyup #dfgSaveAsDfg-filename": "onChangeDfgSaveAsDfgFilename"
    },
        
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
        
    initialize : function() {
        /* GUI elements */
        this.$dfgSave = this.$('#dfgSaveDfg');
        this.$dfgSaveAs = this.$('#dfgSaveAsDfg');
        this.$dfgSaveAsFilename = this.$('#dfgSaveAsDfg-filename');
        
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
        
        //bind functions
        this.refresh = _.bind(this.refresh, this);
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
        
        var saveDfgAsAction = new WaitAction("#dfgSaveAsDfg", WaitActionType.Click)
        saveDfgAsAction.set("action", function() { saveDfgAsAction.start(); self.dfgSaveAsDfg(undefined, function() { saveDfgAsAction.stop() }); });
        app.Wait.setWaitAction(saveDfgAsAction);
        
        var saveDfgAction = new WaitAction("#dfgSaveDfg", WaitActionType.Click)
        saveDfgAction.set("action", function() { if(self.$dfgSave.parent().hasClass('disabled')) {return;} saveDfgAction.start(); self.dfgSaveAsDfg(self.lastFileName, function() { saveDfgAction.stop() }); });
        app.Wait.setWaitAction(saveDfgAction);
        
        this.dfgGraph = new joint.dia.Graph;
        this.listenTo(this.dfgGraph, 'change:target', this.setLink);
        this.listenTo(this.dfgGraph, 'change:source', this.setLink);

        this.listenTo(this.dfgGraph, 'remove', this.onRemoveNode);
        this.listenTo(this.dfgGraph, 'add', this.onAddNode);
        this.listenTo(this.dfgGraph, 'add', this.onResizeNeed);
        this.listenTo(this.dfgGraph, 'change:position', this.onResizeNeed);


        this.initializeDfgToolbox4Drop();
           
        this.dfgPaper = new joint.dia.Paper({
            el: $('#flow-graph-screen'),
            width: 1024, 
            height: 500, 
            gridSize: 10,   // size of grid in px (how many px reprezents one cell)
            model: this.dfgGraph,
            validateConnection: function(cellViewS, magnetS, cellViewT, magnetT, end, linkView) {
                if(magnetS && magnetT && magnetS.getAttribute("type") == "out" && magnetT.getAttribute("type") == "in") {
                    var idS = magnetS.getAttribute("port");
                    var idT = magnetT.getAttribute("port");

                    var semTT = cellViewT.model.get("inPortsType")[idT].semType;
                    var synTT = cellViewT.model.get("inPortsType")[idT].synType;
                    
                    return self.model.isConnectable(cellViewS.model.get("outPortsType")[idS].synType, cellViewS.model.get("outPortsType")[idS].semType, synTT, semTT);
                }
                
                return false;
            },
            validateMagnet: function(cellView, magnet) {
                return magnet.getAttribute('type') != "in";
            },
            snapLinks: { radius: 45 },
        });
    },
    
    refresh : function() {
        var nodes = this.dfgGraph.getElements();
        for(var i = 0; i < nodes.length; ++i){
            var view = this.dfgPaper.findViewByModel(nodes[i]);
            if(view) {
                view.render();
            }
        }
            
        var links = this.dfgGraph.getLinks();
        for(var i = 0; i < links.length; ++i){
            var view = this.dfgPaper.findViewByModel(links[i]);
            if(view) {
                view.render();
            }
        }
    },
    
    onAddNode : function(model) {
        this.refreshTooltips(model);
    },
    
    onResizeNeed: function(model) {
        this.dfgPaper.fitToContent(1024, 500, 50);
    },
    
    refreshTooltips : function(model) {
        var modelId = model.get("id");
        var modelOrigId = model.get("origId");
        if(modelId && modelOrigId) {
            var foo = setInterval(function () {
                var cloneNode = $('.DataFlowGraphCloneNode[model-id="' + modelId +'"]');
                if(cloneNode.length > 0) {
                    cloneNode.find('.port-tooltip').tooltip();
                    clearInterval(foo);
                }
            }, 1000)
        }
    },
    
    onRemoveNode : function(model) {
        // create widgets
        var prototypeId = model.get("origId");
        var cloneId = model.get("id");
        
        // check if model is node
        if(prototypeId && cloneId) {
            this.checkGuiWidgets();
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
        var modelId;
        if(this.dfgCounter[nodeId] == 1) {
            modelId = nodeId;   
        } else {
            modelId = nodeId + this.dfgCounter[nodeId];
        }
        modelId = modelId.charAt(0).toLowerCase() + modelId.slice(1);
        
        var m = new (joint.shapes.dfg.DataFlowGraphCloneNode)({
            position: { x: x, y: y },
        });
        m.setId(modelId);
        m.setOrigId(nodeId);
        m.setLabel(modelId);
        
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
                m.addRegisterXVar(registerName);
            });
        }
        
        m.setAutoSize();
        
        m.setState(NodeState.NOTCREATED);
        
        this.dfgGraph.addCell(m);
        this.dfgModels.addModel(modelId, m);
        var self = this;
        m.on('change:nodeState', function(){ self.refreshTooltips(m); });
        
        //set context menu
        var self = this;
        $("#flow-graph-screen .DataFlowGraphCloneNode").contextMenu({
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
        } else {
            this.setWellLink(link);
        }
    },
    
    setBadLink : function(link) {
        link.attr({
            '.connection': { stroke: 'red' },
            '.marker-target': { fill: 'red', d: 'M 10 0 L 0 5 L 10 10 z' }
        });
    },
    
    setWellLink : function(link) {
        link.attr({
            '.connection': { stroke: 'green' },
            '.marker-target': { fill: 'green', d: 'M 10 0 L 0 5 L 10 10 z' }
        });
    },
    
    onSavedDfgChange : function(model) {
        var dfgs = model.get("savedDfg");
        
        var modalDfgItems = $('#modal-dfg-files .modal-body table');  
        
        // reset saved dfgs
        if(dfgs.length > 0) {
            //NOTE: there should be no need to unregister old click events - jquery should handle it yourself
            modalDfgItems.html('<thead><tr><th>#</th><th>File Name</th><th>Actions</th></tr></thead><tbody></tbody>');
            modalDfgItems = modalDfgItems.find("tbody");
        } else {
            modalDfgItems.html('<thead><tr><th>No DFG for load...</th></tr></thead>');
        }
        
        var self = this;
        var loadDfgAction = {};
        var removeDfgAction = {};
        for(var i = 0; i < dfgs.length; ++i) {
            var idLoad = "dfgLoadDfg_" + this.trimId(dfgs[i]);
            var idRemove = "dfgRemoveDfg_" + this.trimId(dfgs[i]);
            
            modalDfgItems.append(
                '<tr>\
                    <td>' + (i+1) + '</td>\
                    <td>' + dfgs[i] + '</td>\
                    <td>\
                        <button type="button" class="dfgLoadDfg btn btn-link" id="' + idLoad + '" filename="' + dfgs[i] + '" tabindex="-1" href="#">Load</button>\
                        <button type="button" class="dfgRemoveDfg btn btn-link" id="' + idRemove + '" filename="' + dfgs[i] + '" tabindex="-1" href="#">Remove</button>\
                    </td>\
                </tr>');
            
            // set load action
            loadDfgAction[idLoad] = new WaitAction("#" + idLoad, WaitActionType.Click);
            loadDfgAction[idLoad].set("action", function(event) { 
                var hrefId = $(event.currentTarget).attr("id");
                loadDfgAction[hrefId].start(); 
                self.dfgLoadDfg(event, function() { 
                    loadDfgAction[hrefId].stop(); 
                }); 
            });
            app.Wait.setWaitAction(loadDfgAction[idLoad]);
            
            // set remove action
            removeDfgAction[idRemove] = new WaitAction("#" + idRemove, WaitActionType.Click);
            removeDfgAction[idRemove].set("action", function(event) { 
                var hrefId = $(event.currentTarget).attr("id");
                removeDfgAction[hrefId].start(); 
                self.dfgRemoveDfg(event, function() { 
                    removeDfgAction[hrefId].stop(); 
                }); 
            });
            app.Wait.setWaitAction(removeDfgAction[idRemove]);
        }
    },
    
    onDfgDefChange : function(model) {    
        var graph = model.get("dfgDef");
        if(graph && graph.DFG && graph.filename) {
            this.loadGraph(graph.filename, graph.DFG);
        }
    },
     
    onDdfgChange : function(model) {
        var graph = model.get("ddfg");       
        this.loadGraph("default", graph);
    },
    
    loadGraph : function(filename, dfg, append) {        
        // default value 4 append is false
        append = typeof append !== 'undefined' ? append : false;
        
        if(!dfg || !filename) {
            return;
        }
        
        try {
            this.lastFileName = filename;
            $("#dfgSaveDfgPresentation").removeClass("disabled");
            $("#dfgSaveDfg .filename").text(" (" + this.lastFileName + ")");
            
            if(!append) {
                this.dfgReset(false);
            }
                                    
            var dfgJson = JSON.parse(dfg);
            if(dfgJson.cells) {
                var nodes = {};
                var notValidNodes = [];
                
                // nodes
                for(var i = 0; i < dfgJson.cells.length; ++i) {
                    var cell = dfgJson.cells[i];
                    
                    // node
                    if(cell.origId) {
                        if(this.dfgToolboxNodes[cell.origId]) {
                            nodes[cell.id] = this.addNodeToGraph(cell.origId, cell.position.x, cell.position.y);
                        } else {
                            notValidNodes.push(cell.origId);
                        }
                    }
                }
                
                // show warning
                if(notValidNodes.length > 0) {
                    app.Flash.flashWarning("Loaded DFG file " + filename + " is not valid. Some nodes can not be loaded: " + JSON.stringify(notValidNodes) + ".");
                }
                
                // links
                var error = false;
                for(var i = 0; i < dfgJson.cells.length; ++i) {
                    var cell = dfgJson.cells[i];
                    
                    //cell
                    if(cell.source) {
                        var vertices = ((cell.vertices) ? cell.vertices : []);
                        var sourceSelector = ((cell.source.selector) ? cell.source.selector : "");
                        var targetSelector = ((cell.target.selector) ? cell.target.selector : "");
                        var setted = false;
                        
                        if(cell.source.id && nodes[cell.source.id] && cell.source.port && 
                           cell.target.id && nodes[cell.target.id] && cell.target.port) {
                            
                            var srcModel = this.dfgModels[nodes[cell.source.id]];
                            var tgtModel = this.dfgModels[nodes[cell.target.id]];
                            
                            if(srcModel && srcModel.hasOutputPort(cell.source.port) && 
                               tgtModel && tgtModel.hasInputPort(cell.target.port)) {
                                this.addLinkToGraph(
                                    nodes[cell.source.id], cell.source.port, sourceSelector, 
                                    nodes[cell.target.id], cell.target.port, targetSelector,
                                    vertices);
                                
                                setted = true;
                            }
                        }
                        
                        if(!setted) {
                            error = true;
                        }
                        
                        if(!setted && cell.source.id && cell.target.id && cell.source.port && cell.target.port) {
                            console.log(cell.source.id + "." + cell.source.port + " >> " + cell.target.id + "." + cell.target.port);
                        }
                    }
                }
                
                if(error) {
                    app.Flash.flashWarning("Loaded DFG file " + filename + " is not valid. Some links can not be loaded.");
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
        
        //validation
        if(!this.dfgToolboxNodes[prototypeId]) {
            console.error("ERROR(onPrototypeRemove): " + prototypeName + " was not yet loaded!");
            return;
        }
        
        // stop listening to old model
        this.stopListening(this.dfgToolboxNodes[prototypeId]);
        
        //delete from Gui
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
                    }
                    
                    app.Wait.setWaitHtml("#dfgLoad", "Reload nodes");
                }
                
                // response
                if(response) {
                    response();
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
            var jsonDfg = this.dfgGraph.toJSON();
            
            if(jsonDfg.cells) {
                // prepare object 4 dfg info
                var dfg = {
                    prototype: [],
                    link: [],
                    registerXVar: []
                };
                
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
                        var linkCount = 0;
                        var unlinkCount = 0;
                        
                        if(responseData.prototype) {
                            for(var i = 0; i < responseData.prototype.length; ++i) {
                                var model = self.dfgModels[responseData.prototype[i]];
                                if(model) {
                                    model.setState(NodeState.CREATED);
                                    ++successCount;
                                }
                            }
                        }
                        
                        if(responseData.link) {
                            linkCount = responseData.link;
                        }
                        
                        if(responseData.unlink) {
                            unlinkCount = responseData.unlink;
                        }
                        
                        if(responseData.registerXVar) {
                            linkCount += responseData.registerXVar;
                        }
                        
                        if(responseData.unregisterXVar) {
                            unlinkCount += responseData.unregisterXVar;
                        }
                        
                        var msg = "";
                        if(successCount > 0) {
                            msg = "" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " created. ";
                        }
                        
                        if(linkCount > 0) {
                            msg += "" + linkCount + " " + (linkCount <= 1 ? "link is" : "links are") + " created. "
                        }
                        
                        if(unlinkCount > 0 ) {
                            msg += "" + unlinkCount + " " + (unlinkCount <= 1 ? "link is" : "links are") + " destroyed.";
                        }
                        
                        // show result
                        app.Flash.flashSuccess(msg);
                    }
                    
                    // create all widgets
                    self.checkGuiWidgets();
                    
                    //response action
                    if(response) {
                        response();
                    }
                });
            }
        } catch(ex) {
            // show error
            app.Flash.flashError("Error when try to create nodes: " + ex + "!");
            
            //response action
            if(response) {
                response();
            }
        }
    },
    
    checkGuiWidgets : function() {
        try {
            var views = [];
                        
            // load dfg 2 json object
            var jsonDfg = this.dfgGraph.toJSON()
            
            if(jsonDfg.cells) {
                for(var i = 0; i < jsonDfg.cells.length; ++i) {
                    var cell = jsonDfg.cells[i];
                    
                    // links
                    if(cell.source && cell.target && cell.source.id && cell.target.id && cell.type && cell.type == "link") {
                        // load clone info
                        var cloneId = cell.target.id;
                        var cloneModel = this.dfgModels[cloneId];
                        if(!cloneModel || 
                           cloneModel.get("origId") != "Gui" || 
                           !(cloneModel.getState() == NodeState.CREATED || 
                                cloneModel.getState() == NodeState.STARTED || 
                                cloneModel.getState() == NodeState.STOPPED)) {
                            continue;
                        }
                        
                        // load clone info about source
                        var srcCloneModel = this.dfgModels[cell.source.id];
                        if(!srcCloneModel || 
                           !(srcCloneModel.getState() == NodeState.CREATED || 
                             srcCloneModel.getState() == NodeState.STARTED ||
                             srcCloneModel.getState() == NodeState.STOPPED)) {
                            continue;
                        }
                        
                        // load prototypes info
                        var prototypeId = cloneModel.get("origId");
                        var modelPrototype = this.dfgToolboxNodes[prototypeId];
                        if(!modelPrototype) {
                            continue;
                        }
                        
                        views.push({
                            viewName: cell.target.port,
                            dataId: cell.source.id + "__" + cell.source.port,
                            prototypeDataId: srcCloneModel.get("origId") + "__" + cell.source.port,
                        });
                    }
                }
            }
                
                
            return app.DataView.setCreatedViews(views);
        }
        catch(ex) {
            app.Flash.flashError("Error when try to create/remove widgets: " + ex.message + "!");
            return false;    
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
                        self.model.trigger('nodeStart', model);
                        ++successCount;
                    }
                }

                // show result
                app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " started.\n");
            }
            
            //response action
            if(response) {
                response();
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
                        self.model.trigger('nodeStop', model);
                        ++successCount;
                    }
                }
                
                // show result
                app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " stopped.");
            }
            
            if(response) {
                response();
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
                            }
                        }
                        
                        self.checkGuiWidgets();
                        
                        // show result
                        if(successCount > 0) {
                            app.Flash.flashSuccess("" + successCount + " " + (successCount <= 1 ? "node is" : "nodes are") + " destroyed.");
                        } else if(checkDestroyed) {
                            app.Flash.flashError("Error when try to destroy nodes: nothing to destroy.");
                        }
                    }
                }
                
                if(response) {
                    response();
                }
            });
        } catch(ex) {
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
        } else if(response) {
            app.Flash.flashSuccess("Data flow graph is resetted.");
            response();
        }
    }, 
    
    dfgSaveAsDfg : function(filename, response) {
        if(!filename) {
            var inputFilename = $('#dfgSaveAsDfg-filename');
            
            // read filenam
            var filename = inputFilename.val().trim();
    
            // validate filename
            var errorMsg = '';
            if(this.model.dfgExist(filename)) {
                var self = this;
                
                // show question: rewrite
                app.ModalView.show(
                    "DFG file with the same name already exists. Do you want to rewrite it?",
                    ModalButtonType.NO | ModalButtonType.YES, 
                    function(result) {
                        if(result == ModalButtonType.NO) {
                            app.Flash.flashWarning("DFG was not saved.");
                            
                            // clean input 4 filename
                            inputFilename.val('');
                            
                            if(response) {
                                response();
                            }
                        }
                        else {
                            // rewrite
                            self.dfgSaveAsDfg(filename, response);
                            
                            // clean input 4 filename
                            inputFilename.val('');
                        }
                });
                
                return;
            }
            
            //show error message
            if(errorMsg != '') {
                app.Flash.flashWarning(errorMsg);
                
                if(response) {
                    response();
                }
                
                return;
            }        
    
            // clean input 4 filename
            inputFilename.val('');
        }
        
        // load dfg 2 json object
        var jsonDfg = this.dfgGraph.toJSON();
        
        //send request
        var self = this;
        self.model.requestSaveDfg(JSON.stringify(jsonDfg), filename, true, function(id, responseType, responseData) {
            if(responseType == ResponseType.Error) {
                app.Flash.flashError("Error when try to save DFG: " + responseData + ".");
            }
            else if(responseType == ResponseType.Done) {
                if(responseData.savedDfg) {
                    self.model.setSavedDfg(responseData.savedDfg);
                }
                
                // set filename for save
                self.lastFileName = filename;
                $("#dfgSaveDfgPresentation").removeClass("disabled");
                $("#dfgSaveDfg .filename").text(" (" + self.lastFileName + ")");
                
                app.Flash.flashSuccess("DFG was saved");
            }
            
            if(response) {
                response();
            }
        });
    },
    
    dfgLoadDfg : function(event, response) {        
        var dfg = $(event.currentTarget);
        var dfgFilename = dfg.attr("filename");
        
        var self = this;
        self.model.requestLoadDfg(dfgFilename, function(id, responseType, responseData) {
            //show error
            if(responseType == ResponseType.Error) {
                app.Flash.flashError("Error when try to load saved data flow graph: " + responseData + ".");
            } else if(responseType == ResponseType.Done) {
                if(responseData.DFG && responseData.filename) {
                    self.model.setDfgDef("");
                    self.model.setDfgDef(responseData);
                    app.Flash.flashSuccess('Saved data flow graph "' + responseData.filename + '" loaded.');
                }
            }
            
            if(response) {
                response();
            }
        });
    },
    
    dfgRemoveDfg: function(event, response) {        
        var dfg = $(event.currentTarget);
        var dfgFilename = dfg.attr("filename");
        
        var self = this;
        self.model.requestRemoveDfg(dfgFilename, function(id, responseType, responseData) {
            //show error
            if(responseType == ResponseType.Error) {
                app.Flash.flashError("Error when try to remove saved data flow graph: " + responseData + ".");
            } else if(responseType == ResponseType.Done) {
                if(responseData.savedDfg) {
                    self.model.setSavedDfg(responseData.savedDfg);
                }
                app.Flash.flashSuccess('Data flow graph "' + responseData.filename + '" was successfully removed.');
            }
            
            if(response) {
                response();
            }
        });
    },
    
    dfgShowFiles: function() {
        app.ModalView.showModal("#modal-dfg-files");
    },
    
    onChangeDfgSaveAsDfgFilename: function() {
        var filename = this.$dfgSaveAsFilename.val();
        this.$dfgSaveAs.prop('disabled', !filename);
    }
});