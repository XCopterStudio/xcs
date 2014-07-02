var WaitView = Backbone.View.extend({
    id : 'wait',
    
    el : 'body',
    
    initialize : function() {
        $(window).resize(this.setFontSize);
        $(window).load(this.setFontSize);
        
        //debug
//        var action = new WaitAction(".red", WaitActionType.Click);
//        var self = this;
//        action.set("action", function() {
//            action.start();
//            console.log("foo"); 
//            setTimeout(function() { action.stop(); }, 5000);
//        });
//        this.setWaitAction(action);
    },
    
    setWaitAction : function(waitAction) {
        if(waitAction.isInit()) {
            
            this.listenTo(waitAction, 'change:actionState', this.onWaitActionStateChange_);
             
            $(waitAction.get("selector")).each(function() {
                console.log("cus");
                var self = $(this);
                
                // append class
                if(!self.hasClass('wait-control')) {
                    self.addClass('wait-control');
                }
        
                // appent spinner
                self.append('<div class="wait-container"><i class="icon-spinner wait-icon"></i><div class="wait-text">Wait...</div></div>');
                
                // set respond action to click
                if(waitAction.get("actionType") & WaitActionType.Click == WaitActionType.Click) {
                    self.click(function(event) {
                        event.stopPropagation();
    
                        // run action
                        try {
                            waitAction.get("action")(event);
                        }
                        catch(ex) {
                            console.log(ex.message);
                        }
                   });
                }
            });            
        }
        else { 
        }
    },
    
    onWaitActionStateChange_ : function(waitAction) {
        console.log("zmena stavu: " + waitAction.get("actionState"));
        if(waitAction.get("actionState") == WaitActionState.Started) {
            $(waitAction.get("selector")).each(function() { 
                var self = $(this);
            
                // add spin class
                self.find(".wait-icon").each(function() {
                    console.log('ee');
                    var waitIcon = $(this);
                    if(!waitIcon.hasClass('icon-spin')) {
                        waitIcon.addClass('icon-spin');
                    }
                });
                
                // show spinner
                self.toggleClass('wait-active');
                
                // disable element
                self.disable();
            });
        }
        else if(waitAction.get("actionState") == WaitActionState.Stopped) {
            $(waitAction.get("selector")).each(function() { 
                var self = $(this);
                
                // hide spinner
                self.toggleClass('wait-active');
                
                // enable element
                self.enable();
                
                // remove spin class
                self.find(".wait-icon").each(function() {
                    $(this).removeClass('icon-spin');
                });
            });
        }
    },
    
    setFontSize : function () {
        $('.wait-control').each(function() {
            var self = $(this);
            
            var iconZoom = 0.45;
            var iconAloneZoom = 0.9;
            var iconMaxSize = 100;
            var iconMinSize = 10;
            var textZoom = 0.45;
            var textMaxSize = 36;
            var alone = false;
            
            // min size
            var size = self.width() > self.height() ? self.height() : self.width();
            
            // set icon size
            var iconFontSize = size * iconZoom;
            if(iconFontSize < iconMinSize) {
                alone = true;
                iconFontSize = size * iconAloneZoom;
            }
            if(iconFontSize > iconMaxSize) {
                iconFontSize = iconMaxSize;
            }
            self.find(".wait-icon").each(function() {
                $(this).css('font-size', iconFontSize + "px");
            });
            
            // set text size
            var textFontSize = 0;
            if(!alone) {
                textFontSize = size * textZoom;
                if(textFontSize > textMaxSize) {
                    textFontSize = textMaxSize;
                }
            }
            self.find(".wait-text").each(function() {
                $(this).css('font-size', textFontSize + "px");
            });
            
            //debug
//            console.log("icon font size: " + iconFontSize);
//            console.log("text font size: " + textFontSize);
        });
    },
});


var setDisabled_ = function(self, disabled) {
    if (typeof self.disabled != "undefined") {
        var newDisabled = disabled;
        
        if(disabled) {
            if(!self.hasAttribute("disabled-backup")) {
                self.setAttribute("disabled-backup", self.disabled);
            }
        }
        else {
            if(self.hasAttribute("disabled-backup")) {
                newDisabled = JSON.parse(self.getAttribute("disabled-backup"));
            }
        }
        
        self.disabled = newDisabled;
    }
    
    var events = $._data(self, "events");
    if(events) {
        if(disabled) {
            events._click = events.click;
            events.click = null;
        }
        else {
            events.click = events._click;
            events._click = null;
        }
    }
};

$.fn.setDisabled = function(disabled) {
    return this.each(function() {
        setDisabled_(this, disabled);
    });
};

$.fn.disable = function() {
    return $(this).setDisabled(true);
};

$.fn.enable = function() {
    return $(this).setDisabled(false);
};