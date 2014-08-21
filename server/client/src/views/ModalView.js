ModalType = ENUM("STANDARD", "CUSTOM");
ModalButtonType = ENUM("YES", "NO", "OK", "CANCEL");
    
var ModalView = Backbone.View.extend({

    id: 'modals',
    
    modalResult: ModalButtonType.None,
    
    initialize: function() {
        this.requests = [];
        this.shown = false;
        
        this.$btnYes = $('#modal-msg .btn-yes');
        this.$btnNo = $('#modal-msg .btn-no');
        this.$btnOk = $('#modal-msg .btn-ok');
        this.$btnCancel = $('#modal-msg .btn-cancel');
        
        this.onModalClose = _.bind(this.onModalClose, this);
        this.$btnYes.click({result: ModalButtonType.YES}, this.onModalClose);
        this.$btnNo.click({result: ModalButtonType.NO}, this.onModalClose);
        this.$btnOk.click({result: ModalButtonType.OK}, this.onModalClose);
        this.$btnCancel.click({result: ModalButtonType.CANCEL}, this.onModalClose);
    },
    
    onModalClose: function(event) {
        this.modalResult = event.data.result;
    },
    
    show: function(msg, buttons, callback, title) {
        if(msg) {
            this.requests.push({
                type: ModalType.STANDARD,
                msg: msg,
                buttons: buttons,
                callback: callback,
                title: title,
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
                    if(request.title) {
                        $('#modal-msg .modal-title').html(request.title);
                    }
                    $('#modal-msg .modal-body').html(request.msg);
                    modalId = '#modal-msg';
                    options = { show: true, backdrop: false, keyboard: false };   
                    
                    // Show/hide buttons
                    if(!request.buttons || request.buttons == ModalButtonType.None) {
                        request.buttons = ModalButtonType.OK;
                    }
                    if(((request.buttons & ModalButtonType.YES) == ModalButtonType.YES)) {
                        this.$btnYes.removeClass("hidden");
                    }
                    else {
                        this.$btnYes.addClass("hidden");
                    }
                    if(((request.buttons & ModalButtonType.NO) == ModalButtonType.NO)) {
                        this.$btnNo.removeClass("hidden");
                    }
                    else {
                        this.$btnNo.addClass("hidden");
                    }
                    if(((request.buttons & ModalButtonType.OK) == ModalButtonType.OK)) {
                        this.$btnOk.removeClass("hidden");
                    }
                    else {
                        this.$btnOk.addClass("hidden");
                    }
                    if(((request.buttons & ModalButtonType.CANCEL) == ModalButtonType.CANCEL)) {
                        this.$btnCancel.removeClass("hidden");
                    }
                    else {
                        this.$btnCancel.addClass("hidden");
                    }
                    
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
                    if(request.callback) {
                        if(self.modalResult) {
                            request.callback(self.modalResult);
                        }
                        else {
                            request.callback(ModalButtonType.None);
                        }
                    }
                    
                    self.shown = false;
                    self.process();
                });
            }
        }
    },
});