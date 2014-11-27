function Overview()
{
	var that = this;
	this.sensorData = [];

	this.onmessage = function(e)
	{
		var sensorData = JSON.parse(e.data);
		that.sensorData = sensorData;

		that.showSensorValues();
	}

	this.showSensorValues = function()
	{
		$("#overViewTable").empty();
		$(this.sensorData).each(function(idx, element)
		{
			$("#overViewTable").append("<tr><td>" + element.category + "</td><td>" + element.id + "</td><td>" + element.label + "</td><td>" + element.value + "</td></tr>");
		});
	}

	this.init = function()
	{
		that.showSensorValues();
	}
}

if(typeof (HardwareMonitor().overview) !== "object")
{
	var overview = new Overview;
	HardwareMonitor().overview = overview;
	HardwareMonitor().delegates.push(overview);
}

var overview = HardwareMonitor().overview;
overview.init();