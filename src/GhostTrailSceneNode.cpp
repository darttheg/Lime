
#include "ghostTrailSceneNode.h"

ghostTrailSceneNode::ghostTrailSceneNode(scene::ISceneNode* parent, scene::ISceneManager* smgr, s32 id, f32 TWidth, s32 numberSegments)
    :scene::ISceneNode(parent, smgr, id)
{
    trailMesh = new scene::SMesh();
    trailMeshBuffer = new scene::SMeshBuffer();
    trailMesh->addMeshBuffer(trailMeshBuffer);
    trailMeshBuffer->drop();

    //number of vertices: as many segments plus 2, multiplied by 2; one initial, one final, n between
    //number of indices: as many segments plus 1, multiplied by 6; 2 triangles each segment.

    numberOfSegments = (numberSegments + 2) * 2 < 65536 ? numberSegments : 32766; //max 65536 vertices! we use 16 bit indices.

    numberOfSegments < 0 ? numberOfSegments = 2 : 0; //faster than an IF :)

    video::S3DVertex* vertexData = new video::S3DVertex[2 * (numberOfSegments + 2)];
    u16* indexData = new u16[6 * (numberOfSegments + 1)];

    //We have to startup the mapping coordinates...

    for (u32 i = 0; i < numberOfSegments + 2; i++)
    {
        f32 uData = i / (f32)(numberOfSegments + 1);

        vertexData[2 * i + 0].TCoords = core::vector2df(uData, 0.0f);
        vertexData[2 * i + 0].Color = video::SColor(255, 255, 255, 255);
        vertexData[2 * i + 1].TCoords = core::vector2df(uData, 1.0f);
        vertexData[2 * i + 1].Color = video::SColor(255, 255, 255, 255);
    }


    /*
        Now, we have to sew the indices...

        0-2
        |/| if 0 segments. indices: 0,1,2,2,1,3
        1-3

        0-2-4
        |/|/| if 1 segment; indices 0,1,2,2,1,3 ,2,3,4,4,3,5
        1-3-5

        0-2-4-6
        |/|/|/| if 2 segments; indices 0,1,2,2,1,3 ,2,3,4,4,3,5 ,4,5,6,6,5,7
        1-3-5-7


        if N segments... indices ... 2i, 2i+1, 2i+2, 2i+2, 2i+1, 2i+3 ...
    */

    for (u32 i = 0; i < numberOfSegments + 1; i++)
    {
        indexData[6 * i + 0] = 2 * i + 0;
        indexData[6 * i + 1] = 2 * i + 1;
        indexData[6 * i + 2] = 2 * i + 2;
        indexData[6 * i + 3] = 2 * i + 2;
        indexData[6 * i + 4] = 2 * i + 1;
        indexData[6 * i + 5] = 2 * i + 3;
    }

    //Finally, we add this soup of triangles to the mesh buffer
    trailMeshBuffer->append(vertexData, 2 * (numberOfSegments + 2), indexData, 6 * (numberOfSegments + 1));

    startingPoint = 0;//It must be an in range index!

    pointBuffer = new core::vector3df[2 * (numberOfSegments + 2)];

    //Having a single strip of triangles creates somewhat odd effects when this isn't set, so we help the 
    //user by setting this for him. Anyway, he can unset it again later if he wishes...

    for (u32 i = 0; i < _IRR_MATERIAL_MAX_TEXTURES_; i++)
    {
        material.TextureLayer[i].TextureWrapU = material.TextureLayer[i].TextureWrapV = video::ETC_CLAMP_TO_EDGE;
    }
    material.BackfaceCulling = false;

    trailWidth = TWidth;

    updateNormals = false;//Just in case, this trail won't update its normals unless specified otherwise.

    autoAnimate = true;//If we have a multiple pass setup, maybe we DON'T want to let this happen...

    secondaryNode = 0;//for the moment, only the parent

    wind = core::vector3df(0, 0, 0);//no wind.

    mode = EALM_GLOBAL_Y_AXIS;//default, aligned with the Y world axis.

    trailMesh->setHardwareMappingHint(scene::EHM_DYNAMIC);//It may change, or it may not, still, the user may alter it...
}

