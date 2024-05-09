/*******************************************************************************************
*
*   raylib [shaders] example - basic lighting
*
*   NOTE: This example requires raylib OpenGL 3.3 or ES2 versions for shaders support,
*         OpenGL 1.1 does not support shaders, recompile raylib to OpenGL 3.3 version.
*
*   NOTE: Shaders used in this example are #version 330 (OpenGL 3.3).
*
*   Example originally created with raylib 3.0, last time updated with raylib 4.2
*
*   Example contributed by Chris Camacho (@codifies) and reviewed by Ramon Santamaria (@raysan5)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2019-2024 Chris Camacho (@codifies) and Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION            100
#endif

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);  // Enable Multi Sampling Anti Aliasing 4x (if available)
    InitWindow(screenWidth, screenHeight, "raylib [shaders] example - basic lighting");

    // Define the camera to look into our 3d world
    Camera camera = { 0 };
    camera.position = (Vector3){ -2.0f, 4.0f, -6.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 0.5f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    // Load plane model from a generated mesh
    Model model = LoadModelFromMesh(GenMeshPlane(10.0f, 10.0f, 3, 3));
    Model cube = LoadModelFromMesh(GenMeshCube(2.0f, 4.0f, 2.0f));
    Model sphere = LoadModelFromMesh(GenMeshSphere(1.0f, 16.0f, 12.0f));

    // Load basic lighting shader
    Shader shader = LoadShader(TextFormat("D:\\Tools\\raylib\\examples\\shaders\\resources\\shaders\\glsl330\\lighting.vs", GLSL_VERSION),
                               TextFormat("D:\\Tools\\raylib\\examples\\shaders\\resources\\shaders\\glsl330\\lighting.fs", GLSL_VERSION));

    MyMaterial obsidian {0.05375, 0.05, 0.06625, 0.18275, 0.17, 0.22525, 0.332741, 0.328634, 0.346435, 1};
    MyMaterial emerald {0.0215, 0.1745, 0.0215, 0.07568, 0.61424, 0.07568, 0.633, 0.727811, 0.633, 0.1};

    MyMaterial* currentMaterial = &obsidian;
    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading,
    // no need to get the location again if using that uniform name
    //shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    float val[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    int ambientLoc = GetShaderLocation(shader, "ambient");
    SetShaderValue(shader, ambientLoc, val, SHADER_UNIFORM_VEC4);

    auto texture1 = LoadTexture("..\\img.png");
    auto texture2 = LoadTexture("..\\bomb.png");

    // Assign out lighting shader to model
    model.materials[0].shader = shader;
    cube.materials[0].shader = shader;
    sphere.materials[0].shader = shader;

    cube.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture1;
    sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture2;

    // Create light
    Vector3 lightPos = { -2, 1, -2 };
    Light light = CreateLight(LIGHT_POINT, lightPos, Vector3Zero(), WHITE, shader);

    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateCameraPro(&camera,
                        (Vector3){
                                IsKeyDown(KEY_UP)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f) - IsKeyDown(KEY_DOWN)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f),  // Move forward-backward
                                IsKeyDown(KEY_RIGHT)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f) - IsKeyDown(KEY_LEFT)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f),  // Move right-left
                                0.0f                                                     // Move up-down
                        },
                        (Vector3){
                                GetMouseDelta().x*0.05f,                            // Rotation: yaw
                                GetMouseDelta().y*0.05f,                            // Rotation: pitch
                                0.0f                                                // Rotation: roll
                        },
                        GetMouseWheelMove()*1.5f);                         // Move to target (zoom)

        if(IsKeyDown(KEY_BACKSPACE)) {
            EnableCursor();
        }
        else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            DisableCursor();
        }

        // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

        // Check key inputs
        if (IsKeyPressed(KEY_TAB)) { light.enabled = !light.enabled; }
        if (IsKeyDown(KEY_A)) { light.position.x += .1f; }
        if (IsKeyDown(KEY_D)) { light.position.x -= .1f; }
        if (IsKeyDown(KEY_S)) { light.position.z -= .1f; }
        if (IsKeyDown(KEY_W)) { light.position.z += .1f; }
        if (IsKeyDown(KEY_LEFT_SHIFT)) { light.position.y += .1f; }
        if (IsKeyDown(KEY_LEFT_CONTROL)) { light.position.y -= .1f; }
        if(IsKeyPressed(KEY_R)) { light.color = RED; }
        if(IsKeyPressed(KEY_G)) { light.color = GREEN; }
        if(IsKeyPressed(KEY_B)) { light.color = BLUE; }
        if(IsKeyPressed(KEY_F)) { light.color = WHITE; }
        if(IsKeyPressed(KEY_ONE)) {currentMaterial = &obsidian; }
        if(IsKeyPressed(KEY_TWO)) {currentMaterial = &emerald; }

        // Update light/material values
        UpdateLightValues(shader, light);
        UpdateMaterialValues(*currentMaterial, shader);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawModel(model, Vector3Zero(), 1.0f, WHITE);
        DrawModel(cube, Vector3Zero(), 1.0f, WHITE);
        DrawModel(sphere, Vector3{2.0f, 2.5f, 2.0f}, 1.0f, WHITE);

        // Draw spheres to show where the lights are
        if (light.enabled) DrawSphereEx(light.position, 0.2f, 8, 8, light.color);
        else DrawSphereWires(light.position, 0.2f, 8, 8, ColorAlpha(light.color, 0.3f));

        DrawGrid(10, 1.0f);

        EndMode3D();

        DrawFPS(10, 10);

        DrawText("Use Tab to toggle light\n\nUse [W][A][S][D][Shift][Ctrl] to move the light\n\nUse [R][G][B][F] to change light color", 10, 40, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadModel(model);     // Unload the model
    UnloadModel(cube);      // Unload the model
    UnloadShader(shader);   // Unload shader

    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}