#include "iec61850_server.h"
#include "hal_thread.h"
#include "goose_receiver.h"
#include "goose_subscriber.h"

#include <signal.h>
#include <stdio.h>
#include <time.h>

#include "modelBuilder.h"

#define PRINT_DEBUG

volatile sig_atomic_t running = 1;

void signal_handler(int sigID)
{
    running = 0;
}

typedef struct sInputState
{
    bool toUpdate;
    bool value;
} InputState;

static void goCbEventHandler(MmsGooseControlBlock goCb, int event, void* parameter)
{
    bool enable = MmsGooseControlBlock_getGoEna(goCb);

    printf("Goose control block '%s' : ", MmsGooseControlBlock_getName(goCb));
    printf("%s\n", enable ? "enabled" : "disabled");
}

static void gooseListener(GooseSubscriber subscriber, void* parameter)
{
    static uint32_t prev_stNum = 0;
    uint32_t stNum = GooseSubscriber_getStNum(subscriber);

    if ((stNum != prev_stNum) && GooseSubscriber_isValid(subscriber))
    {
        prev_stNum = stNum;

        MmsValue* dataSet = GooseSubscriber_getDataSetValues(subscriber);
        MmsValue* boolVal = MmsValue_getElement(dataSet, 0);

        InputState* inputState = (InputState*) parameter;

        inputState->toUpdate = true;
        inputState->value = MmsValue_getBoolean(boolVal);

#ifdef PRINT_DEBUG
        printf("Goose subscribe : %s\n", inputState->value ? "True" : "False");
#endif
    }
}

int main(int argc, char** argv)
{
    char* interface;
    if (argc > 1)
        interface = argv[1];
    else
        interface = "enp5s0";

    IedServerConfig config = IedServerConfig_create();

    IedServerConfig_enableFileService(config, false);
    IedServerConfig_enableLogService(config, false);
    IedServerConfig_enableEditSG(config, false);
    IedServerConfig_enableDynamicDataSetService(config, false);

    // Create model & server
    IedModel* model = BuildModel();

    IedServer iedServer = IedServer_createWithConfig(model, NULL, config);
    IedServerConfig_destroy(config);

    // Attibutes for update value
    DataAttribute* DA_Value_stVal = (DataAttribute*)IedModel_getModelNodeByShortObjectReference(model, "SYS/LLN0.Value.stVal");
    DataAttribute* DA_Value_t     = (DataAttribute*)IedModel_getModelNodeByShortObjectReference(model, "SYS/LLN0.Value.t");
    
    IedServer_updateBooleanAttributeValue(iedServer, DA_Value_stVal, false);
    IedServer_updateUTCTimeAttributeValue(iedServer, DA_Value_t, Hal_getTimeInMs());

    // Set goose publisher interface
    IedServer_setGooseInterfaceId(iedServer, interface);
    IedServer_enableGoosePublishing(iedServer);

    IedServer_setGoCBHandler(iedServer, goCbEventHandler, NULL);

    // Set goose Subscriber
    GooseReceiver receiver = GooseReceiver_create();
    GooseReceiver_setInterfaceId(receiver, interface);

    GooseSubscriber subscriber = GooseSubscriber_create("IED1LD1/LLN0$GO$GSE1", NULL);

    uint8_t dstMac[6] = { 0x01, 0x0c, 0xcd, 0x01, 0x01, 0xff };
    GooseSubscriber_setDstMac(subscriber, dstMac);
    GooseSubscriber_setAppId(subscriber, 0xDEAD);

    InputState gooseInput = { 0, 0 };
    GooseSubscriber_setListener(subscriber, gooseListener, &gooseInput);

    GooseReceiver_addSubscriber(receiver, subscriber);
    GooseReceiver_start(receiver);

    // Start server & GooseReciever
    int tcpPort = 102;
    IedServer_start(iedServer, tcpPort);

    if (!IedServer_isRunning(iedServer))
    {
        printf("Starting server failed! Exit\n");
        GooseReceiver_destroy(receiver);
        IedServer_destroy(iedServer);
        IedModel_destroy(model);
        exit(-1);
    }

    // Start GooseReciever
    if (!GooseReceiver_isRunning(receiver))
    {
        printf("Failed to start Goose subscriber!\n");
    }

    printf("Server started!\n");

    signal(SIGINT, signal_handler);

    // Main loop
    while (running)
    {
        // Process update data
        IedServer_lockDataModel(iedServer);

        if (gooseInput.toUpdate)
        {
            IedServer_updateBooleanAttributeValue(iedServer, DA_Value_stVal, gooseInput.value);
            IedServer_updateUTCTimeAttributeValue(iedServer, DA_Value_t, Hal_getTimeInMs());
#ifdef PRINT_DEBUG
            printf("Process input   : %s\n", gooseInput.value ? "True" : "False");
#endif
            gooseInput.toUpdate = false;
        }

        IedServer_unlockDataModel(iedServer);

        Thread_sleep(5);
    }

    // Wait a second
    Thread_sleep(1000);

    GooseReceiver_stop(receiver);
    IedServer_stop(iedServer);

    printf("\nServer stopped.\n");

    GooseReceiver_destroy(receiver);
    IedServer_destroy(iedServer);
    IedModel_destroy(model);

    return 0;
}