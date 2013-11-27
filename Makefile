all: clean build run_test

clean:
	rm -rf libserial_port.*	

build:
	dart build.dart

run_test:
	dart test/test_serial_port.dart 
