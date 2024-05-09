/*******************************************************************************************
*
*   raylib [core] example - Initialize 3d camera mode
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "rlgl.h"
#include "functions.h"
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };  // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

    Vector3 cubePosition = { 0.0f, 0.0f, 0.0f };

    float cubeRotationX = 0;
    float cubeRotationY = 0;
    float cubeRotationZ = 0;

    float RotationAngle = 0;
    float globalPositionX = 0;

    float cubeSideSize = 2;
    float offsetFromGround = 10;

    int cameraMode = 1;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    DisableCursor();

    Texture heliTexture = LoadTexture("..\\helicopter.png");
    GenTextureMipmaps(&heliTexture);

    Texture bombTexture = LoadTexture("..\\bomb.png");
    GenTextureMipmaps(&bombTexture);

    Texture lessssgooTexture = LoadTexture("..\\img.png");
    GenTextureMipmaps(&lessssgooTexture);

    Model sphere = LoadModelFromMesh(GenMeshSphere(1.0f, 20, 10));
    sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = bombTexture;


    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        //Camera update
        //не зрозуміло, як поєднати режим камери із UpdateCameraPro(), моє рішення:
        if(IsKeyDown(KEY_ONE))
            cameraMode = 1;
        else if(IsKeyDown(KEY_TWO))
            cameraMode = 2;
        else if(IsKeyDown(KEY_THREE))
            cameraMode = 3;

        switch (cameraMode) {
            case 1:
                UpdateCameraPro(&camera,
                                (Vector3){
                                        IsKeyDown(KEY_W)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f) - IsKeyDown(KEY_S)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f),  // Move forward-backward
                                        IsKeyDown(KEY_D)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f) - IsKeyDown(KEY_A)*(IsKeyDown(KEY_LEFT_SHIFT) ? .25f : .1f),  // Move right-left
                                        0.0f                                                     // Move up-down
                                },
                                (Vector3){
                                        GetMouseDelta().x*0.05f,                            // Rotation: yaw
                                        GetMouseDelta().y*0.05f,                            // Rotation: pitch
                                        0.0f                                                // Rotation: roll
                                },
                                GetMouseWheelMove()*1.5f);                              // Move to target (zoom)

                break;

            case 2:
                UpdateCamera(&camera, CAMERA_FREE);
                break;

            case 3:
            default:
                UpdateCamera(&camera, CAMERA_FIRST_PERSON);
                break;
        }

        //Projection toggle
        if (IsKeyPressed(KEY_P)) {
            if(camera.projection == CAMERA_PERSPECTIVE)
                camera.projection = CAMERA_ORTHOGRAPHIC;
            else
                camera.projection = CAMERA_PERSPECTIVE;
        }

        //Sphere and propeller update
        if(IsKeyDown(KEY_EQUAL)) {
            RotationAngle+=20.0f;
            if(RotationAngle >= 360)
                RotationAngle = 0;
            globalPositionX+=.25;
        }
        else if (IsKeyDown(KEY_MINUS))
        {
            RotationAngle-=20.0f;
            if(RotationAngle >= 360)
                RotationAngle = 0;
            globalPositionX-=.25;
        }

        //Enable\Disable cursor
        if(IsKeyDown(KEY_BACKSPACE)) {
            EnableCursor();
        }
        else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            DisableCursor();
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        if(cameraMode == 3)
            DrawCube(camera.target, 1.0f, 1.0f, 1.0f, PURPLE);

        DrawTextureSide(lessssgooTexture, (Vector2) {100.0f, 100.0f}, WHITE);

        rlPushMatrix();

        rlTranslatef(globalPositionX, 0, 0);

        rlRotatef(cubeRotationX, 1, 0, 0);
        rlRotatef(cubeRotationY, 0, 1, 0);
        rlRotatef(cubeRotationZ, 0, 0, 1);

        DrawCube(Vector3{ 1.0f, 10.0f, 0.0f }, 1.0f, .25f, .25f, RED);
        DrawCubeWires(Vector3{ 1.0f, 10.0f, 0.0f }, 1.0f, .25f, .25f, BLACK);

        rlPushMatrix();
        rlTranslatef(0,offsetFromGround,cubeSideSize/2);
        rlRotatef(90, 1,0,0);
        DrawTextureSide(heliTexture, (Vector2) {cubeSideSize, cubeSideSize}, WHITE);
        rlPopMatrix();

        rlPushMatrix();
        rlTranslatef(-cubeSideSize/2,offsetFromGround,0);
        rlRotatef(90, 0,0,1);
        DrawTextureSide(heliTexture, (Vector2) {cubeSideSize, cubeSideSize}, WHITE);
        rlPopMatrix();

        rlPushMatrix();
        rlTranslatef(0,offsetFromGround,-cubeSideSize/2);
        rlRotatef(90, 1,0,0);
        rlRotatef(180, 0,0,1);
        DrawTextureSide(heliTexture, (Vector2) {cubeSideSize, cubeSideSize}, WHITE);
        rlPopMatrix();

        rlPushMatrix();
        rlTranslatef(cubeSideSize/2,offsetFromGround,0);
        rlRotatef(-90, 0,0,1);
        DrawTextureSide(heliTexture, (Vector2) {cubeSideSize, cubeSideSize}, WHITE);
        rlPopMatrix();

        rlPushMatrix();
        rlTranslatef(0,cubeSideSize/2 + offsetFromGround,0);
        DrawTextureSide(heliTexture, (Vector2) {cubeSideSize, cubeSideSize}, WHITE);
        rlPopMatrix();

        rlPushMatrix();
        rlTranslatef(0,-cubeSideSize/2 + offsetFromGround,0);
        rlRotatef(180, 1,0,0);
        DrawTextureSide(heliTexture, (Vector2) {cubeSideSize, cubeSideSize}, WHITE);
        rlPopMatrix();

        rlPushMatrix();

        Vector3 a {0.0f, 10.0f, 0.0f};

        rlTranslatef(a.x, a.y, a.z);
        rlRotatef(RotationAngle, 1, 0, 0);
        rlTranslatef(a.x, -a.y, a.z);

        DrawModel(sphere, (Vector3){ -2.0f, 10.0f, 0.0f }, 1.0f, WHITE);

        DrawCube(Vector3{ 1.5f, 10.0f, 0.0f }, .1f,  5.0f, .5f, ORANGE);
        DrawCubeWires(Vector3{ 1.5f, 10.0f, 0.0f }, .1f,  5.0f, .5f, BLACK);

        rlPopMatrix();

        rlPopMatrix();

        EndMode3D();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(heliTexture);
    UnloadTexture(bombTexture);
    UnloadTexture(lessssgooTexture);

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}