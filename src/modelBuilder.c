#include "modelBuilder.h"

IedModel* BuildModel()
{
    IedModel* model = IedModel_create("IED1_");

    LogicalDevice* sysLD = LogicalDevice_create("SYS", model);

    LogicalNode* lln0 = LogicalNode_create("LLN0", sysLD);

    DataObject* DO_Value  = CDC_SPS_create("Value",  (ModelNode*) lln0, 0);

    // DataSets
    DataSet* dataSet_DA = DataSet_create("DataSet_DA", lln0);
    DataSetEntry_create(dataSet_DA, "LLN0$ST$Value$stVal", -1, NULL);
    DataSetEntry_create(dataSet_DA, "LLN0$ST$Value$q",     -1, NULL);
    DataSetEntry_create(dataSet_DA, "LLN0$ST$Value$t",     -1, NULL);

    DataSet* dataSet_DO = DataSet_create("DataSet_DO", lln0);
    DataSetEntry_create(dataSet_DO, "LLN0$ST$Value",       -1, NULL);

    // Report triggets
    uint8_t trgOptions = TRG_OPT_DATA_CHANGED | TRG_OPT_QUALITY_CHANGED | TRG_OPT_GI;

    // All options set
    uint8_t rptOptions = 0xFF;

    ReportControlBlock_create("Rpt_DO", lln0, "Report_DO", false, "DataSet_DO", 1000 , trgOptions, rptOptions, 0, 5000);
    ReportControlBlock_create("Rpt_DA", lln0, "Report_DA", false, "DataSet_DA", 1000 , trgOptions, rptOptions, 0, 5000);

    // Goose settings
    uint8_t vlanPriority = 4;       // Max -    0x7
    uint16_t vlanId = 5;            // Max -  0xFFF
    uint16_t appID = 0xDEAD;        // Max - 0xFFFF

    uint8_t dstMac[6] = { 0x01, 0x0c, 0xcd, 0x01, 0x01, 0xff };

    PhyComAddress* ptyAddr = PhyComAddress_create(vlanPriority, vlanId, appID, dstMac);

    GSEControlBlock* gse = GSEControlBlock_create("GSE", lln0, "Goose_ID", "DataSet_DO", 1000, false, 100, 1000);

    GSEControlBlock_addPhyComAddress(gse, ptyAddr);

    return model;
}