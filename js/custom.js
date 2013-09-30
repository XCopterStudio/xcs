'use strict';
/*jslint browser: true*/
/*global $, jQuery*/

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
    $(".gridster > ul").gridster({
        widget_margins: [10, 10],
        widget_base_dimensions: [270, 150]

    });
    //var gridster = $('.gridster > ul').gridster().data('gridster');
    /*
     $('#big-view').click(function () {
     gridster.resize_widget($('#big-view'), 3, 3);
     });
     */


    //$('#left-menu').sidr();

    $('#takeoff').click(function() {
        sampleRun();
    });
}
);
