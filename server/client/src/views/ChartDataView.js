var ChartDataView = AbstractDataView.extend({
    template: '\
    <li class="widget-line">                                                            \
        <div class="widget-chart-container">                                            \
			<div id="chart_<%= widgetId %>_<%= dataId %>" class="widget-chart"></div>   \
		</div>                                                                          \
    </li>',
    
    sizeX: 2,
    
    totalPoints: 50,
        
    data: [],
    
    plot: {},
    
    min: 0,
                                            
    max: 100,
                                                                                        
    settings: [
        { name: "minimum value", type: SettingType.NUMBER, default: 0 },
        { name: "maximum value", type: SettingType.NUMBER, default: 100 },
    ],
                                            
    init: function() {
        for(var i = 0; i < this.totalPoints; ++i) {
            this.data.push(0);
        }
            
        var plotSelector = "#chart_" + this.attrs.widgetId + "_" + this.attrs.dataId;
    
        this.setPlot();

        var tooltipId = "tooltip_" + this.attrs.widgetId + "_" + this.attrs.dataId;    
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
    
        var self = this;
        $(plotSelector).bind("plothover", function (event, pos, item) {
            if (item) {
                var y = item.datapoint[1].toFixed(0);

                $(tooltipSelector).html(self.attrs.name + ": " + y)
                    .css({top: item.pageY+5, left: item.pageX+5})
                    .fadeIn(200);
            } else {
                $(tooltipSelector).hide();
            }
		});
    },
        
    setPlot: function() {
        var plotSelector = "#chart_" + this.attrs.widgetId + "_" + this.attrs.dataId;
        
        var plotData = [];
        for(var i = 0; i < this.data.length; ++i) {
            plotData.push([i, this.data[i]]);
        }
        
        this.plot = $.plot(
            plotSelector, 
            [{data: plotData, label: this.attrs.name, color: "blue"}], {
                series: {
                    shadowSize: 0,	// Drawing is faster without shadows
                    lines: {
                        show: true,
                        fill: true,
                    },
                    points: {
                        show: false
                    }
                },
                yaxis: {
                    min: this.min,
                    max: this.max
                },
                xaxis: {
                    show: false
                },
                grid: {
                    hoverable: true,
                    clickable: true,
                    autoHighlight: false
                },
        });
    },
    
    addValue: function(value) {
        if(this.data.length >= this.totalPoints) {
            this.data = this.data.slice(1);
        }
        
        this.data.push(value);
        
        var plotData = [];
        for(var i = 0; i < this.data.length; ++i) {
            plotData.push([i, this.data[i]]);
        }
        
        this.plot.setData([{data: plotData, color: "blue"}]);
        this.plot.draw();
    },
        
    setData: function(data) {
        var intData = parseInt(data);
        if(!isNaN(intData)) {            
            this.addValue(intData);    
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
        
        this.setPlot();
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