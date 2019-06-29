// Copyright (c) 2010-2017 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGFXMGRVK_H
#define	MOAIGFXMGRVK_H

#include <moai-gfx-vk/MOAIGfxMgrVKComponents.h>
//#include <moai-gfx-vk/MOAIGfxMgrVK_PipelineClerkVK.h>
#include <moai-gfx-vk/MOAIGfxMgrVK_GPUCacheVK.h>
#include <moai-gfx-vk/MOAIGfxMgrVK_RenderTreeVK.h>
//#include <moai-gfx-vk/MOAIGfxMgrVK_ResourceClerkVK.h>
#include <moai-gfx-vk/MOAIGfxMgrVK_VertexCacheVK.h>

////================================================================//
//// MOAIGfxStateFrameVK
////================================================================//
//class MOAIGfxStateFrameVK :
//	public MOAIGfxStateCPUCacheFrame,
//	public MOAIGfxStateGPUCacheFrameVK {
//protected:
//};

//================================================================//
// MOAIGfxStateFrameVK
//================================================================//
class MOAIGfxStateFrameVK :
	public MOAIGfxStateCPUCacheFrame {
protected:
};

//================================================================//
// MOAIGfxMgrVK
//================================================================//
class MOAIGfxMgrVK :
	public ZLContextClass < MOAIGfxMgrVK >,
	public virtual MOAIGfxMgrVKComponents,
	public virtual MOAIGfxMgr,
	public virtual MOAIGfxMgrVK_GPUCacheVK,
//	public virtual MOAIGfxMgrVK_PipelineClerkVK,
	public virtual MOAIGfxMgrVK_RenderTreeVK,
