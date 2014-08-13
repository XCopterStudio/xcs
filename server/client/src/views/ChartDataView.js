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
    
    init: function() {
        var plotData = [];
        for(var i = 0; i < this.totalPoints; ++i) {
            this.data.push(0);
            plotData.push([i, 0]);
        }
            
        var plotSelector = "#chart_" + this.attrs.widgetId + "_" + this.attrs.dataId;
    
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
                    min: 0,
                    max: 100
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
});