ghostTrailSceneNode::~ghostTrailSceneNode()
{
    trailMesh->drop();
    delete pointBuffer;
}

video::SMaterial& ghostTrailSceneNode::getMaterial(u32 index)
{
    return material;
}

u32 ghostTrailSceneNode::getMaterialCount() const
{
    return 1;
}

const core::aabbox3df& ghostTrailSceneNode::getBoundingBox() const
{
    return boundingBox;
}

void ghostTrailSceneNode::OnRegisterSceneNode()
{
    if (getParent()) { //If there is no parent node, there is no need for rendering
        if (isVisible()) {
            getSceneManager()->registerNodeForRendering(this);
        }
        scene::ISceneNode::OnRegisterSceneNode();
    }
}

void ghostTrailSceneNode::OnAnimate(u32 timeMs)
{
    lastTime = currentTime;
    currentTime = timeMs;

    if (autoAnimate)
    {
        animate();
    }
    scene::ISceneNode::OnAnimate(timeMs);
}

void ghostTrailSceneNode::render()
{
    scene::ISceneManager* smgr = getSceneManager();
    video::IVideoDriver* drv = smgr->getVideoDriver();
    scene::ICameraSceneNode* cam = smgr->getActiveCamera();

    drv->setMaterial(material);

    drv->setTransform(video::ETS_WORLD, core::matrix4().makeIdentity());
    drv->setTransform(video::ETS_VIEW, cam->getViewMatrix());
    drv->setTransform(video::ETS_PROJECTION, cam->getProjectionMatrix());

    drv->drawMeshBuffer(trailMeshBuffer);
}

void ghostTrailSceneNode::setTrailWidth(f32 width)
{
    trailWidth = width;
    //variable trail size, but only for the last pair of points,
    //this allows, for instance, for the matrix bullet trails effect :)
}

