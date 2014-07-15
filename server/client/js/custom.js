'use strict';
/*jslint browser: true*/
/*global $, jQuery*/

/****************/
/* CONTEXT MENU */
/****************/
(function ($, window) {
    $.fn.contextMenu = function (options) {   
        return this.each(function () {
            $(this).on("contextmenu", function (e) {
                // show context menu
                $(options.menuSelector)
                    .data("contextMenuTarget", $(e.currentTarget))
                    .show()
                    .css({
                        left: getLeft(e),
                        top: getTop(e),
                    })
                    .off('click')
                    .on('click', function (e) {
                        // hide context menu
                        $(this).hide();
                
                        // get information about click action
                        var target = $(this).data("contextMenuTarget");
                        var selectedMenuItem = $(e.target);
                        
                        // call click action
                        options.menuSelected.call(this, target, selectedMenuItem);
                });
                
                return false;
            });
    
            //make sure menu closes on any click
            $(options.parentSelector).click(function () {
                $(options.menuSelector).hide();
            });
        });
    
        function getLeft(e) {
            // select parent element - bordet for context menu
            var parent = $(options.parentSelector);
            
            // get coordinates
            var mouseX = e.pageX - parent.offset().left;
            var parentWidth = parent.width(); //$(window).width();
            var menuWidth = $(options.menuSelector).width();

            // determine if the menu will drop left/right
            if (mouseX + menuWidth > parentWidth && menuWidth < mouseX) {
                return e.pageX - menuWidth;
            } 
            
            return e.pageX;
        }        
        
        function getTop(e) {
            // select parent element - bordet for context menu
            var parent = $(options.parentSelector);
            
            // get coordinates
            var mouseY = e.pageY - parent.offset().top;
            var parentHeight = parent.height();
            var menuHeight = $(options.menuSelector).height();
            
            // determine if the menu will drop up/down
            if (mouseY + menuHeight > parentHeight && menuHeight < mouseY) {
                return e.pageY - menuHeight;
            } 
            
            return e.pageY;
        }
    
    };
})(jQuery, window);

$.fn.toggleClick = function () {
    'use strict';
    var methods = arguments, // store the passed arguments for future reference
        count = methods.length; // cache the number of methods 

    //use return this to maintain jQuery chainability
    return this.each(function (i, item) {
        // for each element you bind to
        var index = 0; // create a local counter for that element
        $(item).click(function () { // bind a click handler to that element
            return methods[index++ % count].apply(this,arguments); // that when called will apply the 'index'th method to that element
            // the index % count means that we constrain our iterator between 0 and (count-1)
        });
    });
};


function sampleRun() {

    var flash = $('#flash-messages');
    var video = document.getElementById('video-parrot');

    var animation = [
        {t: 2000, v: 0.0, h: 0.0, f: 'Turning motors on.', a: function() {
                animate();
            }},
        {t: 3000, v: 0.0, h: 0.8, f: 'Taken off and executing script.', a: function() {
                video.play();
            }},
        {t: 2000, v: 1.0, h: 1.2},
        {t: 3000, v: 2.2, h: 1.3},
        {t: 2000, v: 1.7, h: 2.2},
        {t: 10000, v: 1.5, h: 0.8},
        {t: 89000, v: 0.0, h: 0.0, f: 'Landed.'} // aproximately matches the video
    ];


    (function animate(step) {
        $('#data-kph').text(animation[step].v + ' km/h');
        $('#data-alt').text(animation[step].h + ' m');
        if (animation[step].f) {
            flash.text(animation[step].f);
        }
        if (animation[step].a) {
            animation[step].a();
        }
        setTimeout(function() {
            animate(step + 1);
        }, animation[step + 1].t);
    })(0);

}

$(function() {
    // status led
    var ledSize = 32;
    $('#status-led').height(ledSize).width(ledSize)
            .css({borderRadius: ledSize / 2, backgroundColor: "#5bb75b"});

    /***

    // initialize gridster
    var gridster = $(".gridster > ul").gridster({
        widget_margins: [10, 10],
        widget_base_dimensions: [270, 150]
    }).data('gridster');
    
    gridster.disable();
    $('#gridster-enable').toggleClick(function (){
        gridster.enable();
        $('#gridster-enable').parent().addClass('active');
    }, function (){
        gridster.disable();
        $('#gridster-enable').parent().removeClass('active');
    });
    
    // sidepanels init
    //$("#left-menu").css("float", "left").css("background", "whitesmoke").css("overflow", "hidden").css("width", "50px").css("z-index", "1001");
    
    initSidepanels();
    
    sampleRun();
    
    ***/
    
    // Setup drop down menu (4 input)
    $('.dropdown-toggle').dropdown();
    $('.dropdown-menu input, .dropdown-menu label, .dropdown-menu .dropdown-header').click(function(e) {
        e.stopPropagation();
    });
});

function initSidepanels() {
    var startWidth = 70;
    var endWidth = 300;
    
    $('.sidepanel').width(startWidth);
    $('.sidepanel li > a > h3').hide();
    
    $('.sidepanel-left-trigger').toggleClick(function () {
        $('.sidepanel-left').animate({
            width: endWidth
        }, 0);
        $('.sidepanel-left li > a > h3').show();
    }, function () {
        $(".sidepanel-left").animate({
            width: startWidth
        }, 0);
        $('.sidepanel-left li > a > h3').hide();
    });
    
    $('.sidepanel-right-trigger').toggleClick(function () {
        $('.sidepanel-right').animate({
            width: endWidth
        }, 0);
        $('.sidepanel-right li > a > h3').show();
    }, function () {
        $(".sidepanel-right").animate({
            width: startWidth
        }, 0);
        $('.sidepanel-right li > a > h3').hide();
    });
}

// create enum
var ENUM = function() {
    var self = {};
    
    // vars 4 enum mask
    var maskValue = 1;
    self.mask = {};
    
    // add enum values from arguments
    for (var arg in arguments) {
        self[arguments[arg]] = maskValue;
        
        self.mask[arguments[arg]] = maskValue;
        maskValue *= 2;
    }
    
    // add enum value for nothing
    var noneKey = "None";
    if(!self[noneKey]){
        self[noneKey] = 0;
        self.mask[noneKey] = 0;
    }
    else {
        noneKey = "";
    }
    
    // add enum value for everithing
    var allKey = "All";
    if(!self[allKey]) {
        maskValue = 0;
        for(var m in self.mask) {
            if(self.mask[m]) {
                maskValue |= self.mask[m];
            }
        }
        
        self[allKey] = maskValue;
        self.mask[allKey] = maskValue;
    }
    else {
        allKey = "";
    }
    
    // find single name for value
    self.getName = function(value) {
        for(var p in self.mask) {
            if(self.mask[p] == value) {
                return p;
            }
        }
    };
    
    // find array of all names for value
    self.getNames = function(value) {
        var names = [];
        for(var p in self.mask) {
            if(self.mask[p] && p != noneKey && p != allKey) {
                var contain = ((self.mask[p] & value) == self.mask[p]);
                if(contain) {
                    names.push(p);
                }
            }
        }
        
        return names;
    };
       
    // freeze the enum
    if (Object.freeze) {
        try {
            Object.freeze(self);   
        }
        catch(ex){
        }
    }
    
    return self;
};