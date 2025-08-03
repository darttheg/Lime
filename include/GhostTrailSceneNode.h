 
#ifndef _GHOSTRAILSCENENODE_H_
#define _GHOSTRAILSCENENODE_H_
 
#include <irrlicht.h>
 
using namespace irr;
 
/**
This class creates a motion or ghost, in the sense of the animation term "ghosting", trail.
It maybe used for many types of effects, like, but not reduced to, motion trails, moving ribbons, flags, and others
 
Requires a parent scene node so it can follow it, and updates its mesh with the positions of this node.
Has several alignment options, suited for many situations, but just in the case a single scene node isn't enough
it can be specified a second node to have full control of how the trail behaves.
 
It has a single texture coordinate applied, and it doesn't support tangent space for now, though the normals for this node
may update if there is need for lighting, for instance.
 
It may animate on its own, or it can be specified to be animated on demand, for instance on a multiple rendering pass setup. (shadow maps)
 
It has a wind parameter designed to make the whole set of points move in the specified direction, if set. Though it is a bit too much sensitive
 
This node isn't affected by the rotations or scale operations, simply because it is an special effect, and depends on the parent node
to move. Not obstant, the position shifts the location of the effect. This shift is done in the parents space, not in global space, unless the
parent was the root scene node...
 
If there is no parent node, it doesn't render at all.
 
TODO: serialization options...?
*/
 
class ghostTrailSceneNode : public scene::ISceneNode
{
public:
 
    enum E_ALIGNMENT_MODE
        /*This enumerates all the modes in which the segments of the trail are updated. Note that the scope of this enumerator is*/
    {
        //Aligns segments with regard to the X axis (i.e. the "right" vector) of the camera
        EALM_CAMERA_X_AXIS, 
 
        //Aligns segments with regard to the Y axis (i.e. the "up" vector) of the camera
        EALM_CAMERA_Y_AXIS, 
 
        //Aligns segments with regard to the X world axis
        EALM_GLOBAL_X_AXIS, 
 
        //Aligns segments with regard to the Y world axis
        EALM_GLOBAL_Y_AXIS, 
 
        //Aligns segments with regard to the Z world axis
        EALM_GLOBAL_Z_AXIS, 
 
        //Aligns segments with regard to the X parent axis
        EALM_PARENT_X_AXIS, 
 
        //Aligns segments with regard to the Y parent axis
        EALM_PARENT_Y_AXIS, 
 
        //Aligns segments with regard to the Z parent axis
        EALM_PARENT_Z_AXIS, 
 
        //32 bits enum
        EALM_COUNT = 0x7FFFFFF
    };
 
private:
    //number of segments of the scene node
    s32 numberOfSegments;
 
    //width of the trail
    f32 trailWidth;
 
    //secondary node, for alignment purposes
    scene::ISceneNode* secondaryNode;
 
    //alignment mode.
    E_ALIGNMENT_MODE mode;
 
    //wind strength
    core::vector3df wind;
 
    //Internal variable to handle the circular stack of the trail
    s32 startingPoint;
 
    //The circular stack of the trail
    core::vector3df* pointBuffer;
    
    //the material of the trail
    video::SMaterial material;
 
    //The bounding box of the trail
    core::aabbox3df boundingBox;
 
    //the mesh of the trail
    scene::SMesh* trailMesh;
 
    //the meshBuffer of the trail
    scene::SMeshBuffer* trailMeshBuffer;
 
    //should normals be updated also?
    bool updateNormals;
 
    //should this node animate itself?
    bool autoAnimate;

    //fixed segment size
    f32 fixedSegSize;
 
public:
//constructor.
 
    ghostTrailSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id = -1, f32 trailWidth = 10.0f, s32 numberSegments = 64);
    
//destructor.
 
    ~ghostTrailSceneNode();
 
//gets the material of the trail.
 
    virtual video::SMaterial& getMaterial(u32 index);
 
//there is only 1 material, which is returned here.
 
    virtual u32 getMaterialCount() const;
 
//gets the bounding box of this node.
 
    virtual const core::aabbox3df& getBoundingBox() const;
    
//performs the onRegister callback.
 
    virtual void OnRegisterSceneNode();
 
//Performs an OnAnimate callback.
 
    virtual void OnAnimate(u32 timeMs);
 
//Renders the node.
 
    virtual void render();
 
//This method sets the current width of the trail, so the trail width's varies with the time.
 
    void setTrailWidth(f32 width = 10.0);
 
//This method provides a secondary node to the trail, so the trail is calculated using the position of the parent of this node and the position
//of this node.
 
    void setSecondaryNode(ISceneNode* n)
    {
        secondaryNode = n;
    }
 
//This method specifies how the trail segments will be aligned. It has no effect if a secondary node is provided, as the segments will align
//with regard to the parent and the secondary node. It may change dynamically.
 
    void setAlignmentMode(E_ALIGNMENT_MODE m)
    {
        mode = m;
    }
 
//This method makes the trail update on its own or not. It is useful, for instance, if the node is rendered in a multipass setup
//Also, the node is stored dynamically on the hardware, so if this node isn't updated, the data on the card remains static.
 
    void setAutoAnimate(bool animate)
    {
        autoAnimate = animate;
    }
 
//This method sets the direction of the wind on this trail. It is very sensitive, so it must be handled with care.
 
    void setWind(core::vector3df w = core::vector3df(0,-0.10f,0))
    {
        wind = w;
    }
 
//This method updates the normals of the trail, for lighting purposes. If it is not set, it only animates the positions of the trail
    void setUpdateNormals(bool update = false)
    {
        updateNormals = update;
    }
 
//This method forces the update of the trail. use with caution, as it may provide odd results if it is used too many times in a single render. 
//Generally, it should be used only once per render.
 
    void animate();
 
//This method serves to identify this kind of scene node, if this can have any use...
 
    scene::ESCENE_NODE_TYPE getType() const
    {
        return (scene::ESCENE_NODE_TYPE)MAKE_IRR_ID('t','a','i','l');
    }

//Get fixed segment size
    f32 getFixedSize() const
    {
        return fixedSegSize;
    }

//Set fixed segment size
    void setFixedSize(f32 s)
    {
        fixedSegSize = s;
    }

private:
    u32 currentTime;
 
    u32 lastTime;
 
    void updateTrailData();
 
    void transferTrailData();
};
 
#endif