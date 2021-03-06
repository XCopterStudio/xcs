var FlashMessagesView = Backbone.View.extend({

    id : 'flash-messages',

    initialize : function() {
        this.listenTo(app.Onboard, "change:flash", this.flash);
    },

    render : function() {

    },

    flash : function(model) {
        this.flashInfo(model.get("flash"));
    },
    
    flashSuccess : function(message) {
        this.flashMessage(message, "success", "Success:");
    },
    
    flashInfo : function(message) {
        this.flashMessage(message, "info", "Information:");
    },
    
    flashWarning : function(message) {
        this.flashMessage(message, "warning", "Warning:", 10000);
    },
    
    flashError : function(message) {
        this.flashMessage(message, "danger", "Error:", 15000);
    },
    
    flashMessage : function(message, type, head, timeout) {
        // default value 4 timeout is 5
        timeout = typeof timeout !== 'undefined' ? timeout : 5000;
        
        // create flash div
        var flash = 
            '<div class="alert alert-' + type + ' alert-dismissible fade in flash-message" role="alert">  \                                                             \
                <strong>' + head + '</strong> ' + message + '                               \
            </div>';
        
        // show flash
        var elem = $(flash).appendTo("#flash-messages");
        
        // close flash on click
        elem.click(function() {
            elem.alert('close');
        });
        
        // close flash after 5s
        window.setTimeout(function() {
            elem.alert('close');
        }, timeout);
    }
});