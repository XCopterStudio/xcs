var ScriptGeneratorView = Backbone.View.extend({

    id : 'script-generator',
    
    initialize : function() {
        
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
        
        // add change listener 
        Blockly.addChangeListener(scriptWriter);  
    }
});

function scriptWriter() {
    var code = Blockly.JavaScript.workspaceToCode();
    document.getElementById('scriptScreen').value = code;
}
