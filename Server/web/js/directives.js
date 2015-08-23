angular.module("directives", [])

// <menubar>-tag
.directive("menubar", function() {
    return {
        templateUrl: "html/menubar.html",
        replace: true,
        restrict: "E"
    };
})
