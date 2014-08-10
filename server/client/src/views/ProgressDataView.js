var ProgressDataView = AbstractDataView.extend({
    template: '<li class="widget-line"><div><%= name %></div><div class="progress"><div class="progress-bar progress-bar-success progress-bar-striped active" role="progressbar" aria-valuenow="40" aria-valuemin="0" aria-valuemax="100" style="width: 0%" id="progress_<%= widgetId %>_<%= dataId %>"></div></div></li>',
    
    init: function() {
        this.$progress = $("#progress_" + this.attrs.widgetId + "_" + this.attrs.dataId);
    },
    
    setData : function(data) {
        var intData = parseInt(data);
        if(!isNaN(intData)) {
            var percentData = (intData > 100 ? 100 : intData);
            
            this.$progress.css("width", "" + percentData + "%");
            this.$progress.attr("aria-valuenow", percentData);
            this.$progress.html("" + intData + "%");
        }
    },
});