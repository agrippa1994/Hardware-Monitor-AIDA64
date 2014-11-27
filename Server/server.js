var WebSocketServer = require('websocket').server;
var http = require('http');
var fs = require("fs");
var url = require("url");
var path = require("path");

function from_string(jsonString)
{
	try 
	{
		var o = JSON.parse(jsonString);
		if (o && typeof o === "object" && o !== null)
			return o;
	}
	catch (e) { }

	return false;
}

function to_string(object) {
	try 
	{
		var s = JSON.stringify(object);
		if(s && typeof s === "string" && s !== null)
			return s;
	}
	catch(e) { }

	return false;
}

var connections = [];
function onClientConnect(connection)
{
	connections.push(connection);
}

function onClientDisconnect(connection)
{
	var index = connections.indexOf(connection);
		if(index > -1)
			connections.splice(index, 1);
}

function onClientMessage(connection, messageStr)
{
	var message = from_string(messageStr);
		if(message == false)
			return;

	if(!("computerName" in message && "sensorValues" in message))
		return;

	var sendee = to_string(message.sensorValues);
	for(var i = 0; i < connections.length; i++)
	{
		if(connections[i] == connection)
			continue;

		connections[i].send(sendee);
	}
}


var server = http.createServer();
server.listen(8080);
server.on("error", function(e)
{
	console.error("Error @http (" + e + ")");
});

wsServer = new WebSocketServer(
{
	httpServer: server,
	autoAcceptConnections: false
});

wsServer.on('request', function(request) 
{
	var connection;
	try
	{
		connection = request.accept(null, request.origin);
	}
	catch(e)
	{
		console.error("Illegal connection");
		return;
	}

	onClientConnect(connection);

	connection.on('message', function(message) 
	{
		if(message.type === 'utf8') 
			onClientMessage(connection, message.utf8Data);
	});

	connection.on('close', function(reasonCode, description) 
	{
		onClientDisconnect(connection);
	});
});
