var GaugeDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div id="gauge_<%= widgetId %>_<%= dataId %>" class="widget-gauge"></div></li>',
    
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
            title: this.attrs.name
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
        console.log("set settings: " + JSON.stringify(settings));
        
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
        console.log("validate settings: " + JSON.stringify(settings));
        
        var result = [];
        var msg;
        
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
            msg = "";
            
            switch(setting.name) {
                case "minimum value":
                    msg = this.validateNumber(setting.value);
                    break;
                case "maximum value":
                    msg = this.validateNumber(setting.value);
                    break;
            }
            
            if(msg) {
                result.push({
                    name: setting.name,
                    msg: msg,
                });
            }
        }
                    
        return result;
    },
});