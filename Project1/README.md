# ECEN5013 Project 1

## Author - Gunj Manseta (Embedded Systems Engineering - University of Colorado Boulder)

### A multithreaded console application for BeagleBone black/green(Linux) having sensor interface, server, and logger

#### Documentation can be found at - [Project Document webpage](http://htmlpreview.github.io/?https://github.com/mansetagunj/ECEN-5013/blob/master/Project1/documentation/doxygenfiles.d/html/index.html "Documentation WebPage") (Ctrl+click for new tab)

The project includes 1 parent thread which is the main task. The main task spawns 4 child threads

1. Temperature  sensor task which is responsible for talking with the TMP102 sensor.
2. Light sensor task which is responsible for talking with the APDS9301 light sensor.

3. Socket task which is runs a TCP server and accepts connection from the client with various requests suchs as getting temperature and lux value from the sensor.
4. Logger task which is solely responsible for logging to a file setup in the config file.

The sensor tasks talks with the sensor via a MT-safe I2Cmaster wrapper written around the Intel open source project mraa library intended for various boards.

The main task which is the parent task keeps a check of the children task if they are alive or not and if any of the thread is found to be dead or stuck for more than the timeout set by in the config file, the main task closes all the threads and exits gracefully.

### This project is intended to run on the BeagleBone black/green."
