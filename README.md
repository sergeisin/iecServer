# iecServer
Simple iec61850 server based on libiec61850 v1.5.3
- Dynamic data model
- DataSet + Reports
- Goose Publisher
- Goose Subscriber

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
