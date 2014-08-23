var GaugeDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div class="text-center"><%= name %></div><div id="gauge_<%= widgetId %>_<%= dataId %>" class="widget-gauge"></div></li>',
    
    min: 0,
    
    max: 100,
    
    lastValue: 0,
    
    settings: [
        { name: "minimum value", type: SettingType.NUMBER, default: 0 },
        { name: "maximum value", type: SettingType.NUMBER, default: 100 },
    ],
    
    init: function() {
        this.setGauge();
    },
    
    setGauge: function() {
        //clear container
        $('#gauge_' + this.attrs.widgetId + '_' + this.attrs.dataId).html("");
        
        //create gauge
        this.gauge = new JustGage({
            id: "gauge_" + this.attrs.widgetId + "_" + this.attrs.dataId, 
            value: this.lastValue, 
            min: this.min,
            max: this.max,
            title: " ",
        });
    },
        
    setData: function(data) {
        var intData = parseInt(data);
        if(!isNaN(intData)) {     
            this.lastValue = intData;
            this.gauge.refresh(intData);
        }
    },
        
    setSettings: function(settings) {
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];

            switch(setting.name) {
                case "minimum value":
                    this.min = setting.value;
                    break;
                case "maximum value":
                    this.max = setting.value;
                    break;
            }
        }
        
        this.setGauge();
    },
    
    validateSettings: function(settings) {
        var result = [];
        var msg;
        
        var min;
        var max;
        
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
            msg = "";
            
            switch(setting.name) {
                case "minimum value":
                    msg = this.validateNumber(setting.value);
                    if(!msg) {
                        min = Number(setting.value);
                    }
                    break;
                case "maximum value":
                    msg = this.validateNumber(setting.value);
                    if(!msg) {
                        max = Number(setting.value);
                    }
                    break;
            }
            
            if(msg) {
                result.push({
                    name: setting.name,
                    msg: msg,
                });
            }
        }
        
        if(min !== undefined && max !== undefined && min >= max) {
            result.push({
                name: "minimum value",
                msg: "Minimum must be smaller then maximum. ",
            });
            result.push({
                name: "maximum value",
                msg: "Maximum must be bigger then minimum. ",
            });
        }
                    
        return result;
    },
});