void ghostTrailSceneNode::updateTrailData()
{
    core::matrix4 parentMatrix = getParent()->getAbsoluteTransformation();
    core::vector3df alignmentVector, viewVector, upVector;
    core::vector3df parentPosition;
    parentMatrix.transformVect(parentPosition, getPosition()); //Posicion local
    core::vector3df secondaryPosition;
    s32 p;

    startingPoint < 0 ? startingPoint = numberOfSegments + 1 : 0;

    if (!secondaryNode)
    {
        switch (mode) {
        case EALM_CAMERA_X_AXIS:
            upVector = getSceneManager()->getActiveCamera()->getUpVector();
            upVector.normalize();

            viewVector = getSceneManager()->getActiveCamera()->getTarget() - getSceneManager()->getActiveCamera()->getAbsolutePosition();
            viewVector.normalize();

            alignmentVector = viewVector.crossProduct(upVector);
            alignmentVector.normalize();
            break;

        case EALM_CAMERA_Y_AXIS:
            alignmentVector = getSceneManager()->getActiveCamera()->getUpVector();
            alignmentVector.normalize();
            break;

        case EALM_GLOBAL_X_AXIS:
            alignmentVector = core::vector3df(1.0f, 0, 0);
            break;

        case EALM_GLOBAL_Y_AXIS:
            alignmentVector = core::vector3df(0, 1.0f, 0);
            break;

        case EALM_GLOBAL_Z_AXIS:
            alignmentVector = core::vector3df(0, 0, 1.0f);
            break;

        case EALM_PARENT_X_AXIS:
            parentMatrix.setTranslation(core::vector3df());//absolute transforms include a translation, hence we need this
            parentMatrix.transformVect(alignmentVector, core::vector3df(1, 0, 0));
            alignmentVector.normalize();
            break;

        case EALM_PARENT_Y_AXIS:
            parentMatrix.setTranslation(core::vector3df());
            parentMatrix.transformVect(alignmentVector, core::vector3df(0, 1, 0));
            alignmentVector.normalize();
            break;

        case EALM_PARENT_Z_AXIS:
            parentMatrix.setTranslation(core::vector3df());
            parentMatrix.transformVect(alignmentVector, core::vector3df(0, 0, 1));
            alignmentVector.normalize();
            break;
        }
        pointBuffer[2 * startingPoint + 0] = parentPosition - alignmentVector * trailWidth / 2.0f;
        pointBuffer[2 * startingPoint + 1] = parentPosition + alignmentVector * trailWidth / 2.0f;
    }
    else
    {//There is no need for alignment options with a secondary node
        parentPosition = getParent()->getAbsolutePosition();
        secondaryNode->updateAbsolutePosition();
        secondaryPosition = secondaryNode->getAbsolutePosition();

        pointBuffer[2 * startingPoint + 0] = parentPosition;
        pointBuffer[2 * startingPoint + 1] = secondaryPosition;
    }

    //Wind update... It is okay, in the end all the data is transferred from the app to the video card.
    //The good thing is that this affects all the points at the same time.
    if (wind.getLengthSQ() > 0.001f)
    {
        u32 timeIncrement = currentTime - lastTime;

        irr::core::vector3df out;

        for (u32 i = 0; i < numberOfSegments + 2; i++)
        {
            s32 windStrength = i < startingPoint ? numberOfSegments + 2 - startingPoint + i : i - startingPoint;

            // Problem here
            if (fixedSegSize > 0.001f)
                out = fixedSegSize * wind * (f32)windStrength;
            else
                out = wind * (f32)windStrength / (f32)(numberOfSegments + 2);

            pointBuffer[2 * i + 0] += out;
            pointBuffer[2 * i + 1] += out;
        }
    }

    transferTrailData();

    startingPoint--;
}

void ghostTrailSceneNode::animate()
{
    if (getParent())
    {
        updateTrailData();
    }
}

void ghostTrailSceneNode::transferTrailData()
{
    video::S3DVertex* vertices = (video::S3DVertex*)trailMeshBuffer->getVertices();
    core::vector3df normal, vectorU, vectorV;

    boundingBox.reset(0, 0, 0);
    trailMeshBuffer->BoundingBox.reset(0, 0, 0);
    trailMesh->BoundingBox.reset(0, 0, 0);

    for (u32 i = 0; i < numberOfSegments + 2; i++)
    {
        s32 vertexIndex = i < startingPoint ? numberOfSegments + 2 - startingPoint + i : i - startingPoint;
        vertices[2 * vertexIndex + 0].Pos = pointBuffer[2 * i + 0];
        vertices[2 * vertexIndex + 1].Pos = pointBuffer[2 * i + 1];
        boundingBox.addInternalBox(core::aabbox3df(vertices[2 * vertexIndex + 0].Pos, vertices[2 * vertexIndex + 1].Pos));
    }

    trailMeshBuffer->BoundingBox.addInternalBox(boundingBox);
    trailMesh->BoundingBox.addInternalBox(boundingBox);

    if (updateNormals)
    {
        for (u32 i = 0; i < numberOfSegments + 1; i++)
        {
            vectorU = vertices[2 * i + 2].Pos - vertices[2 * i].Pos;
            vectorV = vertices[2 * i + 1].Pos - vertices[2 * i + 2].Pos;
            normal = vectorU.crossProduct(vectorV);
            normal.normalize();
            vertices[2 * i].Normal = normal;
            vertices[2 * i + 1].Normal = normal;
        }

        vertices[2 * (numberOfSegments + 1) + 0].Normal = normal;
        vertices[2 * (numberOfSegments + 1) + 1].Normal = normal;
    }

    trailMeshBuffer->setDirty(scene::EBT_VERTEX);
}
