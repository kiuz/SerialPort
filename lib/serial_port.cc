// Licence 2

#include <cstring>
#include <stdlib.h>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "include/dart_api.h"
#include "include/dart_native_api.h"

Dart_Handle NewDartExceptionWithMessage(const char* library_url,
                                        const char* exception_name,
                                        const char* message);
/*
Called the first time a native function with a given name is called,
 to resolve the Dart name of the native function into a C function pointer.
*/
Dart_NativeFunction ResolveName(Dart_Handle name, int argc);

Dart_Handle HandleError(Dart_Handle handle);


int selectBaudrate(int baudrate_speed){
  switch(baudrate_speed){
    // TODO baudrate 0 ? B0
    case 50: return B50; break;
    case 75: return B75; break;
    case 110: return B110; break;
    case 134: return B134; break;
    case 150: return B150; break;
    case 200: return B200; break;
    case 300: return B300; break;
    case 600: return B600; break;
    case 1200: return B1200; break;
    case 1800: return B1800; break;
    case 2400: return B2400; break;
    case 4800: return B4800; break;
    case 9600: return B9600; break;
    case 19200: return B19200; break;
    case 38400: return B38400; break;
    case 57600: return B57600; break;
    case 115200: return B115200; break;
    case 230400: return B230400; break;
    #ifdef B460800
    case 460800: return B460800;break;
    #endif
    #ifdef B500000
    case 500000: return B500000; break;
    #endif
    #ifdef B576000
    case 576000: return B576000; break;
    #endif
    #ifdef B921600
    case 921600: return B921600; break;
    #endif
    #ifdef B1000000
    case 1000000: return B1000000; break;
    #endif
    #ifdef B1152000
    case 1152000: return B1152000; break;
    #endif
    #ifdef B1500000
    case 1500000: return B1500000; break;
    #endif
    #ifdef B2000000
    case 2000000: return B2000000; break;
    #endif
    #ifdef B2500000
    case 2500000: return B2500000; break;
    #endif
    #ifdef B3000000
    case 3000000: return B3000000; break;
    #endif
    #ifdef B3500000
    case 3500000: return B3500000; break;
    #endif
    #ifdef B4000000
    case 4000000: return B4000000; break;
    #endif
    #ifdef B7200
    case 7200: return B7200; break;
    #endif
    #ifdef B14400
    case 14400: return B14400; break;
    #endif
    #ifdef B28800
    case 28800: return B28800; break;
    #endif
    #ifdef B76800
    case 76800: return B76800; break;
    #endif
    default: return -1;
  }
}

int selectDataBits(int dataBits) {
  switch (dataBits) {
    case 5: return CS5;
    case 6: return CS6;
    case 7: return CS7;
    case 8: return CS8;
    default: return -1;
  }
}

