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
    
    dfgToolboxNodes : {},
    
    initialize : function() {
        this.model = new DataFlowGraph();
        
        this.onInputChange(app.XcopterState);      
        this.listenTo(app.XcopterState, "change:onboard", this.onInputChange);
        
        this.listenTo(this.model, "change:dataFlowGraph", this.onDataFlowGraphChange);
        this.listenTo(this.model.get("xprototype"), "add", this.onPrototypeAdd);
        this.listenTo(this.model.get("xclone"), "add", this.onCloneAdd);
        
        
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
        
        scriptGeneratoGraph = new joint.dia.Graph;
        this.listenTo(scriptGeneratoGraph, 'change:target', this.setLink);

        scriptGeneratoGraph.on('change', function(model) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });
        scriptGeneratoGraph.on('add', function(cell) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });
        scriptGeneratoGraph.on('remove', function(cell) {
            $('#flow-graph-txt').val(JSON.stringify(this.toJSON()));
        });

        this.initializeDfgToolbox4Drop();
        
        //debug: init toolbox
//        this.addNode2FgToolbox("XXci parrot", "XXci - parrot");
//        this.addNode2FgToolbox("XXci dodo", "XXci - dodo");
        
        var paper = new joint.dia.Paper({
            el: $('#flow-graph-screen'),
            width: 650, height: 200, gridSize: 1,
            model: scriptGeneratoGraph,
            validateConnection: function(cellViewS, magnetS, cellViewT, magnetT, end, linkView) {
                var valid = magnetT.getAttribute('type') != 'out';
                return valid;
            },
            validateMagnet: function(cellView, magnet) {
                return magnet.getAttribute('type') != "in";
            },
            snapLinks: { radius: 45 }
        });
        
        // 4 debug only
//        var m1 = new DataFlowGraphDefaultModel();
//        m1.setId('m1');
//        m1.setLabel('Model 1');
//        m1.setAutoSize();
//        scriptGeneratoGraph.addCell(m1);
//        
//        var m2 = new DataFlowGraphDefaultModel({
//            inPorts: ['in1','in2', 'in3', 'in4', 'in5', 'in6'],
//        });
//        m2.translate(300, 0);
//        m2.setId("m2");
//        m2.setLabel('Model 2');      
//        m2.setAutoSize();
//        m2.addInputPort('in7');
//        m2.addOutpuPort('out2');
//        scriptGeneratoGraph.addCell(m2);
//        scriptGeneratorModels.addModel("m1", m1);
//        scriptGeneratorModels.addModel("m2", m2);
        
