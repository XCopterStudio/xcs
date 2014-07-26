var NodeState = ENUM("NOTCREATED", "CREATED", "STARTED", "STOPPED");

joint.shapes.dfg = {};

joint.shapes.dfg.DataFlowGraphDefaultModel = joint.shapes.basic.Generic.extend(_.extend({}, joint.shapes.basic.PortsModelInterface, {
    inPortsType: {},
    outPortsType: {},
    registerXVars: [],
    
    markup: '<g class="rotatable"><g class="scalable"><rect></rect></g><text class="label"></text><g class="inPorts"></g><g class="outPorts"></g></g>',
    portMarkup: '<g class="port<%= id %>"><circle class="port"></circle><text></text></g>',
        
    defaults: joint.util.deepSupplement({
        type: "dfg.DataFlowGraphDefaultModel",
        id: "id",
        origId: "origId",
        inPorts: [],
        outPorts: [],
        size: { width: 120, height: 90 },
        attrs: {
            '.': { magnet: false },
            text: {
                'fill': 'black',
                'pointer-events': 'none'
            },
            '.label': { 
                ref: "rect",
                'ref-x': 0.5, 
                'ref-y': 0.5, 
                'y-alignment': 'middle', 
                'x-alignment': 'middle'
            },
            '.inPorts text': { 
                'x':-15, 
                'dy': 4, 
                'text-anchor': 'end' 
            },
            '.outPorts text':{ 
                'x': 15, 
                'dy': 4 },
            rect: {  
                width: 120, 
                height: 90,
                stroke: 'black',
            },
            circle: {
                r: 8,
                stroke: 'black',
                magnet: true,
            },
            '.inPorts circle.port': { 
                fill: '#E74C3C',
                type: 'in',
            },
            // this attrs is not use in standard way - it is use in addRegisterXVar method
            '.inPorts circle.registerXVar': { 
                fill: '#A01507',
            },
            '.outPorts circle': { 
                fill: '#16A085', 
                type: 'out',
            }
        },
    }, joint.shapes.basic.Generic.prototype.defaults),

    getPortAttrs: function(portName, index, total, selector, type) {
        var attrs = {};   
        var portClass = 'port' + index;
        var portSelector = selector + '>.' + portClass;
        var portTextSelector = portSelector + '>text';
        var portCircleSelector = portSelector + '>circle';

        attrs[portTextSelector] = { 
            text: portName 
        };
        
        attrs[portCircleSelector] = { 
            port: { 
                id: portName || _.uniqueId(type), 
                type: type,
            }
        };
        
        attrs[portSelector] = { 
            ref: 'rect', 
            'ref-y': (index + 0.5) * (1 / total),
        };
        
        if(selector === '.outPorts') { 
            attrs[portSelector]['ref-dx'] = 0; 
        }
        
        return attrs;
    },
    
    setState : function(state) {
        var color = '#FFFFFF';
        
        switch(state) {
            case NodeState.NOTCREATED:
                color = '#5bc0de';
                break;
            case NodeState.CREATED:
                color = '#428bca';
                break;
            case NodeState.STARTED:
                color = '#5cb85c';
                break;
            case NodeState.STOPPED:
                color = '#f0ad4e';
                break;
        }

        this.attr('rect/fill', color);
    },
    
    setId : function(newId) {
        this.set('id', newId);
    },
    
    setOrigId : function(newId) {
        this.set('origId', newId);
    },
    
    setLabel : function(newLabel) {
        this.attr('.label/text', newLabel);
    },
    
    setAutoSize : function(maxCount, minCount) {
        // default value
        maxCount = typeof maxCount !== 'undefined' ? maxCount : 10;
        minCount = typeof minCount !== 'undefined' ? minCount : 1;
        
        var inCount = this.get('inPorts').length;
        var outCount = this.get('outPorts').length;
        var count = (inCount > outCount) ? inCount : outCount;
        
        // check max count
        if(count > maxCount) {
            count = maxCount;
        }
        
        // check min count
        if(count < minCount) {
            count = minCount;
        }
        
        this.get('size').height = count * 30;
    },
    
    setModel : function(model) {
        this.model = model;
    },
    
    addRegisterXVar : function(portId) {
        // mark portid like registerXVar method
        if(this.registerXVars.indexOf(portId) < 0) {
            this.registerXVars.push(portId);
        }
        
        // add classic input port
        this.addInputPort(portId, "*", "*");
        
        // add registerXVar class to input port
        this.attr('circle[port="' + portId + '"]/class', "registerXVar");

        // set special style 
        var css = this.attr('.inPorts circle.registerXVar');
        for(var key in css) {
            this.attr('circle[port="' + portId + '"]/' + key, css[key]);
        }
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
}));

joint.shapes.dfg.DataFlowGraphDefaultModelView = joint.dia.ElementView.extend(joint.shapes.basic.PortsViewInterface);