var WebSocketServer = require('websocket').server;
var http = require('http');
var fs = require("fs");
var url = require("url");
var path = require("path");
var mysql = require("mysql");

var mysql_connection;
var httpServer;
var wsServer;
var connections = [];

try {
    httpServer = http.createServer();
    httpServer.listen(from_string(fs.readFileSync("config.json")).http.port);
} catch (err) {
    console.log("Error while creating HTTP server: " + err);
    process.exit();
}

try {
    wsServer = new WebSocketServer({
        httpServer: httpServer,
        autoAcceptConnections: false
    });
} catch (err) {
    console.error("Error while creating WebSocket server on the HTTP server socket: " + err);
    process.exit();
}

try {
    mysql_connection = mysql.createConnection(from_string(fs.readFileSync("config.json")).mysql);
    mysql_connection.connect(function (err) {
        if (err) {
            console.error("Error while connecting to the datebase: " + err);
            process.exit();
        } else {
            console.log("Connection to the database has been established");
        }
    });
} catch (error) {
    console.error("Error while creating mysql-connection: " + err);
    process.exit();
}


function from_string(jsonString)
{
	try {
		var o = JSON.parse(jsonString);
		if (o && typeof o === "object" && o !== null)
			return o;
	}
	catch (e) { }

	return false;
}

function to_string(object) {
	try {
		var s = JSON.stringify(object);
		if(s && typeof s === "string" && s !== null)
			return s;
	}
	catch(e) { }

	return false;
}

function onClientConnect(connection) {
    connections.push(connection);
}

function onClientDisconnect(connection) {
	var index = connections.indexOf(connection);
		if(index > -1)
			connections.splice(index, 1);
}

function onClientMessage(connection, messageStr) {
	var message = from_string(messageStr);
		if(message == false)
			return;

	if(!("computerName" in message && "sensorValues" in message && "userName" in message))
		return;
    
	var sendee = to_string(message.sensorValues);
	for(var i = 0; i < connections.length; i++) {
		if(connections[i] == connection)
			continue;

		connections[i].send(sendee);
	}
}

httpServer.on("error", function(e) {
	console.error("Error @http (" + e + ")");
});

wsServer.on('request', function(request) {
	var connection;
	try {
		connection = request.accept(null, request.origin);
	} catch(e) {
		console.error("Illegal connection");
		return;
	}

	onClientConnect(connection);

	connection.on('message', function(message) {
		if(message.type === 'utf8') 
			onClientMessage(connection, message.utf8Data);
	});

	connection.on('close', function(reasonCode, description) {
		onClientDisconnect(connection);
	});
});
