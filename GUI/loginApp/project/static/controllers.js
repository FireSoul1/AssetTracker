var userTot = "Arnab"
angular.module('myApp').controller('loginController',
  ['$scope', '$location', 'AuthService',
  function ($scope, $location, AuthService) {

    $scope.login = function () {

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
          $scope.user = $scope.registerForm.email
          user = $scope.registerForm.email
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
angular.module('myApp').controller('AppController', function($scope, $http) {
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
    $scope.gpsloc = "Unknown"

    //get devices for a User
    $http.get('/api/'+$scope.user+'/get_devices').then(function(response) {
        $scope.devices = response.data.devices;
        $log.log($scope.devices);
    });
    $scope.selectedDevice = null;

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
        $scope.selectedDevice = value;
        $scope.showDevice = !$scope.showDevice

        //get GPS location
        $scope.gpsloc = dict[value];

        //send Http request to update active user with device Id

    }


});
