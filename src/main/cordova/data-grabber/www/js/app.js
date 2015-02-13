
// TODO add license text

var app = {

  pollLength: 5,

  initialize: function() {
    this.bindEvents();
  },
  // Bind Event Listeners
  //
  // Bind any events that are required on startup. Common events are:
  // 'load', 'deviceready', 'offline', and 'online'.
  bindEvents: function() {

    document.addEventListener('online', this.onDeviceOnline, false);
    document.addEventListener('offline', this.onDeviceOffline, false);
    document.addEventListener('deviceready', this.onDeviceReady, false);
    document.addEventListener('resume', this.onDeviceReady, false);
  },

  onDeviceOnline: function() {

  },

  onDeviceOffline: function() {

  },

  onDeviceReady: function() {

    $("#refresh-list-button").on("click", app.findRFduinoDevices);

    app.findRFduinoDevices();
  },

  onDeviceResume: function() {

  },

  _onScanStart: function() {

    $("#logger-list .scanning").css("opacity", 1);
    $("#refresh-list-button").prop("disabled", true);
  },

  _onScanEnd: function() {

    $("#logger-list .scanning").css("opacity", 0);
    $("#refresh-list-button").prop("disabled", null);

    if ($(".table-view-cell", "#logger-list").length === 1) {
      $("#logger-list").append(
        "<li class='table-view-cell'>No devices found</li>");
      }
    },

    findRFduinoDevices: function() {

      app._onScanStart();
      setTimeout(app._onScanEnd, app.pollLength * 1000);

      $("#logger-list .device").remove();
      $("#logger-list .error").remove();

      rfduino.isEnabled(function() {
        rfduino.discover(
          app.pollLength,
          function(device) {

            // create the item in the list for the device
            $("#logger-list").append(
              "<li class='table-view-cell device'>" + device.name +
              " <button data-device-uuid='" + device.uuid +
              "' class='btn manage'>Manage</button></li>");

              // bind to the button's click event
              $("button[data-device-uuid='" + device.uuid + "']", "#logger-list").on("click", function() {

                // fill in any model-specific node text
                for (var prop in device) {
                  if (device.hasOwnProperty(prop)) {
                    $("[data-model-device='" + prop + "']", "#devicemodal").text(device[prop]);
                  }
                }

                // show the modal
                $("#devicemodal").addClass("active");
                /*
                console.log(JSON.stringify(navigator));

                Camera.getPicture(
                function(imageData) {
                $("#devicemodal content").append(
                "<img src='data:image/jpeg;base64," + imageData +
                "' alt='picture' />";
              });,
              function() {

            }, {
            quality : 75,
            destinationType : Camera.DestinationType.DATA_URL,
            sourceType : Camera.PictureSourceType.CAMERA,
            allowEdit : true,
            encodingType: Camera.EncodingType.JPEG,
            targetWidth: 100,
            targetHeight: 100,
            popoverOptions: CameraPopoverOptions,
            saveToPhotoAlbum: false
          });
          */
          // TODO fire connect and populate data
          /*
          rfduino.connect(
          device.uuid,
          function() {

        },
        function(reason) {

      });
      */
      /*
      var data = new ArrayBuffer(3);
      data[0] = 0xFF;
      data[1] = 0x00;
      data[2] = 0x17;
      rfduino.write(data.buffer, success, failure);
      */
    });
  });
}, function() {
  $("#logger-list").append(
    "<li class='table-view-cell error'>Bluetooth is not enabled</li>");
    app._onScanEnd();
  });
}
};
app.initialize();
