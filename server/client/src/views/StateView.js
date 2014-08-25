var StateView = Backbone.View.extend({
    id : 'state',
    
    el : 'body',
    
    initialize : function() {
        window.onbeforeunload = function() {
            //TODO: return context text by x-copter state?
            return "X-copter plan execution will be stopped!"; //Are you sure you want to navigate away?
        }
        
        window.onunload = function() {
            //TODO: send message to onboard?
            console.log("DISCONNECTING FROM ONBOARD!");
        }
        
        window.onload = function() {
            // show all x-copter tooltips
            $("[xtitle]").xtooltip();
        }
    },
});

(function ($) {
    $.fn.xtooltip = function () {   
        return this.each(function () {
            // prepare content
            var content = '';
            var title = $(this).attr("xtitle");
            var group = $(this).closest("[xtitle-group]").attr("xtitle-group");
            if(group) {
                content = '<strong>' + group + ': </strong>';
            }
            if(title) {
                content += '<p>' + title + '</p>'
            }
            
            // prepare placement: top/bottom
            var offsetTop = getOffsetTop($(this));
            var placement = 'top';
            if(offsetTop < 150) {
                placement = 'bottom';    
            }
            
            // set tooltip
            if(content) {
                $(this).tooltip({
                    title: content,
                    placement: placement,
                    delay: { show: 1000, hide: 0 },
                    html: true,
                });
            }
        });
    };
    
    function getOffsetTop($el) {
        var $current = $el;
        while(true) {
            var offset = $current.offset();
            if(offset && offset.top > 0) {
                break;
            }
                
            $current = $current.parent();
            
            if($current.length == 0) {
                $current = $el;
                break;
            }
        }
        
        return $current.offset().top;
    }
})(jQuery);