#include <gtest/gtest.h>
#include "graph/Graph.h"
#include "graph/CoreNodes.h"
#include "graph/NodeRegistry.h"
#include "python/PythonEngine.h"
#include "operators/data/TextDataOp.h"
#include "operators/data/TableDataOp.h"
#include "operators/data/ScriptDataOp.h"
#include "operators/data/JSONDataOp.h"
#include "operators/data/SelectDataOp.h"
#include "operators/data/MergeDataOp.h"
#include "operators/data/OSCInOp.h"
#include "operators/data/OSCOutOp.h"
#include "operators/data/WebDataOp.h"
#include "operators/data/ChanToDataOp.h"
#include "operators/data/DataToChanOp.h"
#include "operators/chan/ConstantChanOp.h"

using namespace nf;

class DataOpPipelineTest : public ::testing::Test {
protected:
    void SetUp() override {
        RegisterCoreNodes(NodeRegistry::Instance());
        PythonEngine::Instance().Initialize();
    }
};

TEST_F(DataOpPipelineTest, TextAndTableDataOpCook) {
    TextDataOp textOp(1, "text1");
    textOp.SetParam("text", std::string("// Vertex Shader\nvoid main() {}"));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(textOp.Cook(ctx));

    const DataTable* tOut = textOp.GetOutputTable();
    ASSERT_NE(tOut, nullptr);
    EXPECT_EQ(tOut->GetRowCount(), 2);
    EXPECT_EQ(tOut->GetCell(0, 0), "// Vertex Shader");

    TableDataOp tableOp(2, "table1");
    tableOp.SetParam("raw_csv", std::string("chan,min,max\ntx,-10,10\nty,-20,20"));
    EXPECT_TRUE(tableOp.Cook(ctx));

    const DataTable* tbOut = tableOp.GetOutputTable();
    ASSERT_NE(tbOut, nullptr);
    EXPECT_EQ(tbOut->GetRowCount(), 2);
    EXPECT_EQ(tbOut->GetColumnCount(), 3);
    EXPECT_EQ(tbOut->GetCell(0, 0), "tx");
    EXPECT_FLOAT_EQ(tbOut->GetCellFloat(0, 1), -10.0f);
}

TEST_F(DataOpPipelineTest, ScriptDataOpPythonExecution) {
    ScriptDataOp scriptOp(3, "script1");
    std::string code =
        "def onCook(dat):\n"
        "    dat.clear()\n"
        "    dat.append_row(['name', 'score'])\n"
        "    dat.append_row(['alpha', '100'])\n"
        "    dat.append_row(['beta', '200'])\n";
    scriptOp.SetParam("script", code);

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(scriptOp.Cook(ctx));

    const DataTable* sOut = scriptOp.GetOutputTable();
    ASSERT_NE(sOut, nullptr);
    EXPECT_EQ(sOut->GetRowCount(), 3);
    EXPECT_EQ(sOut->GetCell(1, 0), "alpha");
    EXPECT_EQ(sOut->GetCell(1, 1), "100");
}

TEST_F(DataOpPipelineTest, JSONDataOpPointerQuery) {
    JSONDataOp jsonOp(4, "json1");
    std::string jsonStr = "{\"projector\": {\"ip\": \"192.168.1.50\", \"port\": 4352, \"status\": \"online\"}}";
    jsonOp.SetParam("json_text", jsonStr);
    jsonOp.SetParam("json_pointer", std::string("/projector"));
    jsonOp.SetParam("mode", static_cast<int32_t>(1)); // Key-Value

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(jsonOp.Cook(ctx));

    const DataTable* jOut = jsonOp.GetOutputTable();
    ASSERT_NE(jOut, nullptr);
    EXPECT_GT(jOut->GetRowCount(), 0);
}

TEST_F(DataOpPipelineTest, SelectAndMergeDataOp) {
    Graph graph;
    auto* t1 = graph.CreateNode<TableDataOp>("t1");
    auto* t2 = graph.CreateNode<TableDataOp>("t2");
    auto* merge = graph.CreateNode<MergeDataOp>("merge1");
    auto* select = graph.CreateNode<SelectDataOp>("select1");

    t1->SetParam("raw_csv", std::string("name,x,y\np1,1,2\np2,3,4"));
    t2->SetParam("raw_csv", std::string("name,x,y\np3,5,6\np4,7,8"));

    graph.Connect(t1->GetOutputPin("output"), merge->GetInputPin("input1"));
    graph.Connect(t2->GetOutputPin("output"), merge->GetInputPin("input2"));
    graph.Connect(merge->GetOutputPin("output"), select->GetInputPin("input"));

    merge->SetParam("mode", static_cast<int32_t>(0)); // Append Rows
    select->SetParam("col_names", std::string("name y"));
    select->SetParam("start_row", static_cast<int32_t>(1));
    select->SetParam("num_rows", static_cast<int32_t>(2));

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const DataTable* sOut = select->GetOutputTable();
    ASSERT_NE(sOut, nullptr);
    EXPECT_EQ(sOut->GetRowCount(), 2);
    EXPECT_EQ(sOut->GetColumnCount(), 2);
    EXPECT_EQ(sOut->GetColumnHeaders()[0], "name");
    EXPECT_EQ(sOut->GetColumnHeaders()[1], "y");
}

TEST_F(DataOpPipelineTest, ChanToDataAndDataToChanBridges) {
    Graph graph;
    auto* constChan = graph.CreateNode<ConstantChanOp>("constChan1");
    auto* chopToDat = graph.CreateNode<ChanToDataOp>("chopToDat1");
    auto* datToChop = graph.CreateNode<DataToChanOp>("datToChop1");

    constChan->SetParam("channel_names", std::string("tx ty"));
    constChan->SetParam("values", glm::vec4(12.5f, 25.0f, 0.0f, 0.0f));
    constChan->SetParam("sample_count", static_cast<int32_t>(4));

    graph.Connect(constChan->GetOutputPin("output"), chopToDat->GetInputPin("input"));
    graph.Connect(chopToDat->GetOutputPin("output"), datToChop->GetInputPin("input"));

    chopToDat->SetParam("include_index", false);
    datToChop->SetParam("layout", static_cast<int32_t>(0)); // Columns are channels

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(graph.CookAll(ctx));

    const DataTable* datOut = chopToDat->GetOutputTable();
    ASSERT_NE(datOut, nullptr);
    EXPECT_EQ(datOut->GetRowCount(), 4);
    EXPECT_EQ(datOut->GetColumnCount(), 2);

    const ChannelBuffer* chanOut = datToChop->GetOutputBuffer();
    ASSERT_NE(chanOut, nullptr);
    EXPECT_EQ(chanOut->GetChannelCount(), 2);
    EXPECT_EQ(chanOut->GetSampleCount(), 4);
    EXPECT_FLOAT_EQ(chanOut->GetSample(0, 0), 12.5f);
    EXPECT_FLOAT_EQ(chanOut->GetSample(1, 3), 25.0f);
}

TEST_F(DataOpPipelineTest, OSCAndWebDataOpNonBlocking) {
    OSCInOp oscIn(5, "oscIn1");
    oscIn.SetParam("active", false); // Headless non-blocking test

    CookContext ctx{ .frameIndex = 1 };
    EXPECT_TRUE(oscIn.Cook(ctx));
    EXPECT_EQ(oscIn.GetOutputTable()->GetColumnCount(), 3);

    WebDataOp web(6, "web1");
    web.SetParam("auto_fetch", false);
    EXPECT_TRUE(web.Cook(ctx));
    EXPECT_EQ(web.GetOutputTable()->GetColumnCount(), 3);
}
