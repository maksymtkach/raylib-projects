#include "stdlib.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "glfw3.h"

int screenWidth = 1280;
int screenHeight = 720;

typedef struct {
    bool enabled;
    Vector3 position;
    Vector3 target;
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;

    // Shader locations
    int enabledLoc;
    int positionLoc;
    int targetLoc;
    int ambientLoc;
    int diffuseLoc;
    int specularLoc;
    int attenuationLoc;
} Light;

typedef struct {
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    float shininess;
    float transparency;

    int ambientLoc;
    int diffuseLoc;
    int specularLoc;
    int shininessLoc;
    int transparencyLoc;
} MyMaterial;

typedef struct {
    Model model;
    Vector3 position;
    MyMaterial material;
    float viewDistance;
} ModelPos;

void updateLight(Light light, Shader shader) {
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    // Send to shader light position values
    float position[3] = { light.position.x, light.position.y, light.position.z };
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);
    // Send to shader light target position values
    float target[3] = { light.target.x, light.target.y, light.target.z };
    SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

    // Send to shader light color values
    float ambient[3] = {light.ambient.x, light.ambient.y, light.ambient.z};
    float diffuse[3] = {light.diffuse.x, light.diffuse.y, light.diffuse.z};
    float specular[3] = {light.specular.x, light.specular.y, light.specular.z};

    SetShaderValue(shader, light.ambientLoc, ambient, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.diffuseLoc, diffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, light.specularLoc, specular, SHADER_UNIFORM_VEC3);
}

void updateMaterial(MyMaterial material, Shader shader) {
    float ambient[3] = {material.ambient.x, material.ambient.y, material.ambient.z};
    float diffuse[3] = {material.diffuse.x, material.diffuse.y, material.diffuse.z};
    float specular[3] = {material.specular.x, material.specular.y, material.specular.z};

    SetShaderValue(shader, material.ambientLoc, ambient, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, material.diffuseLoc, diffuse, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, material.specularLoc, specular, SHADER_UNIFORM_VEC3);
    SetShaderValue(shader, material.shininessLoc, &material.shininess, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, material.transparencyLoc, &material.transparency, SHADER_UNIFORM_FLOAT);
}

int cmp(const void *l, const void *r) {
    return (int)(((ModelPos*)r)->viewDistance - ((ModelPos*)l)->viewDistance);
}

#ifndef LIGHTING_STENCIL_H
#define LIGHTING_STENCIL_H

void BeginStencil();
void BeginStencilMask();
void EndStencilMask();
void EndStencil();

#endif //LIGHTING_STENCIL_H

// Stencil masks
void BeginStencil()
{
    rlDrawRenderBatchActive();
    glEnable(GL_STENCIL_TEST);
}

void BeginStencilMask()
{
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Set any stencil to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF); // Write to stencil buffer
    glDepthMask(GL_FALSE); // Don't write to depth buffer
    glClear(GL_STENCIL_BUFFER_BIT); // Clear stencil buffer (0 by default)
}

void EndStencilMask()
{
    rlDrawRenderBatchActive();
    glStencilFunc(GL_EQUAL, 1, 0xFF); // Pass test if stencil value is 1
    glStencilMask(0x00); // Don't write anything to stencil buffer
    glDepthMask(GL_TRUE); // Write to depth buffer
}

