'use strict';
/*jslint browser: true*/
/*global $, jQuery*/

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
    
    $('#takeoff').click(function() {
        sampleRun();
        $(this).hide();
        $('#land').show();
    });
    $('#land').click(function(){
       alert('Cannot land. The video takes 2:11.');
    });
    
    initSidepanels();
    
    sampleRun();
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
