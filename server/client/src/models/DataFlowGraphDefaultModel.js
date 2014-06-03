var DataFlowGraphDefaultModel = joint.shapes.devs.Model.extend({
    inPortsType: {},
    outPortsType: {},
    
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
    
    setModel : function(model) {
        this.model = model;
    },
    
    addInputPort : function(portId, semT, synT) {
        var inPorts = this.get('inPorts').slice();
        inPorts.push(portId);
        this.inPortsType[portId] = {semType: semT, synType: synT};
        this.set('inPorts', inPorts);
    },
    
    addOutpuPort : function(portId, semT, synT) {
        var outp = this.get('outPorts').slice();
        outp.push(portId);
        this.outPortsType[portId] = {semType: semT, synType: synT};
        this.set('outPorts', outp);
    },
});
