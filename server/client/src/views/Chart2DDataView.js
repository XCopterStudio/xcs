var Chart2DDataView = AbstractDataView.extend({
    id: 'chart-widget-2d',
    
    template: '\
    <li class="widget-line">\
        <div class="text-center"><%= name %></div>\
        <div class="widget-chart-container">                                            \
			<div id="chart2d_<%= widgetId %>_<%= dataId %>" class="widget-chart"></div>   \
		</div>                                                                          \
    </li>',
    
    sizeX: 2,
    sizeY: 3,
    
    totalPoints: 100,
        
    data: [],
    
    plot: {},
    
    xMin: 0,
                                            
    xMax: 100,
    
    yMin: 0,
                                            
    yMax: 100,
                                              
    xToken: "x",
                                              
    yToken: "y",
                                              
    settings: [
        { name: "X minimum value", type: SettingType.NUMBER, default: 0 },
        { name: "X maximum value", type: SettingType.NUMBER, default: 100 },
        { name: "Y minimum value", type: SettingType.NUMBER, default: 0 },
        { name: "Y maximum value", type: SettingType.NUMBER, default: 100 },
        { name: "total points", type: SettingType.NUMBER, default: 100 },
        { name: "x axis", type: SettingType.TEXT, default: "x" },
        { name: "y axis", type: SettingType.TEXT, default: "y" },
    ],
                                            
    init: function() {
        this.data = [];        
        this.data.push([0,0]);
            
        var plotSelector = "#chart2d_" + this.attrs.widgetId + "_" + this.attrs.dataId;
    
        this.setPlot();

        var tooltipId = "tooltip2d_" + this.attrs.widgetId + "_" + this.attrs.dataId;    
        var tooltipSelector = "#" + tooltipId;   
    
        $("<div id='"+tooltipId+"'></div>").css({
			position: "absolute",
			display: "none",
			border: "1px solid #fdd",
			padding: "2px",
			"background-color": "#fee",
			opacity: 0.80,
            "z-index": 998
		}).appendTo("body");
    
        this.$tooltip = $(tooltipSelector);
    
        var self = this;
        $(plotSelector).bind("plothover", function (event, pos, item) {
            if (item) {
                var y = item.datapoint[1];
                var x = item.datapoint[0];

                self.$tooltip.html(self.attrs.name + ": [" + x + "," + y + "]")
                    .css({top: item.pageY+5, left: item.pageX+5})
                    .fadeIn(200);
            } else {
                self.$tooltip.hide();
            }
		});
    },
        
    removeCustom: function() {
        this.$tooltip.remove();
    },
        
    setPlot: function() {
        var plotSelector = "#chart2d_" + this.attrs.widgetId + "_" + this.attrs.dataId;
        
        var plotData = [];
        for(var i = 0; i < this.data.length; ++i) {
            plotData.push([this.data[i][0], this.data[i][1]]);
        }
        
        this.plot = $.plot(
            plotSelector, 
            [{data: plotData, color: "blue"}], {
                series: {
                    shadowSize: 0,	// Drawing is faster without shadows
                    lines: {
                        show: true,
                        //fill: true,
                    },
                    points: {
                        show: false
                    }
                },
                yaxis: {
                    min: this.yMin,
                    max: this.yMax
                },
                xaxis: {
                    min: this.xMin,
                    max: this.xMax
                },
                grid: {
                    hoverable: true,
                    clickable: true,
                    autoHighlight: false
                },
        });
    },
    
    addValue: function(xValue, yValue) {
        if(this.data.length >= this.totalPoints) {
            this.data = this.data.slice(1);
        }
        
        this.data.push([xValue, yValue]);
        
        var plotData = [];
        for(var i = 0; i < this.data.length; ++i) {
            plotData.push([this.data[i][0], this.data[i][1]]);
        }
        
        this.plot.setData([{data: plotData, color: "blue"}]);
        this.plot.draw();
    },
        
    setData: function(data) {
        if(data !== undefined && data[this.xToken] !== undefined && data[this.yToken] !== undefined) {
            var xNumber = Number(data[this.xToken]);
            var yNumber = Number(data[this.yToken]);
        
            if(!isNaN(xNumber) && !isNaN(yNumber)) {      
                this.addValue(xNumber, yNumber);    
            }
        }
    },
        
    setSettings: function(settings) {
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];

            switch(setting.name) {
                case "X minimum value":
                    this.xMin = setting.value;
                    break;
                case "X maximum value":
                    this.xMax = setting.value;
                    break;
                case "Y minimum value":
                    this.yMin = setting.value;
                    break;
                case "Y maximum value":
                    this.yMax = setting.value;
                    break;
                case "total points":
                    this.totalPoints = setting.value;
                    break;
                case "x axis":
                    this.xToken = setting.value;
                    break;
                case "y axis":
                    this.yToken = setting.value;
                    break;
            }
        }
        
        this.setPlot();
    },
    
    validateSettings: function(settings) {
        var result = [];
        var msg;
        
        var xMin;
        var xMax;
        var yMin;
        var yMax;
        
        for(var i = 0; i < settings.length; ++i) {
            var setting = settings[i];
            msg = "";
            
            switch(setting.name) {
                case "X minimum value":
                    msg = app.SettingView.validateNumber(setting.value);
                    if(!msg) {
                        xMin = Number(setting.value);
                    }
                    break;
                case "X maximum value":
                    msg = app.SettingView.validateNumber(setting.value);
                    if(!msg) {
                        xMax = Number(setting.value);
                    }
                    break;
                case "Y minimum value":
                    msg = app.SettingView.validateNumber(setting.value);
                    if(!msg) {
                        yMin = Number(setting.value);
                    }
                    break;
                case "Y maximum value":
                    msg = app.SettingView.validateNumber(setting.value);
                    if(!msg) {
                        yMax = Number(setting.value);
                    }
                    break;
                case "total points":
                    msg = app.SettingView.validateNumber(setting.value);
                    if(!msg && setting.value <= 0) {
                        msg = "Value must be bigger then 0. "
                    }
                    break;
                case "x axis":
                    msg = app.SettingView.validateEmpty(setting.value);
                    break;
                case "y axis":
                    msg = app.SettingView.validateEmpty(setting.value);
                    break;
            }
            
            if(msg) {
                result.push({
                    name: setting.name,
                    msg: msg,
                });
            }
        }
        
        if(xMin !== undefined && xMax !== undefined && xMin >= xMax) {
            result.push({
                name: "X minimum value",
                msg: "Minimum must be smaller then maximum. ",
            });
            result.push({
                name: "X maximum value",
                msg: "Maximum must be bigger then minimum. ",
            });
        }
        
        if(yMin !== undefined && yMax !== undefined && yMin >= yMax) {
            result.push({
                name: "Y minimum value",
                msg: "Minimum must be smaller then maximum. ",
            });
            result.push({
                name: "Y maximum value",
                msg: "Maximum must be bigger then minimum. ",
            });
        }
        
        return result;
    },
});