void EndStencil()
{
    rlDrawRenderBatchActive();
    glDisable(GL_STENCIL_TEST);
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "lab7");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    Camera camera = { 0 };
    camera.position = (Vector3){ 25.0f, 6.0f, 25.0f };     // Camera position
    camera.target = (Vector3){ 0.0f, 3.0f, 0.0f };          // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };              // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                    // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;
    DisableCursor();
    SetTargetFPS(60);
    bool firstPerson = false;

    Model cube = LoadModelFromMesh(GenMeshCube(5, 5, 5));
    Model plane = LoadModelFromMesh(GenMeshPlane(40, 40, 5, 5));
    Model sphere = LoadModelFromMesh(GenMeshSphere(2, 16, 16));
    Shader shader = LoadShader("../lighting.vert", "../lighting.frag");

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    plane.materials[0].shader = shader;
    cube.materials[0].shader = shader;
    sphere.materials[0].shader = shader;

    Light light = {false};
    light.enabled = false;
    light.position = (Vector3){10,10,10};
    light.target = Vector3Zero();
    light.ambient = (Vector3) {1.0f, 1.0f, 1.0f};
    light.diffuse = (Vector3) {1.0f, 1.0f, 1.0f};
    light.specular = (Vector3) {1.0f, 1.0f, 1.0f};

    light.enabledLoc = GetShaderLocation(shader, "light.enabled");
    light.positionLoc = GetShaderLocation(shader, "light.position");
    light.targetLoc = GetShaderLocation(shader, "light.target");
    light.ambientLoc = GetShaderLocation(shader, "light.ambient");
    light.diffuseLoc = GetShaderLocation(shader, "light.diffuse");
    light.specularLoc = GetShaderLocation(shader, "light.specular");

    MyMaterial emerald = {0};
    emerald.ambient = (Vector3){0.24725f,	0.1995f, 0.0745f};
    emerald.diffuse = (Vector3){0.75164f, 0.60648f, 0.22648f};
    emerald.specular = (Vector3){0.628281f, 0.555802f, 0.366065f};
    emerald.shininess = 0.4f*128;
    emerald.transparency = 0.3f;

    emerald.ambientLoc = GetShaderLocation(shader, "material.ambient");
    emerald.diffuseLoc = GetShaderLocation(shader, "material.diffuse");
    emerald.specularLoc = GetShaderLocation(shader, "material.specular");
    emerald.shininessLoc = GetShaderLocation(shader, "material.shininess");
    emerald.transparencyLoc = GetShaderLocation(shader, "material.transparency");

    MyMaterial ruby = {0};
    ruby.ambient = (Vector3){0.1745f,	0.01175f, 0.01175f};
    ruby.diffuse = (Vector3){0.61424f, 0.04136f, 0.04136f};
    ruby.specular = (Vector3){0.727811f, 0.626959f, 0.626959f};
    ruby.shininess = 0.6f*128;
    ruby.transparency = 0.3f;

    ruby.ambientLoc = GetShaderLocation(shader, "material.ambient");
    ruby.diffuseLoc = GetShaderLocation(shader, "material.diffuse");
    ruby.specularLoc = GetShaderLocation(shader, "material.specular");
    ruby.shininessLoc = GetShaderLocation(shader, "material.shininess");
    ruby.transparencyLoc = GetShaderLocation(shader, "material.transparency");

    MyMaterial obsidian = {0};
    obsidian.ambient = (Vector3){0.05375f, 0.05f, 0.06625f};
    obsidian.diffuse = (Vector3){0.18275f, 0.17f, 0.22525f};
    obsidian.specular = (Vector3){0.332741f, 0.328634f, 0.346435f};
    obsidian.shininess = 0.3f*128;
    obsidian.transparency = 1.0f;

    obsidian.ambientLoc = GetShaderLocation(shader, "material.ambient");
    obsidian.diffuseLoc = GetShaderLocation(shader, "material.diffuse");
    obsidian.specularLoc = GetShaderLocation(shader, "material.specular");
    obsidian.shininessLoc = GetShaderLocation(shader, "material.shininess");
    obsidian.transparencyLoc = GetShaderLocation(shader, "material.transparency");

    MyMaterial lamp = {0};
    lamp.ambient = (Vector3){1.0f, 1.0f, 1.0f};
    lamp.diffuse = (Vector3){1.0f, 1.0f, 1.0f};
    lamp.specular = (Vector3){1.0f, 1.0f, 1.0f};
    lamp.shininess = 1.0f*128;
    lamp.transparency = 0.3f;

    lamp.ambientLoc = GetShaderLocation(shader, "material.ambient");
    lamp.diffuseLoc = GetShaderLocation(shader, "material.diffuse");
    lamp.specularLoc = GetShaderLocation(shader, "material.specular");
    lamp.shininessLoc = GetShaderLocation(shader, "material.shininess");
    lamp.transparencyLoc = GetShaderLocation(shader, "material.transparency");

    updateLight(light, shader);

    ModelPos modelPositions[2] = {
            (ModelPos) {sphere, light.position, ruby},
            (ModelPos) {cube, (Vector3) {7, 2.5f, 0}, lamp}
    };
    int modelIndex = 0;

    while (!WindowShouldClose()) {
        Vector3 camMovement = (Vector3){0};
        Vector3 camRotation = (Vector3){0};

        Vector2 delta = Vector2Scale(GetMouseDelta(), 0.1f);
        camRotation.x = delta.x;
        camRotation.y = delta.y;

        float camSpeed = 1.0f;

        if(IsKeyPressed(KEY_R)) EnableCursor();
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) DisableCursor();
        if(IsKeyDown(KEY_W)) camMovement.x = 1;
        if(IsKeyDown(KEY_S)) camMovement.x = -1;
        if(IsKeyDown(KEY_A)) camMovement.y = -1;
        if(IsKeyDown(KEY_D)) camMovement.y = 1;
        if(IsKeyDown(KEY_LEFT_SHIFT)) camSpeed = 2.0f;
        if(IsKeyDown(KEY_LEFT_CONTROL)) camSpeed = 0.3f;
        if(IsKeyPressed(KEY_L)) light.enabled = !light.enabled;
        if(IsKeyPressed(KEY_P)) camera.projection = !camera.projection;
        if(IsKeyPressed(KEY_F)) {
            firstPerson = !firstPerson;
            if(firstPerson) {
                camera.position.y = 6.0f;
                camera.target.y = 6.0f;
            }
        }
        if(!firstPerson) {
            if(IsKeyDown(KEY_C)) camMovement.z = 1;
            if(IsKeyDown(KEY_V)) camMovement.z = -1;
        }

