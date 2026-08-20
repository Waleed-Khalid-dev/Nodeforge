#ifndef NF_PLUGIN_ABI_H
#define NF_PLUGIN_ABI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ─── Platform Export Macros ────────────────────────────────────────────── */
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef NF_PLUGIN_BUILD_EXPORT
        #define NF_PLUGIN_API __declspec(dllexport)
    #else
        #define NF_PLUGIN_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define NF_PLUGIN_API __attribute__((visibility("default")))
    #else
        #define NF_PLUGIN_API
    #endif
#endif

/* ─── ABI Versioning ─────────────────────────────────────────────────────── */
#define NF_PLUGIN_ABI_VERSION_MAJOR 1
#define NF_PLUGIN_ABI_VERSION_MINOR 0
#define NF_PLUGIN_ABI_VERSION_PATCH 0
#define NF_PLUGIN_ABI_VERSION       ((NF_PLUGIN_ABI_VERSION_MAJOR << 16) | (NF_PLUGIN_ABI_VERSION_MINOR << 8) | NF_PLUGIN_ABI_VERSION_PATCH)

/* ─── Status Codes ───────────────────────────────────────────────────────── */
typedef enum NF_Result {
    NF_SUCCESS                =  0,
    NF_ERROR_INVALID_ARG      = -1,
    NF_ERROR_ABI_MISMATCH     = -2,
    NF_ERROR_EXECUTION_FAILED = -3,
    NF_ERROR_OUT_OF_MEMORY    = -4,
    NF_ERROR_UNSUPPORTED      = -5
} NF_Result;

/* ─── Node Families ──────────────────────────────────────────────────────── */
typedef enum NF_NodeFamily {
    NF_NODE_FAMILY_TEXOP  = 0,  /* 2D GPU Texture Operator (TOP) */
    NF_NODE_FAMILY_CHANOP = 1,  /* Control / Audio Channel Operator (CHOP) */
    NF_NODE_FAMILY_GEOMOP = 2,  /* 3D Geometry Operator (SOP) */
    NF_NODE_FAMILY_MATOP  = 3,  /* Material / Shader Operator (MAT) */
    NF_NODE_FAMILY_DATAOP = 4,  /* Table / Text / Data Operator (DAT) */
    NF_NODE_FAMILY_COMP   = 5   /* Component / Container (COMP) */
} NF_NodeFamily;

/* ─── Pin Types & Directions ─────────────────────────────────────────────── */
typedef enum NF_PinDirection {
    NF_PIN_DIR_INPUT  = 0,
    NF_PIN_DIR_OUTPUT = 1
} NF_PinDirection;

typedef enum NF_PinType {
    NF_PIN_TYPE_TEXTURE2D = 0,
    NF_PIN_TYPE_CHANNEL   = 1,
    NF_PIN_TYPE_TABLE     = 2,
    NF_PIN_TYPE_GEOMETRY  = 3,
    NF_PIN_TYPE_MATERIAL  = 4,
    NF_PIN_TYPE_GENERIC   = 5
} NF_PinType;

/* ─── Parameter Types ────────────────────────────────────────────────────── */
typedef enum NF_ParamType {
    NF_PARAM_FLOAT = 0,
    NF_PARAM_INT   = 1,
    NF_PARAM_BOOL  = 2,
    NF_PARAM_STRING= 3,
    NF_PARAM_VEC2  = 4,
    NF_PARAM_VEC3  = 5,
    NF_PARAM_VEC4  = 6,
    NF_PARAM_COLOR = 7,
    NF_PARAM_PULSE = 8,
    NF_PARAM_MENU  = 9
} NF_ParamType;

/* ─── Pin & Param Definitions ────────────────────────────────────────────── */
typedef struct NF_PinDef {
    const char*     name;
    NF_PinDirection direction;
    NF_PinType      type;
} NF_PinDef;

typedef struct NF_ParamDef {
    const char*     name;
    const char*     label;
    const char*     category;
    NF_ParamType    type;
    float           defaultFloat;
    int32_t         defaultInt;
    bool            defaultBool;
    const char*     defaultString;
    float           minVal;
    float           maxVal;
    float           stepVal;
    const char**    menuItems;
    uint32_t        menuItemCount;
} NF_ParamDef;

/* ─── Context & Descriptors ──────────────────────────────────────────────── */
typedef struct NF_CookContext {
    uint64_t frameNumber;
    double   timeSeconds;
    double   deltaTime;
    bool     isRealtime;
} NF_CookContext;