int64_t openAsync(const char* portname, speed_t baudrate, int databits){
  // Open serial port
  struct termios tio;
  memset(&tio, 0, sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag= IGNPAR;
  tio.c_cflag= databits | CREAD | CLOCAL | HUPCL;
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=0;

  int tty_fd = open(portname, O_RDWR | O_NOCTTY | O_NONBLOCK);
  if(tty_fd > 0) {
    cfsetospeed(&tio, baudrate);
    cfsetispeed(&tio, baudrate);
    tcflush(tty_fd, TCIFLUSH);
    tcsetattr(tty_fd, TCSANOW, &tio);
  }
  return tty_fd;
}

int closeAsync(int64_t tty_fd){
  return close(tty_fd);
}

int sendAsync(int64_t tty_fd, const char* data){
  return write(tty_fd, data, strlen(data));
}

// TODO maybe check type
//   result.type = Dart_CObject_kNull;
void wrappedSerialPortServicePort(Dart_Port send_port_id, Dart_CObject* message){
 Dart_Port reply_port_id = message->value.as_array.values[0]->value.as_send_port;
 Dart_CObject result;
 int argc = message->value.as_array.length - 1;
 Dart_CObject** argv = message->value.as_array.values + 1;
 char *name = argv[0]->value.as_string;
 argv++;
 argc--;
 // TODO return a array : [result, "message"]
 // TODO replace by switch
 // TODO check args nb
 // TODO method return a Dart_CObject result
 if (strcmp("open", name) == 0) {
   //Dart_CObject* param0 = message->value.as_array.values[0];
   //Dart_CObject* param1 = message->value.as_array.values[1];
   const char* portname = argv[0]->value.as_string;
   int64_t baudrate_speed = argv[1]->value.as_int64;
   int64_t databits_nb = argv[2]->value.as_int64;
   int baudrate = selectBaudrate(baudrate_speed);
   int databits = selectDataBits(databits_nb);
   if(baudrate == -1){
     result.type = Dart_CObject_kNull;
     printf("Invalid baudrate");
     // TODO error invalid baudrate
   } else if(databits == -1) {
     result.type = Dart_CObject_kNull;
     printf("Invalid databits");
     // TODO error invalid databits
   } else {
     int64_t tty_fd = openAsync(portname, baudrate, databits);
     result.type = Dart_CObject_kInt64;
     result.value.as_int64 = tty_fd;
   }

 } else  if (strcmp("close", name) == 0) {
   int64_t tty_fd = argv[0]->value.as_int64;

   // TODO code close
   closeAsync(tty_fd);

   result.type = Dart_CObject_kBool;
   result.value.as_bool = true;
 } else  if (strcmp("send", name) == 0) {
   int64_t tty_fd = argv[0]->value.as_int64;
   const char* data = argv[1]->value.as_string;

   int value = sendAsync(tty_fd, data);

   result.type = Dart_CObject_kInt64;
   result.value.as_int64 = value;
 } else  if (strcmp("read", name) == 0) {
   int64_t tty_fd = argv[0]->value.as_int64;
   int buffer_size = (int) argv[1]->value.as_int64;
   int8_t buffer[buffer_size];
   fd_set readfs;
   FD_ZERO(&readfs);
   FD_SET(tty_fd, &readfs);
   select(tty_fd+1, &readfs, NULL, NULL, NULL);
   int n =  read(tty_fd, &buffer, sizeof(buffer));
   if(n > 0){
     result.type = Dart_CObject_kArray;
     result.value.as_array.length = n;

     for(int i=0; i<n; i++){
       Dart_CObject* v = (Dart_CObject*) malloc(sizeof(Dart_CObject_kInt32));
       v->type = Dart_CObject_kInt32;
       v->value.as_int32 = buffer[i];
       result.value.as_array.values[i] = v;
     }

    } else {
      result.type = Dart_CObject_kNull;
    }


 } else {
    // TODO
    printf("ERROR :Unknow function\n");
 }
 Dart_PostCObject(reply_port_id, &result);
}

void serialPortServicePort(Dart_NativeArguments arguments) {
  Dart_EnterScope();
  Dart_SetReturnValue(arguments, Dart_Null());
  Dart_Port service_port = Dart_NewNativePort("SerialPortServicePort", wrappedSerialPortServicePort, true);
  if (service_port != ILLEGAL_PORT) {
    Dart_Handle send_port = HandleError(Dart_NewSendPort(service_port));
    Dart_SetReturnValue(arguments, send_port);
  }
  Dart_ExitScope();
}


DART_EXPORT Dart_Handle serial_port_Init(Dart_Handle parent_library) {
  if (Dart_IsError(parent_library)) { return parent_library; }

  Dart_Handle result_code = Dart_SetNativeResolver(parent_library, ResolveName);
  if (Dart_IsError(result_code)) return result_code;


  return Dart_Null();
}

Dart_NativeFunction ResolveName(Dart_Handle name, int argc) {
  // If we fail, we return NULL, and Dart throws an exception.
  if (!Dart_IsString(name)) return NULL;
  Dart_NativeFunction result = NULL;
  Dart_EnterScope();
  const char* cname;
  HandleError(Dart_StringToCString(name, &cname));

  if (strcmp("serialPortServicePort", cname) == 0) result = serialPortServicePort;

  Dart_ExitScope();
  return result;
}

Dart_Handle HandleError(Dart_Handle handle) {
  if (Dart_IsError(handle)) Dart_PropagateError(handle);
  return handle;
}

Dart_Handle NewDartExceptionWithMessage(const char* library_url,
                                        const char* exception_name,
                                        const char* message) {
  // Create a Dart Exception object with a message.
  Dart_Handle type = Dart_GetType(Dart_LookupLibrary(
      Dart_NewStringFromCString(library_url)),
      Dart_NewStringFromCString(exception_name), 0, NULL);

  if (Dart_IsError(type)) {
    Dart_PropagateError(type);
  }
  if (message != NULL) {
    Dart_Handle args[1];
    args[0] = Dart_NewStringFromCString(message);
    if (Dart_IsError(args[0])) {
      Dart_PropagateError(args[0]);
    }
    return Dart_New(type, Dart_Null(), 1, args);
  } else {
    return Dart_New(type, Dart_Null(), 0, NULL);
  }

}