//        var newContent = JSON.parse('{"cells":[{"type":"devs.Model","size":{"width":90,"height":90},"inPorts":["in1","in2"],"outPorts":["out"],"position":{"x":50,"y":50},"angle":0,"id":"14515f13-24ab-4c10-b67a-0e9b55a526a7","z":0,"attrs":{"rect":{"fill":"#2ECC71"},".label":{"ref-x":0.5},".inPorts circle":{"fill":"#16A085","type":"in"},".outPorts circle":{"fill":"#E74C3C","type":"out"},".inPorts>.port0>text":{"text":"in1"},".inPorts>.port0>circle":{"port":{"id":"in1","type":"in"}},".inPorts>.port0":{"ref":"rect","ref-y":0.25},".inPorts>.port1>text":{"text":"in2"},".inPorts>.port1>circle":{"port":{"id":"in2","type":"in"}},".inPorts>.port1":{"ref":"rect","ref-y":0.75},".outPorts>.port0>text":{"text":"out"},".outPorts>.port0>circle":{"port":{"id":"out","type":"out"}},".outPorts>.port0":{"ref":"rect","ref-y":0.5,"ref-dx":0}}},{"type":"devs.Model","size":{"width":90,"height":90},"inPorts":["in1","in2"],"outPorts":["out"],"position":{"x":350,"y":50},"angle":0,"id":"cacddf4b-9244-4624-b1af-30355f4df78b","z":0,"embeds":"","attrs":{"rect":{"fill":"#2ECC71"},".label":{"text":"Model 2","ref-x":0.5},".inPorts circle":{"fill":"#16A085","type":"in"},".outPorts circle":{"fill":"#E74C3C","type":"out"},".inPorts>.port0>text":{"text":"in1"},".inPorts>.port0>circle":{"port":{"id":"in1","type":"in"}},".inPorts>.port0":{"ref":"rect","ref-y":0.25},".inPorts>.port1>text":{"text":"in2"},".inPorts>.port1>circle":{"port":{"id":"in2","type":"in"}},".inPorts>.port1":{"ref":"rect","ref-y":0.75},".outPorts>.port0>text":{"text":"out"},".outPorts>.port0>circle":{"port":{"id":"out","type":"out"}},".outPorts>.port0":{"ref":"rect","ref-y":0.5,"ref-dx":0}}},{"type":"link","id":"855fec63-9210-44a9-92d3-b9d1d4814406","embeds":"","source":{"id":"14515f13-24ab-4c10-b67a-0e9b55a526a7","selector":"g:nth-child(1) g:nth-child(4) g:nth-child(1) circle:nth-child(1)     ","port":"out"},"target":{"id":"cacddf4b-9244-4624-b1af-30355f4df78b","selector":"g:nth-child(1) g:nth-child(3) g:nth-child(1) circle:nth-child(1)     ","port":"in1"},"z":2,"vertices":[{"x":226,"y":43}],"attrs":{".connection":{"stroke":"green"},".marker-target":{"fill":"green","d":"M 10 0 L 0 5 L 10 10 z"}}},{"type":"link","id":"2fafc239-87ca-4b09-9c09-08a62b606976","embeds":"","source":{"id":"14515f13-24ab-4c10-b67a-0e9b55a526a7","selector":"g:nth-child(1) g:nth-child(4) g:nth-child(1) circle:nth-child(1)     ","port":"out"},"target":{"x":261,"y":131},"z":3,"attrs":{".connection":{"stroke":"red"},".marker-target":{"fill":"red","d":"M 10 0 L 0 5 L 10 10 z"}}},{"type":"link","id":"ed391108-0a1e-4723-a4bc-9d2341d684c6","embeds":"","source":{"id":"cacddf4b-9244-4624-b1af-30355f4df78b","selector":"g:nth-child(1) g:nth-child(4) g:nth-child(1) circle:nth-child(1)     ","port":"out"},"target":{"id":"cacddf4b-9244-4624-b1af-30355f4df78b","selector":"g:nth-child(1) g:nth-child(3) g:nth-child(2) circle:nth-child(1)     ","port":"in2"},"z":4,"attrs":{".connection":{"stroke":"green"},".marker-target":{"fill":"green","d":"M 10 0 L 0 5 L 10 10 z"}}}]}');
//        scriptGeneratoGraph.fromJSON(newContent);    
    },
    
    addNode2FgToolbox : function(id, title) {
        if(!title) {
            title = id;
        }
        
        var flowGraphToolbox = $('#flow-graph-toolbox');
        flowGraphToolbox.append('<div id="' + id + '" class="drag_init">' + title + '</div>');
        this.initializeDfgToolbox4Drag();
    },
    
    initializeDfgToolbox4Drag : function() {
        //Make element draggable
        var dragInit = $(".drag_init");
        dragInit.draggable({
            helper: 'clone',
            containment: '#flow-graph',
            //stop: function (ev, ui) { }
        });
        
        // remove drag_init class and add just drug_
        dragInit.removeClass("drag_init").addClass("drag_");
    },
    
    initializeDfgToolbox4Drop : function() {
        //Counter
        counter = 0;
        var self = this;
        //Make element droppable
        $("#flow-graph-screen").droppable({
            drop: function (ev, ui) {
                counter++;
                
                var toolId = ui.draggable.attr('id');
                var modelId = toolId + counter;
                var pos = $(ui.helper).offset();
                var containerPos = $("#flow-graph-screen").offset();
                
                
                //TODO: create appropriate model
                var modelPrototype = self.dfgToolboxNodes[toolId];
                var xvars = modelPrototype.get("xvar");
                var xinputPorts = modelPrototype.get("xinputPorts");
                if((!xvars || xvars.length == 0) && (!xinputPorts || xinputPorts.length ==0)) {
                    console.log("not defined");
                }
                
                //get prototype name
                //var prototypeName = modelPrototype.get("name");
                //console.log(".prototypeName: " + JSON.stringify(prototypeName));
                
                // get all xvars        
//                console.log(".xvars:");
//                modelPrototype.get("xvar").forEach(function(xvar) {
//                    var xvarName = xvar.get("name");
//                    var xvarSynType = xvar.get("synType");
//                    var xvarSemType = xvar.get("semType");
//                    console.log("..xvarName: " + JSON.stringify(xvarName));
//                    console.log("..xvarSynType: " + JSON.stringify(xvarSynType));
//                    console.log("..xvarSemType: " + JSON.stringify(xvarSemType));
//                });
//                
//                // get all xinputports
//                console.log(".xinputPorts:");
//                modelPrototype.get("xinputPort").forEach(function(xvar) {
//                    var xinputPortName = xvar.get("name");
//                    var xinputPortSynType = xvar.get("synType");
//                    var xinputPortSemType = xvar.get("semType");
//                    console.log("..xinputPortName: " + JSON.stringify(xinputPortName));
//                    console.log("..xinputPortSynType: " + JSON.stringify(xinputPortSynType));
//                    console.log("..xinputPortSemType: " + JSON.stringify(xinputPortSemType));
//                });
//                
//                console.log(".end");
                
                var m = new DataFlowGraphDefaultModel({
                    position: { x: pos.left - containerPos.left, y: pos.top - containerPos.top },
                });
                m.setId(modelId);
                m.setLabel(modelId);
                m.setAutoSize();
                
                scriptGeneratoGraph.addCell(m);
                scriptGeneratorModels.addModel(modelId, m);
            }
        });
    },
    
    setLink : function(link) {      
        if(link.attributes.target.x){
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
        console.log("onPrototypeAdd " + JSON.stringify(modelPrototype.toJSON()));
        
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
    
    onPrototypeChange : function(modelPrototype) {
        //DEBUG
        console.log("onPrototypeChange " + JSON.stringify(modelPrototype.toJSON()));
        
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