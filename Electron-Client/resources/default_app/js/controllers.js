angular.module("controllers", [])

.controller("MenubarController", function($rootScope, $scope, $location, $window) {
    
})

.controller("OverviewController", function($scope, $interval, AIDA64, Notification) {
    $scope.aidaData = AIDA64.fetchSync();
    
    var timer = $interval(function() {
        AIDA64.fetchAsync(function(err, data) {
            if(!err)     
                $scope.aidaData = data;  
        });   
    }, 500);
    
    $scope.$on("$destroy", function() {
        $interval.cancel(timer);
    });
})

.controller("GraphsViewController", function($scope, $interval, $window, AIDA64) {
    $scope.$watch(function() {
        return $window.innerWidth & $window.innerHeight;
    }, function() {
         $("#myChart").height($("#mainContent").height());
        $("#myChart").width($("#mainContent").width());
    });
    
    $scope.$watch("aidaData", function() {
        AIDA64.cpuClocks($scope.aidaData).forEach(function(obj, idx) {
            chart.segments[idx].value = obj;
            chart.update();
        });
    });
    
    var chartData = []
    
    $scope.aidaData = AIDA64.fetchSync();
    AIDA64.cpuClocks($scope.aidaData).forEach(function(clock, idx) {
        chartData.push({
            value: clock,
            color:"#FF5A5E",
            label:"CPU" + idx
        });
    });
    var chart = new Chart($("#myChart").get(0).getContext("2d")).PolarArea(chartData);
    
    var timer = $interval(function() {
        AIDA64.fetchAsync(function(err, data) {
            if(!err) 
                $scope.aidaData = data;     
        });   
    }, 500);
    
    $scope.$on("$destroy", function() {
        $interval.cancel(timer);
    });
})

.controller("MainController", function($scope, AIDA64, Notification) {

})