/* GPU Texture Interop (Vulkan 1.3 handles) */
typedef struct NF_VulkanTextureDescriptor {
    void*    vkDevice;          /* VkDevice */
    void*    vkCommandBuffer;   /* VkCommandBuffer active during pass */
    void*    vkImage;           /* VkImage */
    void*    vkImageView;       /* VkImageView */
    uint32_t width;
    uint32_t height;
    uint32_t format;            /* VkFormat (e.g. VK_FORMAT_R8G8B8A8_UNORM = 37, VK_FORMAT_R32G32B32A32_SFLOAT = 109) */
    uint32_t imageLayout;       /* VkImageLayout */
} NF_VulkanTextureDescriptor;

/* Channel Buffer Interop (SIMD Floats) */
typedef struct NF_ChannelBufferDescriptor {
    uint32_t        channelCount;
    uint32_t        sampleCount;
    double          sampleRate;
    const char**    channelNames;
    float**         channelData; /* Array of channelCount float pointers, each length sampleCount */
} NF_ChannelBufferDescriptor;

/* Table Data Interop (2D Grid) */
typedef struct NF_DataTableDescriptor {
    uint32_t        rowCount;
    uint32_t        colCount;
    const char***   cells;       /* [row][col] null-terminated C-strings */
} NF_DataTableDescriptor;

/* ─── Instance & VTables ─────────────────────────────────────────────────── */
typedef void* NF_PluginInstanceHandle;

typedef struct NF_NodeVTable {
    NF_PluginInstanceHandle (*createInstance)(uint64_t nodeId, const char* nodeName);
    void                    (*destroyInstance)(NF_PluginInstanceHandle instance);
    NF_Result               (*cook)(NF_PluginInstanceHandle instance, const NF_CookContext* ctx);
    void                    (*onParamChanged)(NF_PluginInstanceHandle instance, const char* paramName);
    
    /* Family-specific data exchange */
    NF_Result               (*getOutputTexture)(NF_PluginInstanceHandle instance, uint32_t pinIndex, NF_VulkanTextureDescriptor* outTex);
    NF_Result               (*setInputTexture)(NF_PluginInstanceHandle instance, uint32_t pinIndex, const NF_VulkanTextureDescriptor* inTex);
    
    NF_Result               (*getOutputChannels)(NF_PluginInstanceHandle instance, uint32_t pinIndex, NF_ChannelBufferDescriptor* outChan);
    NF_Result               (*setInputChannels)(NF_PluginInstanceHandle instance, uint32_t pinIndex, const NF_ChannelBufferDescriptor* inChan);
    
    NF_Result               (*getOutputTable)(NF_PluginInstanceHandle instance, uint32_t pinIndex, NF_DataTableDescriptor* outTable);
    NF_Result               (*setInputTable)(NF_PluginInstanceHandle instance, uint32_t pinIndex, const NF_DataTableDescriptor* inTable);

    /* Parameter sync from host */
    void                    (*setParamFloat)(NF_PluginInstanceHandle instance, const char* name, float value);
    void                    (*setParamInt)(NF_PluginInstanceHandle instance, const char* name, int32_t value);
    void                    (*setParamBool)(NF_PluginInstanceHandle instance, const char* name, bool value);
    void                    (*setParamString)(NF_PluginInstanceHandle instance, const char* name, const char* value);
} NF_NodeVTable;

/* ─── Operator & Plugin Metadata ─────────────────────────────────────────── */
typedef struct NF_OperatorDef {
    const char*         typeName;
    NF_NodeFamily       family;
    const char*         category;
    const char*         description;
    const char*         author;
    uint32_t            version;
    const NF_PinDef*    pinDefs;
    uint32_t            pinDefCount;
    const NF_ParamDef*  paramDefs;
    uint32_t            paramDefCount;
    NF_NodeVTable       vtable;
} NF_OperatorDef;

typedef struct NF_PluginInfo {
    const char*         pluginName;
    const char*         author;
    const char*         description;
    const char*         url;
    uint32_t            versionMajor;
    uint32_t            versionMinor;
    uint32_t            versionPatch;
    uint32_t            abiVersion;
    uint32_t            operatorCount;
} NF_PluginInfo;

/* ─── Mandatory Export Entrypoints ───────────────────────────────────────── */
typedef const NF_PluginInfo* (*NF_GetPluginInfo_Fn)(void);
typedef int32_t              (*NF_RegisterOperators_Fn)(NF_OperatorDef* outOperators, uint32_t maxOperators);
typedef void                 (*NF_UnloadPlugin_Fn)(void);

#define NF_ENTRYPOINT_GET_PLUGIN_INFO    "NF_GetPluginInfo"
#define NF_ENTRYPOINT_REGISTER_OPERATORS "NF_RegisterOperators"
#define NF_ENTRYPOINT_UNLOAD_PLUGIN      "NF_UnloadPlugin"

#ifdef __cplusplus
}
#endif

#endif /* NF_PLUGIN_ABI_H */
