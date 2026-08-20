#include "PyNodeForge.h"
#include "../graph/Graph.h"
#include "../graph/Node.h"
#include "../graph/NodeRegistry.h"
#include "../param/Parameter.h"
#include "PythonEngine.h"
#include "../operators/chan/ConstantChanOp.h"
#include "../operators/chan/MathChanOp.h"
#include "../operators/tex/NullTexOp.h"
#include "../operators/tex/ConstantTexOp.h"
#include "../operators/tex/NoiseTexOp.h"
#include "../operators/tex/LoadImageTexOp.h"
#include "../operators/tex/TransformTexOp.h"
#include "../operators/tex/CompositeTexOp.h"
#include "../operators/tex/BlurTexOp.h"
#include "../operators/tex/LevelTexOp.h"
#include "../operators/tex/ResolutionTexOp.h"
#include "../operators/tex/ToWindowTexOp.h"
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;

namespace nf {

class PyParAccessor {
public:
    PyParAccessor(Node* node) : m_node(node) {}

    py::object GetAttr(const std::string& name) {
        if (!m_node) throw py::attribute_error("Node is null");
        Parameter* p = m_node->GetParams().Get(name);
        if (!p) throw py::attribute_error("Parameter '" + name + "' not found on node '" + m_node->GetName() + "'");
        return py::cast(p, py::return_value_policy::reference);
    }

