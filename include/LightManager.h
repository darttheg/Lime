#pragma once

#include "irrlicht.h"
using namespace irr;
using namespace scene;
using namespace core;
using namespace video;

// Light Manager Example by Colin MacDonald

class CLightManager : public scene::ILightManager
{
    typedef enum
    {
        NO_MANAGEMENT,
        LIGHTS_NEAREST_NODE,
        LIGHTS_IN_ZONE
    }
    LightManagementMode;

    LightManagementMode Mode;

    scene::ISceneManager* SceneManager;
    core::array<scene::ISceneNode*>* SceneLightList;
    scene::E_SCENE_NODE_RENDER_PASS CurrentRenderPass;
    scene::ISceneNode* CurrentSceneNode;

public:
    CLightManager(scene::ISceneManager* sceneManager)
        : Mode(NO_MANAGEMENT),
        SceneManager(sceneManager), SceneLightList(0),
        CurrentRenderPass(scene::ESNRP_NONE), CurrentSceneNode(0)
    {
    }

    virtual void OnPreRender(core::array<scene::ISceneNode*>& lightList)
    {
        SceneLightList = &lightList;
    }

    virtual void OnPostRender()
    {
        for (u32 i = 0; i < SceneLightList->size(); i++)
            (*SceneLightList)[i]->setVisible(true);
    }

    virtual void OnRenderPassPreRender(scene::E_SCENE_NODE_RENDER_PASS renderPass)
    {
        CurrentRenderPass = renderPass;
    }

    virtual void OnRenderPassPostRender(scene::E_SCENE_NODE_RENDER_PASS renderPass)
    {
        for (u32 i = 0; i < SceneLightList->size(); ++i)
            (*SceneLightList)[i]->setVisible(false);
    }

    virtual void OnNodePreRender(scene::ISceneNode* node)
    {
        CurrentSceneNode = node;

        if (LIGHTS_NEAREST_NODE == Mode)
        {
            const vector3df nodePosition = node->getAbsolutePosition();

            array<LightDistanceElement> sortingArray;
            sortingArray.reallocate(SceneLightList->size());

            u32 i;
            for (i = 0; i < SceneLightList->size(); ++i)
            {
                scene::ISceneNode* lightNode = (*SceneLightList)[i];
                const f64 distance = lightNode->getAbsolutePosition().getDistanceFromSQ(nodePosition);
                sortingArray.push_back(LightDistanceElement(lightNode, distance));
            }

            sortingArray.sort();

            for (i = 0; i < sortingArray.size(); ++i)
                sortingArray[i].node->setVisible(i < 3);
        }
        else if (LIGHTS_IN_ZONE == Mode)
        {
            for (u32 i = 0; i < SceneLightList->size(); ++i)
            {
                if ((*SceneLightList)[i]->getType() != scene::ESNT_LIGHT)
                    continue;
                scene::ILightSceneNode* lightNode = static_cast<scene::ILightSceneNode*>((*SceneLightList)[i]);
                video::SLight& lightData = lightNode->getLightData();

                if (video::ELT_DIRECTIONAL != lightData.Type)
                    lightNode->setVisible(false);
            }

            scene::ISceneNode* parentZone = findZone(node);
            if (parentZone)
                turnOnZoneLights(parentZone);
        }
    }

    virtual void OnNodePostRender(scene::ISceneNode* node)
    {
    }

    void setMode(int m) {
        Mode = (LightManagementMode)m;
    }

private:

    scene::ISceneNode* findZone(scene::ISceneNode* node)
    {
        if (!node)
            return 0;

        if (node->getType() == scene::ESNT_EMPTY)
            return node;

        return findZone(node->getParent());
    }

    void turnOnZoneLights(scene::ISceneNode* node)
    {
        core::list<scene::ISceneNode*> const& children = node->getChildren();
        for (core::list<scene::ISceneNode*>::ConstIterator child = children.begin();
            child != children.end(); ++child)
        {
            if ((*child)->getType() == scene::ESNT_LIGHT)
                (*child)->setVisible(true);
            else
                turnOnZoneLights(*child);
        }
    }


    class LightDistanceElement
    {
    public:
        LightDistanceElement() {};

        LightDistanceElement(scene::ISceneNode* n, f64 d)
            : node(n), distance(d) {
        }

        scene::ISceneNode* node;
        f64 distance;

        bool operator < (const LightDistanceElement& other) const
        {
            return (distance < other.distance);
        }
    };
};