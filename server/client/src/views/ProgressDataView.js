var ProgressDataView = AbstractDataView.extend({
    id: 'progress-widget',
    
    template: '\
        <li class="widget-line">\
            <div class="text-center"><%= name %></div>\
            <br><br>\
            <div class="progress widget-progress">\
                <div class="progress-bar progress-bar-success progress-bar-striped active" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100" style="width: 0%" id="progress_<%= widgetId %>_<%= dataId %>"></div>\
            </div>\
        </li>',
    
    min: 0,
    
    max: 100,
    
    lastValue: 0,
    
    settings: [
        { name: "maximum value", type: SettingType.NUMBER, default: 100 },
    ],
    
    init: function() {
        this.$progress = $("#progress_" + this.attrs.widgetId + "_" + this.attrs.dataId);
        
        this.setProgress();
    },
    
    setProgress: function() {
        this.$progress.attr("aria-valuemin", this.min);
        this.$progress.attr("aria-valuemax", this.max);
        this.setValue(this.lastValue);
    },
        
    setData : function(data) {
        var intData = parseInt(data);
        if(!isNaN(intData)) {
            this.setValue(intData);
        }
    },
        
    setValue: function(value) {
        var percentData = (value / this.max) * 100;
        percentData = (percentData > 100 ? 100 : percentData);
            
        this.$progress.css("width", "" + percentData + "%");
        this.$progress.attr("aria-valuenow", percentData);
        this.$progress.html("" + value);
        
        this.lastValue = value;
    },
        
    setSettings: function(settings) {
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];

            switch(setting.name) {
                case "maximum value":
                    this.max = setting.value;
                    break;
            }
        }
        
        this.setProgress();
    },
    
    validateSettings: function(settings) {
        var result = [];
        var msg;
        
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
            msg = "";
            
            switch(setting.name) {
                case "maximum value":
                    msg = app.SettingView.validateNumber(setting.value);
                    
                    if(!msg && setting.value <= 0) {
                        msg = "Value must be bigger then 0. "
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
                    
        return result;
    },
});