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
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<ChannelBuffer>()) {
                    const auto& buf = pin->GetValue().Get<ChannelBuffer>();
                    return buf.GetSample(chanName, 0);
                }
            }
            return 0.0f;
        })
        .def("chan", [](Node* self, const std::string& chanName) -> float {
            for (const auto& pin : self->GetOutputPins()) {
                if (pin->GetValue().Is<ChannelBuffer>()) {
                    const auto& buf = pin->GetValue().Get<ChannelBuffer>();
                    return buf.GetSample(chanName, 0);
                }
            }
            return 0.0f;
        });

    // Bind ChannelBuffer
    py::class_<ChannelBuffer>(m, "ChannelBuffer")
        .def(py::init<>())
        .def_property_readonly("num_channels", &ChannelBuffer::GetChannelCount)
        .def_property_readonly("num_samples", &ChannelBuffer::GetSampleCount)
        .def_property_readonly("sample_rate", &ChannelBuffer::GetSampleRate)
        .def_property_readonly("channel_names", &ChannelBuffer::GetChannelNames)
        .def("get_sample", [](const ChannelBuffer& self, const std::string& name, size_t sampleIdx) {
            return self.GetSample(name, sampleIdx);
        });

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
