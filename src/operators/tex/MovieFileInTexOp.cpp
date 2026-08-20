#include "MovieFileInTexOp.h"

namespace nf {

MovieFileInTexOp::MovieFileInTexOp(NodeId id, const std::string& name)
    : TexOp(id, name, "MovieFileInTexOp") {
    m_outPin = AddOutputPin("output", PinType::Tex);

    SetParam("file_path", std::string("assets/video/sample.mp4"));
    SetParam("play", true);
    SetParam("speed", 1.0f);
    SetParam("loop_mode", static_cast<int32_t>(0)); // 0: Loop, 1: Once, 2: Ping-Pong
    SetParam("index_type", static_cast<int32_t>(0)); // 0: Time, 1: Frame, 2: Normalized
    SetParam("frame_index", static_cast<int32_t>(0));
}

bool MovieFileInTexOp::Cook(const CookContext& context) {
    std::string path = GetParam("file_path").Is<std::string>() ? GetParam("file_path").Get<std::string>() : "";
    bool play = GetParam("play").Is<bool>() ? GetParam("play").Get<bool>() : true;
    float speed = GetParam("speed").Is<float>() ? GetParam("speed").Get<float>() : 1.0f;
    int32_t loopMode = GetParam("loop_mode").Is<int32_t>() ? GetParam("loop_mode").Get<int32_t>() : 0;
    int32_t indexType = GetParam("index_type").Is<int32_t>() ? GetParam("index_type").Get<int32_t>() : 0;

    if (path != m_loadedPath) {
        m_decoder.OpenFile(path);
        m_loadedPath = path;
    }

    if (play) {
        m_decoder.Play();
    } else {
        m_decoder.Pause();
    }

    m_decoder.SetSpeed(speed);
    m_decoder.SetLoopMode(static_cast<VideoLoopMode>(loopMode));

    if (indexType == 1) { // Explicit frame index
        int32_t fIdx = GetParam("frame_index").Is<int32_t>() ? GetParam("frame_index").Get<int32_t>() : 0;
        m_decoder.SeekFrame(static_cast<uint64_t>(std::max(0, fIdx)));
    }

    VideoFrame frame;
    if (m_decoder.GetFrame(context.timeSeconds, frame)) {
        if (context.gpuDevice) {
            auto tex = std::make_shared<gpu::Texture2D>(context.gpuDevice);
            if (tex->UploadData(frame.rgbaPixels.data(), frame.width, frame.height, frame.rgbaPixels.size())) {
                SetOutputTexture(tex);
            }
        }
    }

    return true;
}

} // namespace nf
