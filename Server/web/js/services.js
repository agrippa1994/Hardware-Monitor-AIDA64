angular.module("services", [])

.factory("Notification", function() {
    return function(caption, content, style, keepOpen) {
        style = style || "success";
        keepOpen = keepOpen || false;
        
        $.Notify({ caption: caption, content: content, type: style, keepOpen: keepOpen });
    };
})
