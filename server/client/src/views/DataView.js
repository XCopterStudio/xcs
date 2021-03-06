var DataView = Backbone.View.extend({

    id: 'data-from-onboard',
    
    views: {
        raw: {
            name: "raw data",
            ctor: function(prototypeDataId, dataId) {
                var index = dataId.indexOf("__");
                if(index > 0) {
                    var nodeName = dataId.substring(0, dataId.indexOf("__"))
                    var portName = dataId.substring(dataId.indexOf("__") + 2)
                
                    nodeName = nodeName.charAt(0).toUpperCase() + nodeName.slice(1);
                    
                    return new RawDataView(prototypeDataId, dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        gauge: {
            name: "gauge data",
            ctor: function(prototypeDataId, dataId) {
                var index = dataId.indexOf("__");
                if(index > 0) {
                    var nodeName = dataId.substring(0, dataId.indexOf("__"))
                    var portName = dataId.substring(dataId.indexOf("__") + 2)
                
                    nodeName = nodeName.charAt(0).toUpperCase() + nodeName.slice(1);
                    
                    return new GaugeDataView(prototypeDataId, dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        progress: {
            name: "progress data",
            ctor: function(prototypeDataId, dataId) {
                var index = dataId.indexOf("__");
                if(index > 0) {
                    var nodeName = dataId.substring(0, dataId.indexOf("__"))
                    var portName = dataId.substring(dataId.indexOf("__") + 2)
                
                    nodeName = nodeName.charAt(0).toUpperCase() + nodeName.slice(1);
                    
                    return new ProgressDataView(prototypeDataId, dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        chart: {
            name: "chart data",
            ctor: function(prototypeDataId, dataId) {
                var index = dataId.indexOf("__");
                if(index > 0) {
                    var nodeName = dataId.substring(0, dataId.indexOf("__"))
                    var portName = dataId.substring(dataId.indexOf("__") + 2)
                
                    nodeName = nodeName.charAt(0).toUpperCase() + nodeName.slice(1);
                    
                    return new ChartDataView(prototypeDataId, dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        chart2D: {
            name: "chart 2D data",
            ctor: function(prototypeDataId, dataId) {
                var index = dataId.indexOf("__");
                if(index > 0) {
                    var nodeName = dataId.substring(0, dataId.indexOf("__"))
                    var portName = dataId.substring(dataId.indexOf("__") + 2)
                
                    nodeName = nodeName.charAt(0).toUpperCase() + nodeName.slice(1);
                    
                    return new Chart2DDataView(prototypeDataId, dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        video: {
            name: "video",
            ctor: function(prototypeDataId, dataId) {
                var index = dataId.indexOf("__");
                if(index > 0) {
                    var nodeName = dataId.substring(0, dataId.indexOf("__"))
                    var portName = dataId.substring(dataId.indexOf("__") + 2)
                
                    nodeName = nodeName.charAt(0).toUpperCase() + nodeName.slice(1);
                    
                    return new VideoDataView(prototypeDataId, dataId, { name: nodeName + " " + portName });
                }
            }
        },
    },
    
    createdViews: {},
    
    initialize: function() {
        this.resetGridster();
    },
    
    resetGridster: function() {
        // reset previously setted gridster
        if(this.$gridster) {
            this.$gridster.removeClass("ready");
            this.$gridster.html("<ul></ul>");
        }
        
        this.$gridster = $(".gridster");
        this.$gridsterUl = $(".gridster > ul");
        
        //set gridster param
        this.$gridsterUl.gridster({
            widget_margins : [ 10, 10 ],
            widget_base_dimensions : [ 270, 150 ]
        });
    },
      
    getViewNames: function() {
        var views = [];
        
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor) {
                views.push(this.views[view].name);
            }
        }
        
        return views;
    },
    
    getViewByName: function(viewName, dataId) {
        var view;
        
        for(var viewId in this.createdViews) {
            if (this.createdViews.hasOwnProperty(viewId)) {
                var createdView = this.createdViews[viewId];
                if(createdView.widgetTypeName == viewName && createdView.dataId == dataId) {
                    view = createdView;
                    break;
                }   
            }
        }
        
        return view;
    },
    
    addViewByName: function(viewName, prototypeDataId, dataId, attrs, register) {
        // default value 4 register is true
        register = typeof register !== 'undefined' ? register : true;
        
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor && this.views[view].name == viewName) {
                var createdView = this.views[view].ctor(prototypeDataId, dataId, attrs);
                if(createdView) {
                    createdView.widgetTypeId = view;
                    createdView.widgetTypeName = this.views[view].name;
                    if(register) { 
                        this.createdViews[createdView.widgetId] = createdView;
                    }
                    return createdView.widgetId;
                }
            }
        }
        
        return -1;
    },
    
    addView: function(viewId, prototypeDataId, dataId, attrs, register) {
        // default value 4 register is true
        register = typeof register !== 'undefined' ? register : true;
        
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor && view == viewId) {
                var createdView = this.views[view].ctor(prototypeDataId, dataId, attrs);
                if(createdView) {
                    createdView.widgetTypeId = view;
                    createdView.widgetTypeName = this.views[view].name;
                    if(register) { 
                        this.createdViews[createdView.widgetId] = createdView;
                    };
                    return createdView.widgetId;
                }
            }
        }
        
        return -1;
    },
    
    removeView: function(widgetId) {
        if(this.createdViews[widgetId]) {
            this.createdViews[widgetId].remove();
            delete this.createdViews[widgetId];
        }
    },
    
    setCreatedViews: function(views) {
        var changed = false;
        
        //create new views
        for(var i = 0; i < views.length; ++i) {
            var view = views[i];
            
            if(!view.viewName || !view.dataId || !view.prototypeDataId) {
                throw "bad views format";
            }
            
            var createdView = this.getViewByName(view.viewName, view.dataId);
            if(!createdView) {
                this.addViewByName(view.viewName, view.prototypeDataId, view.dataId);
                changed = true;
            }
        }
        
        //find old views (to remove)
        var viewToRemove = [];
        for(var viewId in this.createdViews) {
            if(this.createdViews.hasOwnProperty(viewId)) {
                var createdView = this.createdViews[viewId];
                
                var remove = true;
                for(var i = 0; i < views.length; ++i) {
                    var view = views[i];
                    if(createdView.widgetTypeName == view.viewName && createdView.dataId == view.dataId) {
                        remove = false;
                        break;
                    }
                }
                
                if(remove) {
                    viewToRemove.push(createdView.widgetId);
                }
            }
        }
        
        //remove old views
        for(var i = 0; i < viewToRemove.length; ++i) {
            this.removeView(viewToRemove[i]);
            changed = true;
        }
        
        var noView = XMLHttpRequest;
        for(var viewId in this.createdViews) {
            if (this.createdViews.hasOwnProperty(viewId)) {
                noView = false;    
            }
        }
        
        if(noView) {
            var gridster = $(".gridster > ul").gridster().data('gridster');
            this.resetGridster();
        }
        
        return changed;
    },
});