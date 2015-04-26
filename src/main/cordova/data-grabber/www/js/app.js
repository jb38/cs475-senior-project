
// TODO add license text

/*
  String <-> ArrayBuffer conversion courtesy of HTML5 Rocks (.com)
  http://updates.html5rocks.com/2012/06/How-to-convert-ArrayBuffer-to-and-from-String
  but modified to use Uint8 types, instead of Uint16.
*/

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

function str2ab(str) {
  var buf = new ArrayBuffer(str.length);
  var bufView = new Uint8Array(buf);
  for (var i=0, strLen=str.length; i<strLen; i++) {
    bufView[i] = str.charCodeAt(i);
  }
  return buf;
}

function ab2float(ab) {
  var a = new Float32Array(ab);
  return a[0];
}

function ab2int(ab) {
  var a = new Int32Array(ab);
  return a[0];
}

function c2f(c) {
  return c * (9/5) + 32;
}

function pad(str, len, pad) {
  str = str.toString();
  while (str.length < len) {
    str = pad + str;
  }
  return str;
}

function date2str(date) {
  return pad(date.getFullYear(), 4, "0") + "-" +
         pad(date.getMonth() + 1, 2, "0") + "-" +
         pad(date.getDate(), 2, "0") + "T" +
         pad(date.getHours(), 2, "0") + ":" +
         pad(date.getMinutes(), 2, "0") + ":" +
         pad(date.getSeconds(), 2, "0") + "Z";
}

