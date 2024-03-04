# iecServer
Simple iec61850 server
- Dynamic data model
- DataSet + Reports
- Goose Publisher
- Goose Subscriber

## Requirements
``` bash
git clone https://github.com/mz-automation/libiec61850.git
```

## Build
``` bash
./build.sh
```

## Run
``` bash
sudo ./build/iecServer eth0
```

## Check memory leaks
``` bash
sudo valgrind --tool=memcheck ./build/iecServer eth0
```

## Clean
``` bash
rm -rf build
```
