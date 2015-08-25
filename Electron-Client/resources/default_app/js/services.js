var aida64 = require("node-aida64");

angular.module("services", [])

.factory("Notification", function() {
    return function(caption, content, style, keepOpen) {
        style = style || "success";
        keepOpen = keepOpen || false;
        
        $.Notify({ caption: caption, content: content, type: style, keepOpen: keepOpen });
    };
})

.factory("AIDA64", function() {
    function prepareData(data) {
        if(data)
            for(key in data)
                if(!Array.isArray(data[key]))
                    data[key] = [data[key]];
        return data;
    }
    
    return {
        fetchAsync: function(callback) {
            return aida64.readAIDA64(function(err, data){
                callback(err, prepareData(data));
            });
        },
        
        fetchSync: function(callback) {
            callback = callback || function() {};
            
            try {
                var data = aida64.readAIDA64Sync();
                data = prepareData(data);
                
                callback(null, data);
                return data;
            } catch(e) {
                callback(e, null);
            }
        }
    }
})
