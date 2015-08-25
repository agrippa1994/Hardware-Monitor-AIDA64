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

.controller("MainController", function($scope, AIDA64, Notification) {

})
