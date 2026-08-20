#include "../../include/NodeForgePluginSDK.hpp"
#include <string>
#include <vector>

class DataCSVTransformPlugin : public nf::sdk::DataOpPlugin {
public:
    using DataOpPlugin::DataOpPlugin;

    NF_Result Cook(const NF_CookContext& ctx) override {
        std::string prefix = GetString("prefix", "ROW_");
        int32_t count = GetInt("rowCount", 5);
        if (count <= 0) count = 1;

        m_cellStorage.clear();
        m_rowPointers.clear();

        // Header row + data rows
        m_cellStorage.resize(count + 1);
        m_rowPointers.resize(count + 1);

        // Header
        m_cellStorage[0] = { "ID", "Timestamp", "Value" };
        m_cellPointers[0] = { m_cellStorage[0][0].c_str(), m_cellStorage[0][1].c_str(), m_cellStorage[0][2].c_str() };
        m_rowPointers[0] = m_cellPointers[0].data();

        // Data rows
        m_cellPointers.resize(count + 1);
        for (int32_t i = 1; i <= count; ++i) {
            m_cellStorage[i] = {
                prefix + std::to_string(i),
                std::to_string(ctx.timeSeconds),
                std::to_string(i * 1.5)
            };
            m_cellPointers[i] = { m_cellStorage[i][0].c_str(), m_cellStorage[i][1].c_str(), m_cellStorage[i][2].c_str() };
            m_rowPointers[i] = m_cellPointers[i].data();
        }

        NF_DataTableDescriptor desc{};
        desc.rowCount = static_cast<uint32_t>(count + 1);
        desc.colCount = 3;
        desc.cells = m_rowPointers.data();

        if (m_outputTables.empty()) {
            m_outputTables.resize(1);
        }
        m_outputTables[0] = desc;

        return NF_SUCCESS;
    }

private:
    std::vector<std::vector<std::string>> m_cellStorage;
    std::vector<std::vector<const char*>> m_cellPointers;
    std::vector<const char**> m_rowPointers;
};

static const NF_PinDef s_dataPins[] = {
    { "Output", NF_PIN_DIR_OUTPUT, NF_PIN_TYPE_TABLE }
};

static const NF_ParamDef s_dataParams[] = {
    { "prefix", "Row Prefix", "Table", NF_PARAM_STRING, 0.0f, 0, false, "ITEM_", 0.0f, 0.0f, 0.0f, nullptr, 0 },
    { "rowCount", "Row Count", "Table", NF_PARAM_INT, 5.0f, 5, false, nullptr, 1.0f, 100.0f, 1.0f, nullptr, 0 }
};

static const NF_PluginInfo s_dataPluginInfo = {
    "DataCSVTransformPlugin",
    "Neo Realms",
    "Tabular Data Table Generator & Transformer",
    "https://neorealms.io",
    1, 0, 0,
    NF_PLUGIN_ABI_VERSION,
    1
};

extern "C" {

NF_PLUGIN_API const NF_PluginInfo* NF_GetPluginInfo(void) {
    return &s_dataPluginInfo;
}

NF_PLUGIN_API int32_t NF_RegisterOperators(NF_OperatorDef* outOps, uint32_t maxOps) {
    if (!outOps || maxOps < 1) return 0;

    outOps[0].typeName = "Data.CSVTransformPlugin";
    outOps[0].family = NF_NODE_FAMILY_DATAOP;
    outOps[0].category = "Custom/Data";
    outOps[0].description = "Tabular Data Grid Transformer Operator";
    outOps[0].author = "Neo Realms";
    outOps[0].version = 1;
    outOps[0].pinDefs = s_dataPins;
    outOps[0].pinDefCount = 1;
    outOps[0].paramDefs = s_dataParams;
    outOps[0].paramDefCount = 2;
    outOps[0].vtable = nf::sdk::VTableBinder<DataCSVTransformPlugin>::MakeVTable();

    return 1;
}

NF_PLUGIN_API void NF_UnloadPlugin(void) {}

}
