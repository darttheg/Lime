#include <irrlicht.h>

using namespace irr;
using namespace video;

class ShaderCallback : public video::IShaderConstantSetCallBack
{
public:
    ShaderCallback(bool hl, bool cg, IrrlichtDevice* d) : useHighLevelShaders(hl), useCGShaders(cg), device(d) {}

    bool useHighLevelShaders = false;
    bool useCGShaders = false;
    IrrlichtDevice* device = 0;

    virtual void OnSetConstants(video::IMaterialRendererServices* services,
        s32 userData)
    {
        video::IVideoDriver* driver = services->getVideoDriver();

        core::matrix4 invWorld = driver->getTransform(video::ETS_WORLD);
        invWorld.makeInverse();

        if (useHighLevelShaders)
            services->setVertexShaderConstant("mInvWorld", invWorld.pointer(), 16);
        else
            services->setVertexShaderConstant(invWorld.pointer(), 0, 4);

        core::matrix4 worldViewProj;
        worldViewProj = driver->getTransform(video::ETS_PROJECTION);
        worldViewProj *= driver->getTransform(video::ETS_VIEW);
        worldViewProj *= driver->getTransform(video::ETS_WORLD);

        if (useHighLevelShaders)
            services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
        else
            services->setVertexShaderConstant(worldViewProj.pointer(), 4, 4);

        core::vector3df pos = device->getSceneManager()->
            getActiveCamera()->getAbsolutePosition();

        if (useHighLevelShaders)
            services->setVertexShaderConstant("mLightPos", reinterpret_cast<f32*>(&pos), 3);
        else
            services->setVertexShaderConstant(reinterpret_cast<f32*>(&pos), 8, 1);

        video::SColorf col(0.0f, 1.0f, 1.0f, 0.0f);

        if (useHighLevelShaders)
            services->setVertexShaderConstant("mLightColor",
                reinterpret_cast<f32*>(&col), 4);
        else
            services->setVertexShaderConstant(reinterpret_cast<f32*>(&col), 9, 1);

        core::matrix4 world = driver->getTransform(video::ETS_WORLD);
        world = world.getTransposed();

        if (useHighLevelShaders)
        {
            services->setVertexShaderConstant("mTransWorld", world.pointer(), 16);

            s32 TextureLayerID = 0;
            services->setPixelShaderConstant("myTexture", &TextureLayerID, 1);
        }
        else
            services->setVertexShaderConstant(world.pointer(), 10, 4);
    }
};

// Thanks Irrlicht Example 10