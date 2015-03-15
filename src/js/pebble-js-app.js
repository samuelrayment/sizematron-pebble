Pebble.addEventListener("ready",
    function(e) {
      console.log("Hello world! - Sent from your javascript application.");
			var message = {};
			message['type'] = 'ready';
			Pebble.sendAppMessage(message);
    }
);

Pebble.addEventListener("appmessage",
		function(e) {
			console.log('-----------');
			for (var i in e.payload) {
				console.log(i + ': ' + e.payload[i]);
			}
			console.log("type: " + e.payload['type']);
			switch (e.payload['type']) {
				case "ready":
					sendSessionInfo();
					break;
				case "session_selected":
					selectSession(e.payload['session-id']);
					break;
			}
		}
);

function selectSession(sessionId) {
	setTimeout(function() {
		var message = {};
		message['type'] = 'ticket_chosen';
		message['ticket-id'] = 'IOS-123';
		Pebble.sendAppMessage(message);
	}, 3000);
}

function packSessionInfo(sessionInfo) {
	var packed = {};
	var startingKey = 2;
	var key = startingKey;
	for (var i in sessionInfo) {
		var info = sessionInfo[i];
		packed[''+key] = info.id;
		key++;
		packed[''+key] = info.name;
		key++;
	}
	return packed;
}

function sendSessionInfo() {
	var sessionInfo = [
		{id: 0, name: "First"},
		{id: 1, name: "Second"},
		{id: 2, name: "Third"},
	];
	packedSessionInfo = packSessionInfo(sessionInfo);
	packedSessionInfo['type'] = 'session_info';
	packedSessionInfo['session-count'] = 3;
	console.log(packedSessionInfo[2]);
	Pebble.sendAppMessage(packedSessionInfo);
}
