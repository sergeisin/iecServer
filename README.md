# iecServer
Simple iec61850 server
- Dynamic data model
- DataSet + Reports
- Goose Publisher
- Goose Subscriber

## Dependencies
- libiec61850 v1.5.3

## Build
``` bash
./build.sh
```

## Run
``` bash
sudo ./build/iecServer eth0
```

## Clean
``` bash
rm -rf build
```

## Check memory leaks
``` bash
sudo valgrind --tool=memcheck ./build/iecServer eth0
```
