var g_oldActiveElement;
var g_wsClient;

function HardwareMonitor()
{
	var that = this;

	if (arguments.callee._singletonInstance)
		return arguments.callee._singletonInstance;

	arguments.callee._singletonInstance = this;

	this.delegates = [];
	this.wsClient = new WebSocket("ws://" + location.hostname + ":8080/");
	this.forEachDelegate = function(pred)
	{
		for(var i = 0; i < this.delegates.length; i++)
			pred(this.delegates[i], i);
	}

	this.wsClient.onerror = function()
	{
		that.forEachDelegate(function(element)
		{
			if(typeof element !== "object")
				return;

			if(typeof element.onerror === "function")
				element.onerror();
		});
	}

	this.wsClient.onmessage = function(e)
	{
		that.forEachDelegate(function(element)
		{
			if(typeof element !== "object")
				return;

			if(typeof element.onmessage === "function")
				element.onmessage(e);
		});
	}

	this.wsClient.onopen = function()
	{
		that.forEachDelegate(function(element)
		{
			if(typeof element !== "object")
				return;

			if(typeof element.onopen === "function")
				element.onopen();
		});
	}

	this.wsClient.onclose = function(e)
	{
		that.forEachDelegate(function(element)
		{
			if(typeof element !== "object")
				return;

			if(typeof element.onclose === "function")
				element.onclose(e);
		});
	}
}



function loadSite(url, completion)
{
	$.post("content.php", "site=" + encodeURIComponent(url), completion);
}

$("li").click(function() 
{
	var url = $(this).attr("loadee");
		if(typeof url !== "string")
			return;

		var that = this;
		loadSite(url, function(data)
		{
		if(typeof g_oldActiveElement !== "undefined")
			$(g_oldActiveElement).removeClass("active");

		$(that).addClass("active");
		g_oldActiveElement = that;

		$("#content").html(data);
	});
});

$(document).ready(function()
{
	new HardwareMonitor;

	loadSite("overview.php", function(data)
	{
		g_oldActiveElement = $("#mainMenuPoint");
		$(g_oldActiveElement).addClass("active");

		$("#content").html(data);
	});
});