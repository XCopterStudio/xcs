var DataView = Backbone.View.extend({

    id: 'data-from-onboard',
    
    views: {
        raw: {
            name: "raw data",
            ctor: function(dataId) {
                //var dataName = dataId.charAt(0).toUpperCase() + dataId.slice(1);
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    
                    return new RawDataView(dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        gauge100: {
            name: "gauge data (0 - 100)",
            ctor: function(dataId) {
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    
                    return new GaugeDataView(dataId, { 
                        name: nodeName + " " + portName,
                        min: 0,
                        max: 100,
                    });
                 }
            }
        },
        gauge1000: {
            name: "gauge data (0 - 1000)",
            ctor: function(dataId) {
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    
                    return new GaugeDataView(dataId, { 
                        name: nodeName + " " + portName,
                        min: 0,
                        max: 1000,
                    });
                 }
            }
        },
        progress: {
            name: "progress data",
            ctor: function(dataId) {
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    
                    return new ProgressDataView(dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        chart: {
            name: "chart data",
            ctor: function(dataId) {
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    return new ChartDataView(dataId, { name: nodeName + " " + portName });
                 }
            }
        },
        video: {
            name: "video",
            ctor: function(dataId) {
                var words = dataId.split("_");
                if(words.length == 2) {
                    var nodeName = words[0].charAt(0).toUpperCase() + words[0].slice(1);;
                    var portName = words[1];
                    return new VideoDataView(dataId, { name: nodeName + " " + portName });
                }
            }
        },
    },
    
    createdViews: {},
    
    initialize: function() {
        //set gridster param
        $(".gridster > ul").gridster({
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
    
    addViewByName: function(viewName, dataId, attrs, register) {
        // default value 4 register is true
        register = typeof register !== 'undefined' ? register : true;
        
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor && this.views[view].name == viewName) {
                var createdView = this.views[view].ctor(dataId, attrs);
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
    
    addView: function(viewId, dataId, attrs, register) {
        // default value 4 register is true
        register = typeof register !== 'undefined' ? register : true;
        
        console.log("ADD VIEW: " + viewId);
        for(var view in this.views) {
            if (this.views.hasOwnProperty(view) && this.views[view].name && this.views[view].ctor && view == viewId) {
                var createdView = this.views[view].ctor(dataId, attrs);
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
            
            if(!view.viewName || !view.dataId) {
                throw "bad views format";
            }
            
            var createdView = this.getViewByName(view.viewName, view.dataId);
            if(!createdView) {
                this.addViewByName(view.viewName, view.dataId);
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
        
        return changed;
    },
});