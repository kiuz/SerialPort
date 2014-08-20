import 'package:serial_port/serial_port.dart';
import 'dart:async';

main(){
  var arduino = new SerialPort("/dev/tty.usbmodem1411");
  arduino.onRead.map(BYTES_TO_STRING).listen(print);
  arduino.open().then((_) {
    print("Ctrl-c to close");
    new Timer(new Duration(seconds: 2), () => arduino.write("Hello !"));
  });
}