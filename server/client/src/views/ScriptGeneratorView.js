var ScriptGeneratorView = Backbone.View.extend({

    id : 'script-generator',
    
    initialize : function() {
        this.model = new DataFlowGraph();
        this.onInputChange(app.XcopterState);      
        
        this.listenTo(app.XcopterState, "change:data", this.onInputChange);
        
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
//        this.model.set('dataFlowGraph', '<xml></xml>');
//        
//        prot.set("name", "XXCI");
//        
//        c.get("xvar").add(new DataFlowGraphNodeIO({
//            name : "gaz",
//            synType : "double",
//            semType : "GAZ"
//        }));
//        
//        this.model.set('dataFlowGraph', '<xml><text>hello world</text></xml>');        
        
        
        registerBlocks();
        
        // generate toolbox
        var toolbox = $("#toolbox");
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
            document.getElementById('screen'),
            {
                  path : './js/blockly/'
                , toolbox : document.getElementById('toolbox')
                , scrollbars : false //If false, supress scrollbars that appear if the toolbox has categories. Defaults to true.
                //, collapse : true //Allows blocks to be collapsed or expanded. Defaults to true if the toolbox has categories, false otherwis
                //, maxBlocks : Infinity //Maximum number of blocks that may be created. Useful for student exercises. Defaults to Infinity.
                //, readOnly : false //If true, prevent the user from editing. Supresses the toolbox and trashcan. Defaults to false.
                //, rtl : false //If true, mirror the editor for Arabic or Hebrew locales. See RTL demo. Defaults to false.
                //, trashcan : true // Displays or hides the trashcan. Defaults to true if the toolbox has categories, false otherwise.

            });
        
        //window.setTimeout(loadScript, 0);
        loadScript();
        
        // add change listener 
        Blockly.addChangeListener(scriptWriter);  
    },
    
    
    onDataFlowGraphChange : function(model) {
        var graph = model.get("dataFlowGraph");
        
        //DEBUG
        console.log("onDataFlowGraphChange: " + JSON.stringify(graph));
        
        //TODO: change default graph
        //...
    },
    
    onPrototypeAdd : function(modelPrototype) {
        //DEBUG
        console.log("onPrototypeAdd " + JSON.stringify(modelPrototype.toJSON()));
        
        //watch 4 changes
        this.listenTo(modelPrototype, "change", this.onPrototypeChange);
        
        // TODO: add new Prototype 2 toolbox
        //...
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
//        console.log(".prototypeName: " + JSON.stringify(prototypeName));
        
        // get all xvars
        modelPrototype.get("xvar").forEach(function(xvar) {
            var xvarName = xvar.get("name");
            var xvarSynType = xvar.get("synType");
            var xvarSemType = xvar.get("semType");
//            console.log("..xvarName: " + JSON.stringify(xvarName));
//            console.log("..xvarSynType: " + JSON.stringify(xvarSynType));
//            console.log("..xvarSemType: " + JSON.stringify(xvarSemType));
        });
        
        // get all xinputports
        modelPrototype.get("xinputPort").forEach(function(xvar) {
            var xinputPortName = xvar.get("name");
            var xinputPortSynType = xvar.get("synType");
            var xinputPortSemType = xvar.get("semType");
//            console.log("..xinputPortName: " + JSON.stringify(xinputPortName));
//            console.log("..xinputPortSynType: " + JSON.stringify(xinputPortSynType));
//            console.log("..xinputPortSemType: " + JSON.stringify(xinputPortSemType));
        });
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
        
        //TODO: change existed prototype
        //...
        
        // get prototype name
        var prototypeName = modelPrototype.get("name");
        
        // get all xvars
        modelPrototype.get("xvar").forEach(function(xvar) {
            var xvarName = xvar.get("name");
            var xvarSynType = xvar.get("synType");
            var xvarSemType = xvar.get("semType");
        });
        
        // get all xinputports
        modelPrototype.get("xinputPort").forEach(function(xvar) {
            var xinputPortName = xvar.get("name");
            var xinputPortSynType = xvar.get("synType");
            var xinputPortSemType = xvar.get("semType");
        });
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
        var data = xcState.get("data");
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
    }
});

function scriptWriter() {
    //var xml = Blockly.Xml.workspaceToDom(Blockly.getMainWorkspace());
    //var code = Blockly.Xml.domToText(xml)
    var code = Blockly.JavaScript.workspaceToCode();
    document.getElementById('scriptScreen').value = code;
};

function loadScript() {
    var savedXml = Blockly.Xml.textToDom('<xml><block type="connect" id="2" x="148" y="385"><field name="XOB1">subject</field><field name="XVAR1">out</field><field name="XOB2">observer</field><field name="XVAR2">in</field></block></xml>');
    Blockly.Xml.domToWorkspace(Blockly.getMainWorkspace(), savedXml);
};

function registerBlocks() {
  Blockly.Blocks['test'] = {
      init: function() {
        this.setHelpUrl('');
        this.appendDummyInput()
            .appendField("test");
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
}