var app = {

  commands: {
    GET_TIME: "GetTime",
    SET_TIME: "SetTime",
    GET_ID: "GetStationId",
    SET_ID: "SetStationId",
    GET_STATUS: "GetOperatingStatus",
    GET_TEMPERATURE: "GetTemperature",
    GET_ALL_DATA: "GetObservationData",
    GET_BATTERY: "GetBattery"
  },

  pollLength: 5,

  initialize: function() {
    this.bindEvents();
  },
  // Bind Event Listeners
  //
  // Bind any events that are required on startup. Common events are:
  // 'load', 'deviceready', 'offline', and 'online'.
  bindEvents: function() {

    //document.addEventListener('online', this.onDeviceOnline, false);
    //document.addEventListener('offline', this.onDeviceOffline, false);
    //document.addEventListener('resume', this.onDeviceReady, false);
    document.addEventListener('deviceready', this.onDeviceReady, false);
  },

  /*
  onDeviceOnline: function() {

  },

  onDeviceOffline: function() {

  },

  onDeviceResume: function() {

  },
  */

  onDeviceReady: function() {

    console.log(cordova.file.dataDirectory);

    console.log(device.platform.toLowerCase());
    $('head').append('<link rel="stylesheet" type="text/css" href="ratchet/css/ratchet-theme-' + device.platform.toLowerCase() + '.min.css">');

    if (device.platform === "iOS") {
      StatusBar.overlaysWebView( false );
      StatusBar.backgroundColorByHexString('#ffffff');
      StatusBar.styleDefault();
    }

    rfduino.isEnabled(function() {
      $("#refresh-list-button").on("click", app.refreshDeviceList);
      app.refreshDeviceList();
    }, function() {
      $("#logger-list").append(
        "<li class='table-view-cell error'>Bluetooth is not enabled</li>");
    });

    $("#disconnect-button").on("click", app.disconnect);

    $("#set-device-time").on("click", function() {
      rfduino.isConnected(function() {
        var d = new Date();
        var time = Math.floor((d.getTime() + d.getTimezoneOffset() * 60 * 1000) / 1000);
        app.sendCommand(app.commands.SET_TIME, time)
           .then(function(data) {
             console.log(ab2int(data));
           });
      });
    });

    $("#refresh-device-data").on("click", app.refreshDeviceData);

    $("#get-device-data").on("click", function() {
      rfduino.isConnected(function() {
        app.sendCommand(app.commands.GET_ALL_DATA)
          .then(function(data) {
            // TODO
          });
      });
    });
  },

  refreshDeviceList: function() {

    // empty the device list
    $("#logger-list .device").remove();
    $("#logger-list .error").remove();

    // display the scanning indicator
    $("#logger-list .scanning").css("opacity", 1);
    $("#refresh-list-button").prop("disabled", true);

    // execute this code approximately the same time that scanning is complete
    setTimeout(function() {
      // hide the scanning indicator
      $("#logger-list .scanning").css("opacity", 0);
      $("#refresh-list-button").prop("disabled", null);

      // display message when no devices found
      if ($(".table-view-cell", "#logger-list").length === 1) {
        $("#logger-list").append(
          "<li class='table-view-cell device'>No devices found</li>");
      }
    }, app.pollLength * 1000 + 5);

    rfduino.discover(app.pollLength, app.onDiscoverDevice, app.onError);
  },

  onDiscoverDevice: function(device) {

    // create the item in the list for the device
    $("#logger-list").append(
      "<li class='table-view-cell device'>" + device.name +
      " <button data-device-uuid='" + device.uuid +
      "' class='btn manage'>Connect</button></li>");

    // bind to the button's click event
    $("button[data-device-uuid='" + device.uuid + "']", "#logger-list").on("click", function() {
      app.connect(device);
    });
  },

  connect: function(device) {

    var onConnect = function() {

      // fill in any model-specific node text
      for (var prop in device) {
        if (device.hasOwnProperty(prop)) {
          $("[data-model-device='" + prop + "']", "#devicemodal").text(device[prop]);
        }
      }

      // show the modal
      $("#devicemodal").addClass("active");

      setTimeout(app.refreshDeviceData, 250);

/*
      setTimeout(function() {
        app.sendCommand(app.commands.GET_TIME);
      }, 500);

      setTimeout(function() {
        app.sendCommand(app.commands.GET_TEMPERATURE);
      }, 750);
*/
    };

    console.log("connecting to device: " + device.name + " (" + device.uuid + ")");

    rfduino.connect(device.uuid, onConnect, app.onError);
  },

  refreshDeviceData: function() {
    console.log("refreshing device data");

    app.sendCommand(app.commands.GET_BATTERY)
      .then(function(data) {

        var batteryLevel = ab2float(data)
        $("#device-battery").text(batteryLevel.toFixed(2) + "V");

        console.log(batteryLevel.toFixed(2) + "V");

        var batteryClass = "battery-";

        if (batteryLevel > 3) {
          batteryClass += "good";
        } else if (batteryLevel > 2.4) {
          batteryClass += "okay";
        } else {
          batteryClass += "bad";
        }

        $("#device-battery").removeClass("battery-good");
        $("#device-battery").removeClass("battery-okay");
        $("#device-battery").removeClass("battery-bad");
        $("#device-battery").addClass(batteryClass);

        app.sendCommand(app.commands.GET_TEMPERATURE)
          .then(function(data) {

            var temperature = ab2float(data);

            console.log(temperature + "*C");

            $("#device-temperature").html(temperature.toFixed(3) + "&deg;C / " + c2f(temperature).toFixed(3) + "&deg;F");

            app.sendCommand(app.commands.GET_TIME)
              .then(function(data) {

                var dtStr = date2str(new Date(ab2int(data) * 1000));

                console.log(dtStr);

                $("#device-time").text(dtStr);
              });
          });
      });
  },

  disconnect: function() {

    var closeModal = function() {
      $("#devicemodal").removeClass("active");
    };

    console.log("disconnecting from device");

    rfduino.disconnect(closeModal, app.onError);
  },

  sendCommand: function(command, arg) {

    var d = $.Deferred();
    rfduino.onData(d.resolve, d.reject);

    var data = str2ab(command + arg);

    rfduino.write(data, function() { }, app.onError);

    return d.promise();
  },

  onData: function(data) {

    if (app._lastCommand === app.commands.GET_TIME) {

    } else if (app._lastCommand === app.commands.SET_TIME) {

    } else if (app._lastCommand === app.commands.GET_ID) {

    } else if (app._lastCommand === app.commands.SET_ID) {

    } else if (app._lastCommand === app.commands.GET_STATUS) {

    } else if (app._lastCommand === app.commands.GET_TEMPERATURE) {
      var temperature = ab2float(data)
      $("#device-temperature").text(temperature.toFixed(3));
    } else if (app._lastCommand === app.commands.GET_ALL_DATA) {

    } else if (app._lastCommand === app.commands.GET_BATTERY) {
      var batteryLevel = ab2float(data)
      $("#device-battery").text(batteryLevel.toFixed(2));
    }
  },

  onError: function(reason) {
    console.log("ERROR");
    console.log(reason);
  }
};
app.initialize();