    void SetAttr(const std::string& name, py::handle value) {
        if (!m_node) throw py::attribute_error("Node is null");
        Parameter* p = m_node->GetParams().Get(name);
        if (!p) throw py::attribute_error("Parameter '" + name + "' not found on node '" + m_node->GetName() + "'");

        if (py::isinstance<py::float_>(value)) {
            p->SetValue(PinValue(value.cast<float>()));
        } else if (py::isinstance<py::int_>(value)) {
            p->SetValue(PinValue(value.cast<int32_t>()));
        } else if (py::isinstance<py::bool_>(value)) {
            p->SetValue(PinValue(value.cast<bool>()));
        } else if (py::isinstance<py::str>(value)) {
            p->SetValue(PinValue(value.cast<std::string>()));
        } else if (py::isinstance<py::tuple>(value) || py::isinstance<py::list>(value)) {
            auto seq = value.cast<py::sequence>();
            if (seq.size() == 2) {
                p->SetValue(PinValue(glm::vec2(seq[0].cast<float>(), seq[1].cast<float>())));
            } else if (seq.size() == 3) {
                p->SetValue(PinValue(glm::vec3(seq[0].cast<float>(), seq[1].cast<float>(), seq[2].cast<float>())));
            } else if (seq.size() >= 4) {
                p->SetValue(PinValue(glm::vec4(seq[0].cast<float>(), seq[1].cast<float>(), seq[2].cast<float>(), seq[3].cast<float>())));
            }
        }
    }

private:
    Node* m_node = nullptr;
};

PYBIND11_EMBEDDED_MODULE(nodeforge, m) {
    m.doc() = "NodeForge real-time generative engine Python API";

    // Bind Parameter
    py::class_<Parameter>(m, "Parameter")
        .def_property_readonly("name", &Parameter::GetName)
        .def_property_readonly("label", &Parameter::GetLabel)
        .def_property_readonly("page", &Parameter::GetPage)
        .def_property("expr", &Parameter::GetExpression, &Parameter::SetExpression)
        .def_property("mode", &Parameter::GetMode, &Parameter::SetMode)
        .def("eval", [](const Parameter& p) -> py::object {
            const auto& v = p.GetValue();
            if (v.Is<float>()) return py::cast(v.Get<float>());
            if (v.Is<int32_t>()) return py::cast(v.Get<int32_t>());
            if (v.Is<bool>()) return py::cast(v.Get<bool>());
            if (v.Is<std::string>()) return py::cast(v.Get<std::string>());
            if (v.Is<glm::vec2>()) { auto vec = v.Get<glm::vec2>(); return py::make_tuple(vec.x, vec.y); }
            if (v.Is<glm::vec3>()) { auto vec = v.Get<glm::vec3>(); return py::make_tuple(vec.x, vec.y, vec.z); }
            if (v.Is<glm::vec4>()) { auto vec = v.Get<glm::vec4>(); return py::make_tuple(vec.x, vec.y, vec.z, vec.w); }
            return py::none();
        })
        .def("as_float", &Parameter::AsFloat)
        .def("as_int", &Parameter::AsInt)
        .def("as_bool", &Parameter::AsBool)
        .def("as_str", &Parameter::AsString);

    // Bind PyParAccessor
    py::class_<PyParAccessor>(m, "ParAccessor")
        .def("__getattr__", &PyParAccessor::GetAttr)
        .def("__setattr__", &PyParAccessor::SetAttr);

    // Bind Node
    py::class_<Node>(m, "Node")
        .def_property_readonly("id", &Node::GetId)
        .def_property_readonly("name", &Node::GetName)
        .def_property_readonly("type_name", &Node::GetTypeName)
        .def_property_readonly("is_dirty", &Node::IsDirty)
        .def_property_readonly("par", [](Node* self) {
            return PyParAccessor(self);
        })
        .def("get_param", [](Node* self, const std::string& name) -> py::object {
            Parameter* p = self->GetParams().Get(name);
            if (!p) return py::none();
            return py::cast(p, py::return_value_policy::reference);
        })
        .def("set_param", [](Node* self, const std::string& name, py::handle value) {
            PyParAccessor acc(self);
            acc.SetAttr(name, value);
        })
        .def("cook", [](Node* self) {
            CookContext ctx{ .frameIndex = 1 };
            return self->EnsureCooked(ctx);
        })
        .def("__getitem__", [](Node* self, const std::string& chanName) -> float {
            if (!self) return 0.0f;
            CookContext ctx{ .frameIndex = 1 };
            self->EnsureCooked(ctx);
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<ChannelBuffer>()) {
                    const auto& buf = pin->GetValue().Get<ChannelBuffer>();
                    return buf.GetSample(chanName, 0);
                }
            }
            return 0.0f;
        })
        .def("chan", [](Node* self, const std::string& chanName) -> float {
            if (!self) return 0.0f;
            CookContext ctx{ .frameIndex = 1 };
            self->EnsureCooked(ctx);
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<ChannelBuffer>()) {
                    const auto& buf = pin->GetValue().Get<ChannelBuffer>();
                    return buf.GetSample(chanName, 0);
                }
            }
            return 0.0f;
        })
        .def_property_readonly("table", [](Node* self) -> py::object {
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<DataTable>()) {
                    return py::cast(pin->GetValue().Get<DataTable>());
                }
            }
            return py::none();
        })
        .def_property_readonly("geometry", [](Node* self) -> py::object {
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<GeometryData>()) {
                    return py::cast(pin->GetValue().Get<GeometryData>());
                }
            }
            return py::none();
        })
        .def("cell", [](Node* self, size_t row, size_t col) -> std::string {
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<DataTable>()) {
                    const auto& dt = pin->GetValue().Get<DataTable>();
                    return dt.GetCell(row, col);
                }
            }
            return "";
        });

    // Bind ChannelBuffer
    py::class_<ChannelBuffer>(m, "ChannelBuffer")
        .def(py::init<>())
        .def_property_readonly("num_channels", &ChannelBuffer::GetChannelCount)
        .def_property_readonly("num_samples", &ChannelBuffer::GetSampleCount)
        .def_property_readonly("rate", &ChannelBuffer::GetSampleRate)
        .def_property_readonly("channel_names", &ChannelBuffer::GetChannelNames)
        .def("sample", py::overload_cast<size_t, size_t>(&ChannelBuffer::GetSample, py::const_))
        .def("sample", py::overload_cast<const std::string&, size_t>(&ChannelBuffer::GetSample, py::const_), py::arg("name"), py::arg("sampleIdx") = 0)
        .def("__getitem__", [](const ChannelBuffer& buf, const std::string& name) -> float {
            return buf.GetSample(name, 0);
        });

    // Bind DataTable
    py::class_<DataTable>(m, "DataTable")
        .def(py::init<>())
        .def_property_readonly("num_rows", &DataTable::GetRowCount)
        .def_property_readonly("num_cols", &DataTable::GetColumnCount)
        .def_property_readonly("headers", &DataTable::GetColumnHeaders)
        .def("cell", py::overload_cast<size_t, size_t>(&DataTable::GetCell, py::const_))
        .def("cell", py::overload_cast<const std::string&, const std::string&>(&DataTable::GetCell, py::const_))
        .def("set_cell", py::overload_cast<size_t, size_t, const std::string&>(&DataTable::SetCell))
        .def("set_cell", py::overload_cast<const std::string&, const std::string&, const std::string&>(&DataTable::SetCell))
        .def("cell_float", &DataTable::GetCellFloat, py::arg("row"), py::arg("col"), py::arg("default") = 0.0f)
        .def("cell_int", &DataTable::GetCellInt, py::arg("row"), py::arg("col"), py::arg("default") = 0)
        .def("cell_bool", &DataTable::GetCellBool, py::arg("row"), py::arg("col"), py::arg("default") = false)
        .def("append_row", &DataTable::AppendRow)
        .def("append_col", &DataTable::AppendColumn, py::arg("header"), py::arg("columnData") = std::vector<std::string>{})
        .def("clear", &DataTable::Clear)
        .def("to_csv", &DataTable::ToCSV, py::arg("delimiter") = ',', py::arg("includeHeaders") = true)
        .def("to_text", &DataTable::ToText)
        .def("to_json", &DataTable::ToJSON, py::arg("pretty") = true)
        .def("__getitem__", [](const DataTable& dt, py::handle key) -> std::string {
            if (py::isinstance<py::tuple>(key)) {
                auto tup = key.cast<py::tuple>();
                if (tup.size() == 2) {
                    if (py::isinstance<py::int_>(tup[0]) && py::isinstance<py::int_>(tup[1])) {
                        return dt.GetCell(tup[0].cast<size_t>(), tup[1].cast<size_t>());
                    } else if (py::isinstance<py::str>(tup[0]) && py::isinstance<py::str>(tup[1])) {
                        return dt.GetCell(tup[0].cast<std::string>(), tup[1].cast<std::string>());
                    }
                }
            } else if (py::isinstance<py::int_>(key)) {
                return dt.GetCell(key.cast<size_t>(), 0);
            }
            return "";
        });

    // Bind Vertex
    py::class_<Vertex>(m, "Vertex")
        .def(py::init<>())
        .def_property("pos", [](const Vertex& v) { return py::make_tuple(v.pos.x, v.pos.y, v.pos.z); }, [](Vertex& v, py::tuple t) { if (t.size() >= 3) v.pos = glm::vec3(t[0].cast<float>(), t[1].cast<float>(), t[2].cast<float>()); })
        .def_property("normal", [](const Vertex& v) { return py::make_tuple(v.normal.x, v.normal.y, v.normal.z); }, [](Vertex& v, py::tuple t) { if (t.size() >= 3) v.normal = glm::vec3(t[0].cast<float>(), t[1].cast<float>(), t[2].cast<float>()); })
        .def_property("uv", [](const Vertex& v) { return py::make_tuple(v.uv.x, v.uv.y); }, [](Vertex& v, py::tuple t) { if (t.size() >= 2) v.uv = glm::vec2(t[0].cast<float>(), t[1].cast<float>()); })
        .def_property("color", [](const Vertex& v) { return py::make_tuple(v.color.r, v.color.g, v.color.b, v.color.a); }, [](Vertex& v, py::tuple t) { if (t.size() >= 4) v.color = glm::vec4(t[0].cast<float>(), t[1].cast<float>(), t[2].cast<float>(), t[3].cast<float>()); })
        .def_property("tangent", [](const Vertex& v) { return py::make_tuple(v.tangent.x, v.tangent.y, v.tangent.z, v.tangent.w); }, [](Vertex& v, py::tuple t) { if (t.size() >= 4) v.tangent = glm::vec4(t[0].cast<float>(), t[1].cast<float>(), t[2].cast<float>(), t[3].cast<float>()); });

    // Bind GeometryData
    py::class_<GeometryData>(m, "GeometryData")
        .def(py::init<>())
        .def_property_readonly("num_vertices", &GeometryData::GetVertexCount)
        .def_property_readonly("num_indices", &GeometryData::GetIndexCount)
        .def_property_readonly("num_triangles", &GeometryData::GetTriangleCount)
        .def_property_readonly("num_instances", &GeometryData::GetInstanceCount)
        .def_static("create_grid", &GeometryData::CreateGrid, py::arg("size_x") = 1.0f, py::arg("size_y") = 1.0f, py::arg("rows") = 10, py::arg("cols") = 10, py::arg("plane") = 0)
        .def_static("create_sphere", &GeometryData::CreateSphere, py::arg("radius") = 1.0f, py::arg("rings") = 16, py::arg("segments") = 32)
        .def_static("create_box", &GeometryData::CreateBox, py::arg("size_x") = 1.0f, py::arg("size_y") = 1.0f, py::arg("size_z") = 1.0f, py::arg("divs_x") = 1, py::arg("divs_y") = 1, py::arg("divs_z") = 1)
        .def_static("create_torus", &GeometryData::CreateTorus, py::arg("major_radius") = 1.0f, py::arg("minor_radius") = 0.3f, py::arg("major_segments") = 32, py::arg("minor_segments") = 16)
        .def_static("create_cylinder", &GeometryData::CreateCylinder, py::arg("height") = 2.0f, py::arg("radius_bottom") = 1.0f, py::arg("radius_top") = 1.0f, py::arg("segments") = 32, py::arg("cap_bottom") = true, py::arg("cap_top") = true)
        .def("merge", &GeometryData::Merge)
        .def("compute_normals", &GeometryData::ComputeNormals, py::arg("smooth") = true)
        .def("compute_tangents", &GeometryData::ComputeTangents)
        .def("deform_noise", [](GeometryData& self, float amp, float freq, float ox, float oy, float oz, bool alongNorm) {
            self.DeformNoise(amp, freq, glm::vec3(ox, oy, oz), alongNorm);
        }, py::arg("amplitude") = 0.2f, py::arg("frequency") = 1.0f, py::arg("offset_x") = 0.0f, py::arg("offset_y") = 0.0f, py::arg("offset_z") = 0.0f, py::arg("along_normal") = true)
        .def("clear", &GeometryData::Clear);

    py::class_<ConstantChanOp, Node>(m, "ConstantChanOp");
    py::class_<MathChanOp, Node>(m, "MathChanOp");
    py::class_<NullTexOp, Node>(m, "NullTexOp");
    py::class_<ConstantTexOp, Node>(m, "ConstantTexOp");
    py::class_<NoiseTexOp, Node>(m, "NoiseTexOp");
    py::class_<LoadImageTexOp, Node>(m, "LoadImageTexOp");
    py::class_<TransformTexOp, Node>(m, "TransformTexOp");
    py::class_<CompositeTexOp, Node>(m, "CompositeTexOp");
    py::class_<BlurTexOp, Node>(m, "BlurTexOp");
    py::class_<LevelTexOp, Node>(m, "LevelTexOp");
    py::class_<ResolutionTexOp, Node>(m, "ResolutionTexOp");
    py::class_<ToWindowTexOp, Node>(m, "ToWindowTexOp");

    // Module functions
    m.def("op", [](const std::string& name) -> Node* {
        Graph* g = PythonEngine::Instance().GetActiveGraph();
        if (!g) return nullptr;
        return g->FindNode(name);
    }, py::return_value_policy::reference);

    m.def("create_node", [](const std::string& typeName, const std::string& name) -> Node* {
        Graph* g = PythonEngine::Instance().GetActiveGraph();
        if (!g) throw std::runtime_error("No active graph set");
        NodeId id = g->GenerateNodeId();
        auto node = NodeRegistry::Instance().CreateNode(typeName, id, name);
        if (!node) throw std::runtime_error("Unknown node type: " + typeName);
        return g->AddNode(std::move(node));
    }, py::return_value_policy::reference);

    m.def("connect", [](Node* fromNode, const std::string& fromPinName, Node* toNode, const std::string& toPinName) -> bool {
        Graph* g = PythonEngine::Instance().GetActiveGraph();
        if (!g || !fromNode || !toNode) return false;
        Pin* fromPin = fromNode->GetOutputPin(fromPinName);
        Pin* toPin = toNode->GetInputPin(toPinName);
        if (!fromPin || !toPin) return false;
        std::string err;
        return g->Connect(fromPin, toPin, &err) != nullptr;
    });

    m.def("disconnect", [](Node* toNode, const std::string& toPinName) -> bool {
        Graph* g = PythonEngine::Instance().GetActiveGraph();
        if (!g || !toNode) return false;
        Pin* toPin = toNode->GetInputPin(toPinName);
        if (!toPin) return false;
        return g->Disconnect(toPin);
    });

    m.def("get_nodes", []() -> std::vector<Node*> {
        std::vector<Node*> result;
        Graph* g = PythonEngine::Instance().GetActiveGraph();
        if (g) {
            for (const auto& [id, node] : g->GetNodes()) {
                result.push_back(node.get());
            }
        }
        return result;
    }, py::return_value_policy::reference);
}

static volatile int g_pyNodeForgeForceLink = 0;

void RegisterPyNodeForgeModule() {
    g_pyNodeForgeForceLink = 42;
}

} // namespace nf
