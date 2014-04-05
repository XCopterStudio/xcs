'use strict';

Blockly.Blocks['connect'] = {
  init: function() {
    this.setHelpUrl('');
    this.appendDummyInput()
        .appendField("connect");
    this.appendDummyInput()
        .appendField("   xobject")
        .appendField(new Blockly.FieldTextInput(""), "XOB1")
        .appendField("xvar")
        .appendField(new Blockly.FieldTextInput(""), "XVAR1");
    this.appendDummyInput()
        .appendField("   xobject")
        .appendField(new Blockly.FieldTextInput(""), "XOB2")
        .appendField("xinputport")
        .appendField(new Blockly.FieldTextInput(""), "XVAR2");
    this.setPreviousStatement(true);
    this.setNextStatement(true);
    this.setTooltip('');
  }
};

Blockly.JavaScript['connect'] = function(block) {
    var xob2 = block.getFieldValue('XOB2');
    var xvar2 = block.getFieldValue('XVAR2');
    var xob1 = block.getFieldValue('XOB1');
    var xvar1 = block.getFieldValue('XVAR1');
    
    var code = 'connect(' + 
        xob1 + ', ' + 
        xob2 + ', ' + 
        xob1 + '.&' + xvar1 + ', ' + 
        xob2 + '.&' + xvar2 + ', ' + 
        '"' + xvar1 + '", ' +
        '"' + xvar2 + '"' +
    ');\n';
    
    return code;
};