//	public virtual MOAIGfxMgrVK_ResourceClerkVK,
	public virtual MOAIGfxMgrVK_VertexCacheVK {
public:

	typedef std::pair < void ( * )( VkInstance instance, VkSurfaceKHR& surface, void* userdata ), void* > HostCreateSurfaceFunc;

protected:

	HostCreateSurfaceFunc				mHostCreateSurfaceFunc;

	VkInstance                          mInstance; // Vulkan instance, stores all per-application states
    VkPhysicalDevice                    mPhysicalDevice; // Physical device (GPU) that Vulkan will use
	VkPhysicalDeviceProperties          mPhysicalDeviceProperties; // Stores physical device properties (for e.g. checking device limits)
	VkPhysicalDeviceFeatures            mPhysicalDeviceFeature; // Stores the features available on the selected physical device (for e.g. checking if a feature is available)
	VkPhysicalDeviceMemoryProperties    mPhysicalDeviceMemoryProperties; // Stores all available memory (type) properties for the physical device
//    VkFormat                            mDepthFormat; // Depth buffer format (selected during Vulkan initialization)
//
//    std::vector<const char*>            mEnabledDeviceExtensions;
	STLArray < cc8* >					mEnabledInstanceExtensions;
    VkQueue                             mQueue; // Handle to the device graphics queue that command buffers are submitted to
//    VkCommandPool                       mSwapChainQueueCommandPool; // Command buffer pool
//    std::vector<VkCommandBuffer>        mDrawCmdBuffers; // Command buffers used for rendering
//    VkRenderPass                        mRenderPass; // Global render pass for frame buffer writes
//    std::vector<VkFramebuffer>          mFrameBuffers; // List of available frame buffers (same as number of swap chain images)
//    uint32_t                            mCurrentBuffer = 0; // Active frame buffer index
//    VkDescriptorPool                    mDescriptorPool = VK_NULL_HANDLE; // Descriptor set pool
//    std::vector<VkShaderModule>         mShaderModules; // List of shader modules created (stored for cleanup)
    VkPipelineCache                     mPipelineCache; // Pipeline cache object
    MOAISwapChainVK                     mSwapChain; // Wraps the swap chain to present images (framebuffers) to the windowing system
	VkPhysicalDeviceFeatures            mEnabledFeatures; // Set of physical device features to be enabled for this example (must be set in the derived constructor). By default no phyiscal device features are enabled
//
    VkDevice                            mDevice = VK_NULL_HANDLE;                // Logical device representation (application's view of the device)
    VkCommandPool                       mGraphicsCommandPool = VK_NULL_HANDLE;   // Default command pool for the graphics queue family index

//	bool									mHasContext;
//
//	bool									mIsFramebufferSupported;
//	bool									mIsOpenGLES;
//
//	u32										mMajorVersion;
//	u32										mMinorVersion;
//
//	size_t									mTextureMemoryUsage;
//	u32										mMaxTextureSize;
//
//	u32										mRenderCounter;
//
//	ZLLeanArray < MOAIGfxStateFrameVK* >	mStateStack;
//	ZLIndex									mStateStackTop;

	//----------------------------------------------------------------//
//	static int						_enablePipelineLogging		( lua_State* L );
//	static int						_getFrameBuffer				( lua_State* L );
//	static int						_getMaxTextureSize			( lua_State* L );
//	static int						_getMaxTextureUnits			( lua_State* L );
//	static int						_getViewSize				( lua_State* L );
//	static int						_purgeResources				( lua_State* L );
//	static int						_renewResources				( lua_State* L );

	//----------------------------------------------------------------//
//	void							OnGlobalsFinalize			();
//	void							OnGlobalsInitialize			();

	//----------------------------------------------------------------//
	MOAIShader*						MOAIGfxMgr_AffirmShader						( MOAILuaState& state, int idx ) const;
	MOAITexture*					MOAIGfxMgr_AffirmTexture					( MOAILuaState& state, int idx ) const;
	MOAIImageTexture*				MOAIGfxMgr_CreateImageTexture				() const;
	MOAIIndexBuffer*				MOAIGfxMgr_CreateIndexBuffer				() const;
	MOAITexture2D*					MOAIGfxMgr_CreateTexture2D					() const;
	MOAIVertexArray*				MOAIGfxMgr_CreateVertexArray				() const;
	MOAIVertexBuffer*				MOAIGfxMgr_CreateVertexBuffer				() const;
	MOAIShader*						MOAIGfxMgr_GetShaderPreset					( MOAIShaderPresetEnum preset ) const;
	size_t							MOAIGfxMgr_GetTextureMemoryUsage			() const;
	MOAIVertexFormat*				MOAIGfxMgr_GetVertexFormatPreset			( MOAIVertexFormatPresetEnum preset ) const;
	void							MOAIGfxMgr_PopState							();
	void							MOAIGfxMgr_PushState						();
	MOAIGfxMgrVK&					MOAIGfxMgrVKComponents_GetGfxMgrVK			();
	MOAIGfxMgrVK_GPUCacheVK&		MOAIGfxMgrVKComponents_GetGPUCacheVK		();
//	MOAIGfxMgrVK_PipelineClerkVK&	MOAIGfxMgrVKComponents_GetPipelineClerkVK	();
	MOAIGfxMgrVK_RenderTreeVK&		MOAIGfxMgrVKComponents_GetRenderTreeVK		();
//	MOAIGfxMgrVK_ResourceClerkVK&	MOAIGfxMgrVKComponents_GetResourceClerkVK	();
	MOAIGfxMgrVK_VertexCacheVK&		MOAIGfxMgrVKComponents_GetVertexCacheVK		();
	void							MOAILuaObject_RegisterLuaClass				( MOAIComposer& composer, MOAILuaState& state );

public:

	DECL_LUA_SINGLETON ( MOAIGfxMgrVK )

//	GET ( u32, MaxTextureSize, mMaxTextureSize )
//	GET ( bool, HasContext, mHasContext )
//
//	GET_BOOL ( IsOpenGLES, mIsOpenGLES )
//	GET_BOOL ( IsFramebufferSupported, mIsFramebufferSupported )
//
//	GET ( u32, RenderCounter, mRenderCounter );

	GET_SET ( HostCreateSurfaceFunc, HostCreateSurfaceFunc, mHostCreateSurfaceFunc );

	//----------------------------------------------------------------//
//	void					Clear						();
//	void					ClearErrors					();
	VkResult				CreateInstance				( cc8* name, uint32_t apiVersion );
	VkResult				CreateLogicalDevice			( bool useSwapChain = true, VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT );
	void					DetectContext				( u32 width, u32 height, bool vsync );
//	void					DetectFramebuffer			();
//	void					FinishFrame					();
//	u32						LogErrors					();
							MOAIGfxMgrVK				();
							~MOAIGfxMgrVK				();
//	void					ReportTextureAlloc 			( cc8* name, size_t size );
//	void					ReportTextureFree 			( cc8* name, size_t size );
//	void					ResetDrawCount				();
	
	//----------------------------------------------------------------//
	// disambiguate
	static MOAIGfxMgrVK& Get () {
		return ZLContextClass < MOAIGfxMgrVK >::Get ();
	}
};

#endif
