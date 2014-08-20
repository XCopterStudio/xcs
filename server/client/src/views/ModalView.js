ModalType = ENUM("STANDARD", "CUSTOM");
    
var ModalView = Backbone.View.extend({

    id: 'modals',
    
    initialize: function() {
        this.requests = [];
        this.shown = false;
    },
    
    show: function(title, msg) {
        if(msg) {
            this.requests.push({
                type: ModalType.STANDARD,
                title: title,
                msg: msg,
            });
        }
        
        this.process();
    },
    
    showModal: function(modalId, options) {
        // default value
        options = typeof options !== 'undefined' ? options : "show";
        
        if(modalId) {
            this.requests.push({
                type: ModalType.CUSTOM,
                id: modalId,
                options: options,
            });
        }
        
        this.process();
    },
    
    process: function() {
        if(!this.shown && this.requests.length > 0) {
            this.shown = true;
            
            var request = this.requests.shift();
            
            var modalId;
            var options;
            
            switch(request.type) {
                case ModalType.STANDARD:
                    $('#modal-msg .modal-title').html(request.title);
                    $('#modal-msg .modal-body').html(request.msg);
                    modalId = '#modal-msg';
                    options = 'show';        
                    break;
                case ModalType.CUSTOM:
                    modalId = request.id;
                    options = request.options;
                    break;
            }
            
            // show modal 
            if(modalId && options) {
                $(modalId).modal(options);
                
                var self = this;
                $(modalId).on('hidden.bs.modal', function (e) {
                    self.shown = false;
                    self.process();
                });
            }
        }
    },
});