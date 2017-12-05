// Changes XML to JSON

function xmlToJson(xml) {
    'use strict';
    // Create the return object
    var obj = {}, i, j, attribute, item, nodeName, old;

    if (xml.nodeType === 1) { // element
        // do attributes
        if (xml.attributes.length > 0) {
            obj["@attributes"] = {};
            for (j = 0; j < xml.attributes.length; j = j + 1) {
                attribute = xml.attributes.item(j);
                obj["@attributes"][attribute.nodeName] = attribute.nodeValue;
            }
        }
    } else if (xml.nodeType === 3) { // text
        obj = xml.nodeValue;
    }

    // do children
    if (xml.hasChildNodes()) {
        for (i = 0; i < xml.childNodes.length; i = i + 1) {
            item = xml.childNodes.item(i);
            nodeName = item.nodeName;
            if ((obj[nodeName]) === undefined) {
                obj[nodeName] = xmlToJson(item);
            } else {
                if ((obj[nodeName].push) === undefined) {
                    old = obj[nodeName];
                    obj[nodeName] = [];
                    obj[nodeName].push(old);
                }
                obj[nodeName].push(xmlToJson(item));
            }
        }
    }
    return obj;
};


angular.module('myApp').controller('loginController',
['$scope', '$location', 'AuthService',
function ($scope, $location, AuthService) {

    $scope.login = function () {
        console.log('loggin in')
        // initial values
        $scope.error = false;
        $scope.disabled = true;
        userTot = $scope.loginForm.email
        console.log($scope.loginForm.email);
        console.log($scope.loginForm.password);
        // call login from service
        AuthService.login($scope.loginForm.email, $scope.loginForm.password)
        // handle success
        .then(function () {
            $location.path('/');
            $scope.disabled = false;
            $scope.user = $scope.loginForm.email;
            user = $scope.loginForm.email;
            $scope.loginForm = {};
        })
        // handle error
        .catch(function () {
            $scope.error = true;
            $scope.errorMessage = "Invalid username and/or password";
            $scope.disabled = false;
            $scope.loginForm = {};
        });

    };

}]);

angular.module('myApp').controller('logoutController',
['$scope', '$location', 'AuthService',
function ($scope, $location, AuthService) {

    $scope.logout = function () {

        // call logout from service
        AuthService.logout()
        .then(function () {
            $location.path('/login');
        });

    };

}]);
/*angular.module('myApp').controller('MyCtrl',
['$scope', '$location', '$$window', 'ngMap'
function(NgMap){
var vm = this;
NgMap.getMap().then(function(map) {
vm.showCustomMarker= function(evt) {
map.customMarkers.foo.setVisible(true);
map.customMarkers.foo.setPosition(this.getPosition());
};
vm.closeCustomMarker= function(evt) {
this.style.display = 'none';
};
});
});*/

angular.module('myApp').controller('registerController',
['$scope', '$location', 'AuthService',
function ($scope, $location, AuthService) {

    $scope.register = function () {

        // initial values
        $scope.error = false;
        $scope.disabled = true;

        // call register from service
        AuthService.register($scope.registerForm.email,
            $scope.registerForm.password)
            // handle success
            .then(function () {
                $location.path('/login');
                $scope.disabled = false;
                $scope.registerForm = {};
            })
            // handle error
            .catch(function () {
                $scope.error = true;
                $scope.errorMessage = "Something went wrong!";
                $scope.disabled = false;
                $scope.registerForm = {};
            });

        };

    }]);
    angular.module('myApp').controller('AppController',
    ['$scope', '$http', '$window',
    function($scope, $http, $window) {
        //create a temporary dictionary
        var dict = {};
        dict["dev1"] =
        "22 Ross Ade Drive,  West Lafayette, IN 47907"
        dict["dev2"] =
        "305 N University St, West Lafayette, IN 47907"
        dict["dev3"] =
        "2827 US-52 Suite D, West Lafayette, IN 47906"


        $scope.time = "unknown...";
        $scope.showDiv = false;
        $scope.user = userTot
        $scope.userID = -100
        $scope.gpsloc = ["Unknown","sadfsdf"]
        $scope.select_path = "/static/zoid1.png"
        $scope.selectedDevice = null;
        $scope.address = "This is the Address... NOT"

        //get devices for a User
        console.log('scope.user: ' + $scope.user);
        $http.get('/api/'+$scope.user+'/get_devices').then(function(response) {
            $scope.devices = response.data.devices;
            $scope.userID = response.data.userid
            console.log($scope.devices);
        });

        //login User
        $scope.login_user = function() {
            //get the user data
            $http.get('/api/login/'+user).then(function(response) {
                $scope.devices = response.data.username;
                $log.log("this is the shit")
            });
            //get the devices
            $http.get('/api/'+$scope.user+'/get_devices').then(function(response) {
                $scope.devices = response.data.devices;
            });
        }

        //get selected device
        $scope.select_device = function(value) {
            //TODO send Http request to update active user with device Id
            $http.get('/'+value+'/'+$scope.userID+'/active').then(function(response) {

                console.log("Done! "+response.data.status)
                if(response.data.status == -1 && !$scope.showDevice) {
                    alert("Device is locked! Contact the other users: "+response.data.others);
                    console.log("errorrrr")
                }
                else {
                    $scope.selectedDevice = value;
                    $scope.showDevice = !$scope.showDevice

                    if($scope.showDevice ==  true)
                        //get GPS location
                        $http.get('/'+value+'/location').then(function(response) {
                        $scope.gpsloc[0] = response.data.Xloc;
                        $scope.gpsloc[1] = response.data.Yloc;
                        console.log("GPS Done!")
                        console.log('hello')
                        //get the address
                        lat = $scope.gpsloc[0];
                        lng = $scope.gpsloc[1];
                        url = 'http://api.geonames.org/findNearestAddress?lat='+lat+'&lng='+lng+'&username=gkondapa';
                        $http.get(url).then(function(response) {
                            if(response.data != null) {
                        //        console.log("Datata "+response.data);
                                var tmp = response.data+"";
                                var parser = new DOMParser();
                                var dom = parser.parseFromString(tmp, "text/xml")
                                var jsoned = xmlToJson(dom);
                        //        console.log("Json: "+JSON.stringify(jsoned));
                                var start = jsoned.geonames.address
                                var addr = start.streetNumber['#text']+" "+start.street['#text']+", "+
                                start.placename['#text']+", "+start.adminCode1['#text']+", "+
                                start.countryCode['#text']+" "+start.postalcode['#text']
                                $scope.address = addr;
                            }


                            console.log($scope.address);
                        }, function error(_error) {
                            console.log('error: ' + error);
                        });
                    });

                }

            });
        }

        $scope.get_picture = function() {
            $http.get("/"+$scope.selectedDevice+"/picture").then(function(response) {
                $scope.image_src = response.data.picpath
            });
        }
    }]);
