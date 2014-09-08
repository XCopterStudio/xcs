function PingMonitor() {

    var latency_ = -1,
        heartbeatCallback_,
        sendCallback_;

    function startHeartbeat(sendCallback, interval) {
        sendCallback_ = sendCallback;
        interval = interval || 3000; // defaulted to 3 secs
        heartbeatCallback_ = setInterval(ping, interval);
    };

    function stopHeartbeat() {
        clearInterval(heartbeatCallback_);
    }

    function ping() {
        var now = new Date().getTime();
        var payload = { timestamp: now.toString() };
        sendCallback_(payload);
    };

    function setEcho(data) {
        latency_ = new Date().getTime() - parseInt(data['timestamp']);
        latency_ /= 2;
    }

    return {
        startHeartbeat: startHeartbeat,
        stopHeartbeat: stopHeartbeat,
        setEcho: setEcho,
        getLatency: function() { return latency_; },
    };
}

module.exports = PingMonitor;