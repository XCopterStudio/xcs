var NodeState = ENUM("NOTCREATED", "CREATED", "STARTED", "STOPPED");

joint.shapes.dfg = {};

joint.shapes.dfg.DataFlowGraphCloneNode = joint.shapes.basic.Generic.extend(_.extend({}, joint.shapes.basic.PortsModelInterface, {        
    markup: '<g class="rotatable"><g class="scalable"><rect></rect></g><text class="label"></text><g class="inPorts"></g><g class="outPorts"></g></g>',
    portMarkup: '<g class="port<%= id %>"><circle class="port port-tooltip" data-toggle="tooltip" data-container="body" data-trigger="hover focus" title="<%= port.title %>"></circle><text></text></g>',
    
    defaults: joint.util.deepSupplement({
        inPortsType: {},
        outPortsType: {},
        registerXVars: [],
        nodeState: NodeState.NOTCREATED,
        type: "dfg.DataFlowGraphCloneNode",
        id: "id",
        origId: "origId",
        inPorts: [],
        outPorts: [],
        size: { 
            width: 120, 
            height: 90 
        },
        attrs: {
            '.': { 
                magnet: false 
            },
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
        
        var dataType = this.get("outPortsType")[portName]
        var title;
        if(!dataType) {
            dataType = this.get("inPortsType")[portName];
        }
        if(!dataType || !dataType.semType || !dataType.synType) {
            title = "" + portName + ": ";
        }
        else {
            title = "" + portName + ": " + dataType.semType + "(" + dataType.synType + ")";
            title = title.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;')
        }
        
        attrs[portCircleSelector] = { 
            port: { 
                id: portName || _.uniqueId(type), 
                type: type,
                title: title,
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
        
        this.set("nodeState", state);
        
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
    
    getState : function() {
        return this.get("nodeState");
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
        if(this.get("registerXVars").indexOf(portId) < 0) {
            this.get("registerXVars").push(portId);
        }
        
        // add classic input port
        this.addInputPort(portId, "*", "*");
        
        // add registerXVar class to input port
        this.attr('circle[port="' + portId + '"]/class', "registerXVar");
    },
    
    addInputPort : function(portId, semT, synT) {
        var inPorts = this.get('inPorts').slice();
        inPorts.push(portId);
        this.get("inPortsType")[portId] = {semType: semT, synType: synT};
        this.set('inPorts', inPorts);
    },
    
    addOutpuPort : function(portId, semT, synT) {
        var outp = this.get('outPorts').slice();
        outp.push(portId);
        this.get("outPortsType")[portId] = {semType: semT, synType: synT};
        this.set('outPorts', outp);
    },
    
    hasOutputPort: function(port) {
        return _.contains(this.get("outPorts"), port);
    },
    
    hasInputPort: function(port) {
        return _.contains(this.get("inPorts"), port);
    },
}));

joint.shapes.dfg.DataFlowGraphCloneNodeView = joint.dia.ElementView.extend(joint.shapes.basic.PortsViewInterface);