/*
        if(IsKeyDown(KEY_UP)) light.position.z--;
        if(IsKeyDown(KEY_DOWN)) light.position.z++;
        if(IsKeyDown(KEY_LEFT)) light.position.x--;
        if(IsKeyDown(KEY_RIGHT)) light.position.x++;
        if(IsKeyDown(KEY_LEFT_BRACKET)) light.position.y--;
        if(IsKeyDown(KEY_RIGHT_BRACKET)) light.position.y++;
*/
        if (IsKeyPressed(KEY_ONE)) modelIndex = 0;
        if (IsKeyPressed(KEY_TWO)) modelIndex = 1;
        if (IsKeyDown(KEY_UP)) modelPositions[modelIndex].position.x += .5f; // Update the position of the object
        if (IsKeyDown(KEY_DOWN)) modelPositions[modelIndex].position.x += -.5f; // Update the position of the object
        if (IsKeyDown(KEY_LEFT)) modelPositions[modelIndex].position.z += .5f; // Update the position of the object
        if (IsKeyDown(KEY_RIGHT)) modelPositions[modelIndex].position.z += -.5f; // Update the position of the object


        for (int i = 0; i < 2; ++i) {
            modelPositions[i].viewDistance = Vector3Distance(camera.position, modelPositions[i].position);
        }

        UpdateCameraPro(&camera, Vector3Scale(camMovement, camSpeed), camRotation, -GetMouseWheelMove());

        float cameraPos[3] = {camera.position.x, camera.position.y, camera.position.z};
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
        updateLight(light, shader);

        BeginDrawing();
        ClearBackground(light.enabled ? LIGHTGRAY : (Color) {125, 41, 55, 100});
        BeginMode3D(camera);
        BeginBlendMode(BLEND_ALPHA);

        BeginShaderMode(shader);
        MyMaterial material = emerald;
        material.transparency = 1.0f;
        updateMaterial(material, shader);
        rlDrawRenderBatchActive();
        material = ruby;
        material.transparency = 1.0f;
        updateMaterial(material, shader);
        EndShaderMode();

        BeginStencil();
        BeginStencilMask();
        updateMaterial(obsidian, shader);
        DrawModel(plane, (Vector3) {0, 0, 0}, 1.0f, WHITE);
        EndStencilMask();


        BeginShaderMode(shader);
        material = emerald;
        material.transparency = 0.3f;
        updateMaterial(material, shader);
        rlDrawRenderBatchActive();
        material = ruby;
        material.transparency = 0.3f;
        updateMaterial(material, shader);
        EndShaderMode();

        for (int i = 0; i < 2; ++i) {
            material = modelPositions[i].material;
            material.transparency = 0.1f;
            updateMaterial(material, shader);
            DrawModelEx(modelPositions[i].model,
                        (Vector3) {modelPositions[i].position.x, -1-modelPositions[i].position.y, modelPositions[i].position.z},
                        (Vector3) {1.0f, 0.0f, 0.0f},
                        180,
                        (Vector3) {1.0f, 1.0f, 1.0f},
                        WHITE);
        }

        EndStencil();

        lamp.transparency = light.enabled ? 1.0f : 0.3f;
        updateMaterial(lamp, shader);
        for (int i = 0; i < 2; ++i) {
            updateMaterial(modelPositions[i].material, shader);
            DrawModel(modelPositions[i].model, modelPositions[i].position, 1.0f, WHITE);
        }



        EndBlendMode();
        EndMode3D();
        EndDrawing();
    }
    UnloadModel(cube);
    UnloadModel(plane);
    UnloadShader(shader);
    CloseWindow();
    return 0;
}
