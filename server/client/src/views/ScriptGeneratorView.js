var scriptGeneratorGraph = {};

var scriptGeneratorModels =  {
    addModel: function(id, model) {
        this[id] = model;
        
        $('g[model-id="' + id + '"]').dblclick(function(eventObject){
            scriptGeneratorModels[this.getAttribute('model-id')].remove();
        }); 
    }
};

var ScriptGeneratorView = Backbone.View.extend({

    id : 'script-generator',
    
    el : '#dfg',
    
    events : {
        "click #dfgLoad" : "dfgLoad",
        "click #dfgCreate" : "dfgCreate",
        "click #dfgStart" : "dfgStart",
        "click #dfgStop" : "dfgStop",
        "click #dfgReset" : "dfgReset",
    },
    
    dfgToolboxNodes : {},
    
    initialize : function() {
        this.model = new DataFlowGraph();
        
        this.onInputChange(app.XcopterState);      
        this.listenTo(app.XcopterState, "change:onboard", this.onInputChange);
        
        this.listenTo(this.model, "change:dataFlowGraph", this.onDataFlowGraphChange);
        this.listenTo(this.model.get("xprototype"), "add", this.onPrototypeAdd);
        this.listenTo(this.model.get("xclone"), "add", this.onCloneAdd);
        this.listenTo(this.model.get("xprototype"), "remove", this.onPrototypeRemove);
        this.listenTo(this.model.get("xclone"), "remove", this.onCloneRemove);
        
        
        // NOTE: 4 debug only
//        var prot = new DataFlowGraphNode();
//        prot.set("name", "XXci");
//        prot.get("xvar").add(new DataFlowGraphNodeIO({
//            name : "fly",
//            synType : "xcs::nodes::xci::FlyParam",
//            semType : "FLY_PARAM"
//        }));
//        prot.get("xinputPort").add(new DataFlowGraphNodeIO({
//            name : "command",
//            synType : "std::string",
//            semType : "COMMAND"
//        }));
//        
//        //var c = new DataFlowGraphNode({"name" : "dodo"});
//        var c = prot.clone();
//        c.set("name", "dodo");
//        
//        this.model.get("xprototype").add(prot);
//        this.model.get("xclone").add(c);
//        //this.model.set('dataFlowGraph', '<xml></xml>');
//        
//        prot.set("name", "XXCI");
//        
//        c.get("xvar").add(new DataFlowGraphNodeIO({
//            name : "gaz",
//            synType : "double",
//            semType : "GAZ"
//        }));
        
//        this.model.set('dataFlowGraph', '<xml><text>hello world</text></xml>');        
        
        this.initializeDfg();
        
        //this.initializeBlockly();
    },
    
    
    /****************************
    ** DATA FLOW GRAPH SECTION **
    ****************************/
    initializeDfg : function() {
        var flowGraphConsole = $('#flow-graph-console');
        flowGraphConsole.append('<textarea id="flow-graph-txt" rows="15" cols="150"></textarea>');
        
        scriptGeneratorGraph = new joint.dia.Graph;
        this.listenTo(scriptGeneratorGraph, 'change:target', this.setLink);
        this.listenTo(scriptGeneratorGraph, 'change:source', this.setLink);

        scriptGeneratorGraph.on('change', function(model) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });
        scriptGeneratorGraph.on('add', function(cell) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });
        scriptGeneratorGraph.on('remove', function(cell) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });

        this.initializeDfgToolbox4Drop();
           
        var paper = new joint.dia.Paper({
            el: $('#flow-graph-screen'),
            width: 1024, 
            height: 500, 
            gridSize: 10,   // size of grid in px (how many px reprezents one cell)
            model: scriptGeneratorGraph,
            validateConnection: function(cellViewS, magnetS, cellViewT, magnetT, end, linkView) {
                if(magnetS && magnetT && magnetS.getAttribute("type") == "out" && magnetT.getAttribute("type") == "in") {
                    var idS = magnetS.getAttribute("port");
                    var idT = magnetT.getAttribute("port");

                    var sameSemT = cellViewS.model.outPortsType[idS].semType == cellViewT.model.inPortsType[idT].semType;
                    var sameSynT = cellViewS.model.outPortsType[idS].synType == cellViewT.model.inPortsType[idT].synType;
                    
                    var valid = sameSemT && sameSynT;
                    
                    return valid;
                }
                
                return false;
            },
            validateMagnet: function(cellView, magnet) {
                return magnet.getAttribute('type') != "in";
            },
            snapLinks: { radius: 45 },
        });
        
        this.model.requestData();
    },
    
    addNode2FgToolbox : function(id, title) {
        if(!title) {
            title = id;
        }
        
//        var flowGraphToolbox = $('#flow-graph-toolbox');
//        flowGraphToolbox.append('<div id="' + id + '" class="drag_init">' + title + '</div>');
        
        var flowGraphToolbox = $('#flow-graph-toolbox');
        flowGraphToolbox.append('\
            <div class="panel panel-default">         \
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
        //Counter
        var counter = [];
        var self = this;
        
        //Make element droppable
        $("#flow-graph-screen").droppable({
            drop: function (ev, ui) {
                var toolId = ui.draggable.attr('id');
                
                if(!counter[toolId]) {
                    counter[toolId] = 1;
                }
                else {
                    ++counter[toolId];
                }
                
                var pos = $(ui.helper).offset();
                var containerPos = $("#flow-graph-screen").offset();         
                
                // get model
                var modelPrototype = self.dfgToolboxNodes[toolId];
                
                // get xvars and xinput ports
                var xvars = modelPrototype.get("xvar");
                var xinputPorts = modelPrototype.get("xinputPort");
                
                //get name
                var prototypeName;
                
                var modelId;
                if(counter[toolId] == 1) {
                    prototypeName = modelPrototype.get("name");
                    modelId = toolId;   
                }
                else {
                    prototypeName = modelPrototype.get("name") + " " + counter[toolId];
                    modelId = toolId + counter[toolId];
                }
                
                var m = new DataFlowGraphDefaultModel({
                    position: { x: pos.left - containerPos.left, y: pos.top - containerPos.top },
                });
                m.setId(modelId);
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
                
                scriptGeneratorGraph.addCell(m);
                scriptGeneratorModels.addModel(modelId, m);
            }
        });
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
    
    onDataFlowGraphChange : function(model) {
        var graph = model.get("dataFlowGraph");
        
        //DEBUG
        console.log("onDataFlowGraphChange: " + JSON.stringify(graph));
        
        //TODO: change default graph
        //...
        //this.loadScript(graph);
    },
    
    onPrototypeAdd : function(modelPrototype) {
        //DEBUG
        //console.log("onPrototypeAdd " + JSON.stringify(modelPrototype.toJSON()));
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        
        if(this.dfgToolboxNodes[prototypeName]) {
            console.error("ERROR(onPrototypeAdd): " + prototypeName + " was already loaded!");
            return;
        }
        
        // add 2 toolbox - show to user
        this.addNode2FgToolbox(prototypeName);
        this.dfgToolboxNodes[prototypeName] = modelPrototype;
        console.log("added: " + prototypeName + " = " + this.dfgToolboxNodes[prototypeName].get("name"));
        
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
        console.log("onPrototypeRemove " + JSON.stringify(modelPrototype.toJSON()));
        
        //TODO: remove prototype from GUI
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
        
        // add 2 toolbox - show to user
        if(!this.dfgToolboxNodes[prototypeName]) {
            console.error("ERROR(onPrototypeChange): " + prototypeName + " was not yet loaded!");
            return;
        }
        
        // stop listening to old model
        this.stopListening(this.dfgToolboxNodes[prototypeName]);
        
        // remove old model and add new one
        this.dfgToolboxNodes[prototypeName] = modelPrototype;
        
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
        console.log('dfgLoad');
        //TODO: load dfg info
    },
    
    dfgCreate : function() {
        console.log('dfgCreate');
        //TODO: send request to create dfg
    },
    
    dfgStart : function() {
        console.log('dfgStart');
        //TODO: send request to start created dfg
    },
    
    dfgStop : function() {
        console.log('dfgStop');
        //TODO: send request to stop started dfg
    },
    
    dfgReset : function() {
        console.log('dfgReset');
        //TODO: send request to reset dfg and clear GUI
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