var MSG_TYPE = 0;

Pebble.addEventListener("ready",
    function(e) {
        console.log("Hello world! - Sent from your javascript application.");
    }
);

Pebble.addEventListener("appmessage",
		function(e) {
			switch (e.payload[MSG_TYPE]) {
				case "ready":
					sendSessionInfo();
			}
		}
);

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
	packedSessionInfo['0'] = 'session_info';
	packedSessionInfo['1'] = 3;
	console.log(packedSessionInfo[2]);
	Pebble.sendAppMessage(packedSessionInfo);
}
