var FlashMessagesView = Backbone.View.extend({

    id : 'flash-messages',

    initialize : function() {
        this.listenTo(app.XcopterState, "change:flash", this.flash);
        //del
        //console.log(this.$el);
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
        this.flashMessage(message, "warning", "Warning:");
    },
    
    flashError : function(message) {
        this.flashMessage(message, "danger", "Error:");
    },
    
    flashMessage : function(message, type, head) {
        // create flash div
        var flash = 
            '<div class="alert alert-' + type + ' alert-dismissible fade in" role="alert">  \
                <button type="button" class="close" data-dismiss="alert">                   \
                    <span aria-hidden="true">&times;</span>                                 \
                    <span class="sr-only">Close flash</span></button>                       \
                <strong>' + head + '</strong> ' + message + '                               \
            </div>';
        
        // show flash
        var elem = $(flash).appendTo("#flash-messages");
        
        // close flash after 5s
        window.setTimeout(function() {
            elem.alert('close');
        }, 5000);
    }
});