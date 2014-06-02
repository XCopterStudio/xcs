var DataFlowGraphDefaultModel = joint.shapes.devs.Model.extend({
    defaults: joint.util.deepSupplement({
        id: "m",
        position: { x: 50, y: 50 },
        inPorts: [],
        outPorts: [],
        size: { width: 90, height: 90 },
        attrs: {
            '.label': { text: 'Model', 'ref-x': .5, 'ref-y': .2 },
            rect: { fill: '#2ECC71' },
            '.inPorts circle': { fill: '#16A085', type: 'in' },
            '.outPorts circle': { fill: '#E74C3C', type: 'out' }
        },
    }, joint.shapes.devs.Model.prototype.defaults),
    
    setId : function(newId) {
        this.set('id', newId);
    },
    
    setLabel : function(newLabel) {
        this.attr('.label/text', newLabel); 
    },
    
    setAutoSize : function(maxCount) {
        var inCount = this.get('inPorts').length;
        var outCount = this.get('outPorts').length;
        var count = (inCount > outCount) ? inCount : outCount;
        if(!maxCount) {
            maxCount = 5;
        }
        if(count > maxCount) {
            count = maxCount;
        }
        this.get('size').height = count * 30;
    },
    
    addInputPort : function(portId) {
        var inp = this.get('inPorts').slice();
        inp.push(portId);
        this.set('inPorts', inp);
    },
    
    addOutpuPort : function(portId) {
        var outp = this.get('outPorts').slice();
        outp.push(portId);
        this.set('outPorts', outp);
    },
});
