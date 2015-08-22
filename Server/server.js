var fs = require("fs");
var expressWS = require("express-ws")(require("express")());

var app = expressWS.app;

app.ws('/sensor', function(ws, req) {
	ws.data = {};
	ws.data.computerName = "";
	ws.data.sensorData = {};

	ws.on('message', function(msg) {
		try {
			var data = JSON.parse(msg);

			if (typeof data.computerName === "string")
				ws.data.computerName = data.computerName;

			if (typeof data.sensorValues === "object")
				ws.data.sensorData = data.sensorValues;

			expressWS.clientsForRoute("/client").filter(function(client) {
				return ws.data.computerName == client.data.watchComputer && ws.data.sensorData != null;
			}).forEach(function(client) {
				client.send(JSON.stringify(ws.data.sensorData));
			});

		} catch (e) {
			ws.close();
		}
	});

	ws.on("close", function() {
		expressWS.clientsForRoute("/client").filter(function(client) {
			return ws.data.computerName == client.data.watchComputer;
		}).forEach(function(client) {
			client.close();
		});
	});
});

app.ws("/client", function(ws) {
	ws.data = {};
	ws.data.watchComputer = "";

	ws.on("message", function(msg) {
		try {
			var data = JSON.parse(msg);

			if (typeof data.watchComputer === "string")
				ws.data.watchComputer = data.watchComputer;

		} catch (e) {
			ws.close();
		}
	});
});

app.get("/api/computers", function(req, res) {
	var array = [];
	expressWS.clientsForRoute("/sensor").filter(function(client) {
		return client.data.computerName != "" && client.data.sensorData != null;
	}).forEach(function(client) {
		array.push(client.data);
	});
	res.status(200).send(JSON.stringify(array));
});

app.get("*", function(req, res) {
	res.sendFile(__dirname + "/web/" + (req.params["0"] || "index.html"), function(err) {
		if (err) {
			res.sendStatus(err.status);
		}
	});
});


try {
	app.listen(JSON.parse(fs.readFileSync("config.json")).http.port);
} catch (err) {
	console.log("Error while creating HTTP server: " + err);
	process.exit();
}
