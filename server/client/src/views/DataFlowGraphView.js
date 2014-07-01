//var DfgState = ENUM(
//    "DFG_STATE_NODES_LOADED", "DFG_STATE_CREATED", "DFG_STATE_STARTED", "DFG_STATE_STOPPED",    // DFG STATE
//    "DFG_STATE_USER_DFG_LOADED", "DFG_STATE_DEFAULT_DFG_LOADED", "DFG_STATE_NODE_DFG_LOADED"    // LOADED DFG TYPE
//    );

var DataFlowGraphView = Backbone.View.extend({
    id : 'data-flow-graph',
    
    el : '#dfg',
    
    events : {
        "click #dfgLoad" : "dfgLoad",
        "click #dfgCreate" : "dfgCreate",
        "click #dfgStart" : "dfgStart",
        "click #dfgStop" : "dfgStop",
        "click #dfgReset" : "dfgReset",
        "click #dfgSaveDfg" : "dfgSaveDfg",
        "click .dfgLoadDfg" : "dfgLoadDfg",
    },
    
    dfgGraph : {},
    
    dfgToolboxNodes : {},
    
    dfgModels : {
        addModel: function(id, model) {
            this[id] = model;
            
            var self = this;
            
            // remove model on double click
            $('g[model-id="' + id + '"]').dblclick(function(eventObject){
                self.removeModel(this.getAttribute('model-id'));
            }); 
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
                    ps.push(this[p]);
                }
            }
            
            for(var i = 0; i < ps.length; ++i) {
                this.removeModel(ps[i]);
            }
        },
    },
    
    dfgCounter : [],
    
    initialize : function() {
        this.model = new DataFlowGraph();
        
        this.onInputChange(app.XcopterState);      
        this.listenTo(app.XcopterState, "change:onboard", this.onInputChange);
        
        this.listenTo(this.model, "change:dataFlowGraph", this.onDataFlowGraphChange);
        this.listenTo(this.model.get("xprototype"), "add", this.onPrototypeAdd);
        this.listenTo(this.model.get("xclone"), "add", this.onCloneAdd);
        this.listenTo(this.model.get("xprototype"), "remove", this.onPrototypeRemove);
        this.listenTo(this.model.get("xclone"), "remove", this.onCloneRemove);

        this.listenTo(this.model, "change:savedDfg", this.onSavedDfgChange);
        this.listenTo(this.model, "change:dfgDef", this.onDfgDefChange);
        
        this.initializeDfg();
        
        //this.initializeBlockly();
    },
    
    
    /****************************
    ** DATA FLOW GRAPH SECTION **
    ****************************/
    initializeDfg : function() {
        var flowGraphConsole = $('#flow-graph-console');
        flowGraphConsole.append('<textarea id="flow-graph-txt" rows="15" cols="150"></textarea>');
        
        this.dfgGraph = new joint.dia.Graph;
        this.listenTo(this.dfgGraph, 'change:target', this.setLink);
        this.listenTo(this.dfgGraph, 'change:source', this.setLink);

        this.dfgGraph.on('change', function(model) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });
        this.dfgGraph.on('add', function(cell) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });
        this.dfgGraph.on('remove', function(cell) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });

        this.initializeDfgToolbox4Drop();
           
        var paper = new joint.dia.Paper({
            el: $('#flow-graph-screen'),
            width: 1024, 
            height: 500, 
            gridSize: 10,   // size of grid in px (how many px reprezents one cell)
            model: this.dfgGraph,
            validateConnection: function(cellViewS, magnetS, cellViewT, magnetT, end, linkView) {
                if(magnetS && magnetT && magnetS.getAttribute("type") == "in" && magnetT.getAttribute("type") == "out") {
                    var idS = magnetS.getAttribute("port");
                    var idT = magnetT.getAttribute("port");

                    var sameSemT = cellViewS.model.inPortsType[idS].semType == cellViewT.model.outPortsType[idT].semType;
                    var sameSynT = cellViewS.model.inPortsType[idS].synType == cellViewT.model.outPortsType[idT].synType;
                    
                    var valid = sameSemT && sameSynT;
                    
                    return valid;
                }
                
                return false;
            },
            validateMagnet: function(cellView, magnet) {
                return magnet.getAttribute('type') != "out";
            },
            snapLinks: { radius: 45 },
        });
        
        this.model.requestLoad();
    },
    
    trimId : function(id) {
        return id.replace(/ /g,'');
    },
    
    addNode2DfgToolbox : function(id, title) {
        if(!title) {
            title = id;
        }
        
        var toolbox = $('#flow-graph-toolbox');
        toolbox.append('\
            <div class="panel panel-default" id="xprototype_' + id + '">         \
                <div class="panel-heading">                                     \
                    <!--<h4 class="panel-title">  -->                                 \
                        <a data-toggle="collapse" href="#collapse_' + id + '" class="drag_init" id="' + id + '">  \
                            ' + title + '                                      \
                        </a>                                                    \
                        <span class="badge  pull-right" data-toggle="tooltip" title="počet instancí">0</span> \
                    <!--</h4>  -->                                                     \
                </div>                                                         \
                <div id="collapse_' + id + '" class="panel-collapse collapse">  \
                   <!--<div class="panel-body drag_init" id="' + id + '">\
                        bla bla bla\
                    </div>-->     \
                </div>                                                          \
            </div>                                                              \
        ');
        
        this.initializeDfgToolbox4Drag();
    },
    
    removeNodeFromDfgToolbox : function(id) {
        var node = $('#xprototype_' + id);
        node.remove();
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
        
        // get xvars and xinput ports
        var xvars = modelPrototype.get("xvar");
        var xinputPorts = modelPrototype.get("xinputPort");
        
        //get name
        var prototypeName;            
        var modelId;
        if(this.dfgCounter[nodeId] == 1) {
            prototypeName = modelPrototype.get("name");
            modelId = nodeId;   
        }
        else {
            prototypeName = modelPrototype.get("name") + " " + this.dfgCounter[nodeId];
            modelId = nodeId + this.dfgCounter[nodeId];
        }
        
        var m = new DataFlowGraphDefaultModel({
            position: { x: x, y: y },
        });
        m.setId(modelId);
        m.setOrigId(nodeId);
        m.setLabel(prototypeName);
        
        // get all xvars
        if(xvars) {
            xvars.forEach(function(xvar) {
                var xvarName = xvar.get("name");
                var xvarSynType = xvar.get("synType");
                var xvarSemType = xvar.get("semType");   
                m.addInputPort(xvarName, xvarSemType, xvarSynType);
            });
        }
        
        // get all xinputports
        if(xinputPorts) {
            xinputPorts.forEach(function(xvar) {
                var xinputPortName = xvar.get("name");
                var xinputPortSynType = xvar.get("synType");
                var xinputPortSemType = xvar.get("semType");
                m.addOutpuPort(xinputPortName, xinputPortSemType, xinputPortSynType);
            });
        }
        
        m.setAutoSize();
        
        this.dfgGraph.addCell(m);
        this.dfgModels.addModel(modelId, m);
        
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
        loadItems.html('');
        
        for(var i = 0; i < dfgs.length; ++i) {
            loadItems.append('<li><a class="dfgLoadDfg" id="dfgLoadDfg_' + this.trimId(dfgs[i]) + '">' + dfgs[i] + '</a></li>');
        }
    },
    
    onDfgDefChange : function(model) {
        // TODO: show some question? 
        var graph = model.get("dfgDef");
        this.loadGraph(graph.DFG);
    },
     
    onDataFlowGraphChange : function(model) {
        // TODO: show some question? 
        var graph = model.get("dataFlowGraph");       
        this.loadGraph(graph);
    },
    
    loadGraph : function(dfg, append) {
        // TODO: load default DFG
        //console.log("load graph: " + dfg);
        console.log("load graph");
        
        // default value 4 append is false
        append = typeof append !== 'undefined' ? append : false;
        
        try {
            //this.dfgGraph.fromJSON(JSON.parse(dfg));
            //this.dfgGraph.resetCells(this.dfgGraph.getElements());
            
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
            console.log("...ERROR: " + ex.message);
            this.dfgReset(false);
        }
    },
    
    onPrototypeAdd : function(modelPrototype) {
        //DEBUG
        //console.log("onPrototypeAdd " + JSON.stringify(modelPrototype.toJSON()));
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        var prototypeId = this.trimId(prototypeName);
        
        if(this.dfgToolboxNodes[prototypeId]) {
            console.error("ERROR(onPrototypeAdd): " + prototypeName + " was already loaded!");
            return;
        }
        
        // add 2 toolbox - show to user
        this.addNode2DfgToolbox(prototypeId, prototypeName);
        this.dfgToolboxNodes[prototypeId] = modelPrototype;
        
        //DEBUG
        //console.log("added: " + prototypeName + " = " + this.dfgToolboxNodes[prototypeName].get("name"));
        
        //watch 4 changes
        this.listenTo(modelPrototype, "change", this.onPrototypeChange);
    },
    
    onCloneAdd : function(modelClone) {
        //DEBUG
        console.log("onCloneAdd " + JSON.stringify(modelClone.toJSON()));
        
        //watch 4 changes
        this.listenTo(modelClone, "change", this.onCloneChange);
        
        // TODO: add new clone 2 toolbox
        //...
        
        // get prototype name
        var cloneName = modelClone.get("name");
        var cloneId = this.trimId(cloneName);
        
        // get all xvars
        modelClone.get("xvar").forEach(function(xvar) {
            var xvarName = xvar.get("name");
            var xvarSynType = xvar.get("synType");
            var xvarSemType = xvar.get("semType");
        });
        
        // get all xinputports
        modelClone.get("xinputPort").forEach(function(xvar) {
            var xinputPortName = xvar.get("name");
            var xinputPortSynType = xvar.get("synType");
            var xinputPortSemType = xvar.get("semType");
        });
    },
    
    onPrototypeRemove : function(modelPrototype) {
        //DEBUG
        //console.log("onPrototypeRemove " + JSON.stringify(modelPrototype.toJSON()));
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        var prototypeId = this.trimId(prototypeName);
        
        //DEBUG
        //console.log("onPrototypeRemove " + prototypeName);
        
        //validation
        if(!this.dfgToolboxNodes[prototypeId]) {
            console.error("ERROR(onPrototypeRemove): " + prototypeName + " was not yet loaded!");
            return;
        }
        
        // stop listening to old model
        this.stopListening(this.dfgToolboxNodes[prototypeId]);
        
        //delete from GUI
        this.removeNodeFromDfgToolbox(prototypeId);
        
        // delete old model
        delete this.dfgToolboxNodes[prototypeId];
    },
    
    onCloneRemove : function(modelClone) {
        //DEBUG
        console.log("onCloneRemove " + JSON.stringify(modelClone.toJSON()));
        
        //TODO: remove clone from GUI
    },
    
    onPrototypeChange : function(modelPrototype) {
        //DEBUG
        //console.log("onPrototypeChange " + JSON.stringify(modelPrototype.toJSON()));
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        var prototypeId = this.trimId(prototypeName);
        
        //DEBUG
        //console.log("onPrototypeChange: " + prototypeName);
        
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
        this.listenTo(modelPrototype, "change", this.onPrototypeChange);
    },
    
    onCloneChange : function(modelClone) {
        //DEBUG
        console.log("onCloneChange " + JSON.stringify(modelClone.toJSON()));
        
        //TODO: change existed clone
        //...
        
        // get prototype name
        var cloneName = modelClone.get("name");
        var cloneId = this.trimId(cloneName);
        
        // get all xvars
        modelClone.get("xvar").forEach(function(xvar) {
            var xvarName = xvar.get("name");
            var xvarSynType = xvar.get("synType");
            var xvarSemType = xvar.get("semType");
        });
        
        // get all xinputports
        modelClone.get("xinputPort").forEach(function(xvar) {
            var xinputPortName = xvar.get("name");
            var xinputPortSynType = xvar.get("synType");
            var xinputPortSemType = xvar.get("semType");
        });
    },
    
    onInputChange : function(xcState) {
        var data = xcState.get("onboard");
        this.model.setData(data);
                
        //console.log("incoming!!!!!!!!!!!!!!!!!!!!!!!!!");
//        var test = {
//            "type" : "onboard",
//            "data" : {
//                "dataFlowGraph" : '<xml><block type="connect" id="2" x="148" y="385"><field name="XOB1">subject</field><field name="XVAR1">out</field><field name="XOB2">observer</field><field name="XVAR2">in</field></block></xml>',
//                "prototype" : [{
//                    "name" : "XXci",
//                    "var" : [{
//                        "name" : "fly",
//                        "synType" : "xcs::nodes::xci::FlyParam",
//                        "semType" : "FLY_PARAM"
//                    }],
//                    "inputPort" : [{
//                        "name" : "command",
//                        "synType" : "std::string",
//                        "semType" : "COMMAND"
//                    }]
//                }],
//                "clone" : [{
//                    "name" : "Dodo",
//                    "prototype" : "XXci",
//                    "var" : [{
//                        "name" : "fly",
//                        "synType" : "xcs::nodes::xci::FlyParam",
//                        "semType" : "FLY_PARAM"
//                    }],
//                    "inputPort" : [{
//                        "name" : "command",
//                        "synType" : "std::string",
//                        "semType" : "COMMAND"
//                    }]
//                }, {
//                    "name" : "Parrot",
//                    "prototype" : "XXci",
//                    "var" : [{
//                        "name" : "fly",
//                        "synType" : "xcs::nodes::xci::FlyParam",
//                        "semType" : "FLY_PARAM"
//                    }],
//                    "inputPort" : [{
//                        "name" : "command",
//                        "synType" : "std::string",
//                        "semType" : "COMMAND"
//                    }]
//                }]
//            }
//        };
    },

    dfgLoad : function() {
        var self = this;
        
        self.model.reset();
        self.model.requestLoad(function(id, responseType, responseData) {
            if(responseType == ResponseType.Done) {
                if(responseData.prototype) {
                    self.model.setPrototype(responseData.prototype);
                }
                if(responseData.clone) {
                    self.model.setClone(responseData.clone);
                }
            }
        });
    },
    
    dfgCreate : function() {
        // load dfg 2 json object
        var jsonDfg = this.dfgGraph.toJSON()
        
        if(jsonDfg.cells) {
            // prepare object 4 dfg info
            var dfg = {
                prototype: [],
                clone: [],
                link: []
            };
                
            for(var i = 0; i < jsonDfg.cells.length; ++i) {
                var cell = jsonDfg.cells[i];
                
                // prototypes and clones
                if(cell.inPorts && cell.outPorts && cell.id && cell.origId) {
                    // load prototypes info
                    var prototypeId = cell.origId;
                    var cloneId = cell.id;
                    var modelPrototype = this.dfgToolboxNodes[prototypeId];
                    if(!modelPrototype) {
                        console.error("ERROR(dfgCreate): " + prototypeId + " is not loaded!");
                        return;
                    }
                    
                    // prepate loaded prototypes info 4 sending
                    dfg.prototype.push({
                        id: cloneId,
                        name: modelPrototype.get("name"),
                    });
                    
                    //TODO: load and send information about clones
                }
                // links
                else if(cell.source && cell.target && cell.source.id && cell.target.id && cell.type && cell.type == "link") {
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
            
            // send request
            this.model.requestCreate(dfg);
        }
    },
    
    dfgStart : function() {
        this.model.requestStart();
    },
    
    dfgStop : function() {
        this.model.requestStop();
    },
    
    dfgReset : function(all) {
        // default value 4 all is true
        all = typeof all !== 'undefined' ? all : true;
        
        this.dfgModels.clear();
        this.dfgGraph.clear();
        
        if(all) {
            var self = this;
            self.model.reset();
            self.model.requestReset(function(id, responseType, responseData) {
                if(responseType == ResponseType.Done) {
                    if(responseData.prototype) {
                        self.model.setPrototype(responseData.prototype);
                    }
                    if(responseData.clone) {
                        self.model.setClone(responseData.clone);
                    }
                }
            });
        }
    }, 
    
    dfgSaveDfg : function() {
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
            return;
        }        

        // clean input 4 filename
        inputFilename.val('');
        
        // load dfg 2 json object
        var jsonDfg = this.dfgGraph.toJSON();
        
        //send request
        this.model.requestSaveDfg(JSON.stringify(jsonDfg), filename, true);
    },
    
    dfgLoadDfg : function(model) {
        var dfg = $(model.target);
        var dfgFilename = dfg.html();
        this.model.requestLoadDfg(dfgFilename);
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