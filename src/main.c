#include <windows.h>
#include <gl/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

//----------------------------------------------------------------------------------------------------------------------------------

GLuint windowHeight;
GLuint windowWidth;

struct Camera{
    GLfloat x,y,z;
    GLfloat xRot,zRot;
} camera = {0,0,5, 0,0};

void apply_camera()
{
    glRotatef(-camera.xRot, 1,0,0);
    glRotatef(-camera.zRot, 0,0,1);
    glTranslatef(-camera.x, -camera.y, -camera.z);
}

void rotate_camera(GLfloat xAngle, GLfloat zAngle)
{
    camera.xRot += xAngle;
    if(camera.xRot < 0) camera.xRot = 0;
    if(camera.xRot > 180) camera.xRot = 180;

    camera.zRot += zAngle;
    if(camera.zRot < 0) camera.zRot += 360;
    if(camera.zRot > 360) camera.zRot = 0;
}

void move_camera_with_mouse(GLfloat xSpeed, GLfloat ySpeed)
{
    POINT cursor;
    POINT origin = {800, 400};
    GetCursorPos(&cursor);
    rotate_camera((origin.y - cursor.y)*ySpeed, (origin.x-cursor.x)*xSpeed);
    SetCursorPos(origin.x, origin.y);
}

void set_camera_pos_x_y(GLfloat zAngle, GLfloat speed)
{
    if(speed != 0)
    {
        camera.x += sin(zAngle)*speed;
        camera.y += cos(zAngle)*speed;
    }
}

void move_camera_with_button(float speed)
{
    float zAngle = -camera.zRot / 180 * M_PI;
    if(GetKeyState('W') < 0) set_camera_pos_x_y(zAngle, speed);
    if(GetKeyState('S') < 0) set_camera_pos_x_y(zAngle, -speed);
    if(GetKeyState('A') < 0) set_camera_pos_x_y(zAngle-M_PI/2, speed);
    if(GetKeyState('D') < 0) set_camera_pos_x_y(zAngle+M_PI/2, speed);

    if(GetKeyState(VK_SPACE) < 0) camera.z += speed;
    if(GetKeyState(VK_LSHIFT) < 0) camera.z -= speed;

}

void resize_window(int x, int y)
{
    glViewport(0,0,x,y);
    float k = (float)x/y;
    float size = 0.1;
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glFrustum(-k*size, k*size, -size,size, size*2, 100);
}

void load_texture(char *file_name, int *target)
{
    int width, height, cnt;
    unsigned char *data = stbi_load(file_name, &width, &height, &cnt, 0);

    glGenTextures(1, target);
    glBindTexture(GL_TEXTURE_2D, *target);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , width, height, 0, cnt == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D,0);
    stbi_image_free(data);
}

//GLfloat arrBoxPoints[] = {0,0,0, 0,1,0, 1,1,0, 1,0,0, 0,0,1, 0,1,1, 1,1,1, 1,0,1};
//GLuint arrIndexBoxPoints[] = {0,1,2, 0,2,3, 4,7,6, 4,6,5, 0,3,7, 0,7,4, 1,5,6, 1,6,2, 3,2,6, 3,6,7, 0,4,5, 0,5,1};
#define SIZE_BOX 1


void draw_box()
{


    glPushMatrix();
        glScalef(SIZE_BOX, SIZE_BOX, SIZE_BOX);

        glNormal3f(0,0,-1);
	    glBegin(GL_QUADS); // jos
            glTexCoord2f(0, 0);   glVertex3f(0, 0, 0);
            glTexCoord2f(1, 0);   glVertex3f(SIZE_BOX,  0, 0);
            glTexCoord2f(1, 1);   glVertex3f(SIZE_BOX,  SIZE_BOX, 0);
            glTexCoord2f(0, 1);   glVertex3f(0, SIZE_BOX, 0);
        glEnd();

        glNormal3f(0,0,1); // sus
	    glBegin(GL_QUADS);
            glTexCoord2f(0, 0);   glVertex3f(0, 0, SIZE_BOX);
            glTexCoord2f(1, 0);   glVertex3f(SIZE_BOX,  0, SIZE_BOX);
            glTexCoord2f(1, 1);   glVertex3f(SIZE_BOX,  SIZE_BOX, SIZE_BOX);
            glTexCoord2f(0, 1);   glVertex3f(0, SIZE_BOX, SIZE_BOX);
        glEnd();

        glNormal3f(-1,0,0);
	    glBegin(GL_QUADS);
            glTexCoord2f(0, 0);   glVertex3f(0, 0, 0);
            glTexCoord2f(1, 0);   glVertex3f(SIZE_BOX,  0, 0);
            glTexCoord2f(1, 1);   glVertex3f(SIZE_BOX,  0, SIZE_BOX);
            glTexCoord2f(0, 1);   glVertex3f(0, 0, SIZE_BOX);
        glEnd();

        glNormal3f(1,0,0);
	    glBegin(GL_QUADS);
            glTexCoord2f(0, 0);   glVertex3f(0, SIZE_BOX, 0);
            glTexCoord2f(1, 0);   glVertex3f(SIZE_BOX, SIZE_BOX, 0);
            glTexCoord2f(1, 1);   glVertex3f(SIZE_BOX,  SIZE_BOX, SIZE_BOX);
            glTexCoord2f(0, 1);   glVertex3f(0,SIZE_BOX, SIZE_BOX);
        glEnd();

        glNormal3f(0,-1,0);
	    glBegin(GL_QUADS);
            glTexCoord2f(0, 0);   glVertex3f(0, 0, 0);
            glTexCoord2f(1, 0);   glVertex3f(0, 0, SIZE_BOX);
            glTexCoord2f(1, 1);   glVertex3f(0, SIZE_BOX, SIZE_BOX);
            glTexCoord2f(0, 1);   glVertex3f(0,SIZE_BOX, 0);
        glEnd();

        glNormal3f(0,1,0);
	    glBegin(GL_QUADS);
            glTexCoord2f(0, 0);   glVertex3f(SIZE_BOX, 0, 0);
            glTexCoord2f(1, 0);   glVertex3f(SIZE_BOX, 0, SIZE_BOX);
            glTexCoord2f(1, 1);   glVertex3f(SIZE_BOX, SIZE_BOX, SIZE_BOX);
            glTexCoord2f(0, 1);   glVertex3f(SIZE_BOX,SIZE_BOX, 0);
        glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void draw_cross()
{
    glPushMatrix();
        glScalef(0.02f, 0.02f, 1);
        glColor3f(0,0,0);
        glLineWidth(1);
        glBegin(GL_LINES);
            glColor3f(1,1,1);
            glVertex3f(-1,0,-2);
            glVertex3f(1,0,-2);
            glVertex3f(0,-1,-2);
            glVertex3f(0,1,-2);
        glEnd();
    glPopMatrix();
}

#define SIZE_MAP 16
int map[SIZE_MAP][SIZE_MAP][SIZE_MAP] = {0};

int tex_moss;
int tex_iron;

void draw_map()
{

    for(int x = 0; x < SIZE_MAP; x++)
    {
        for(int y = 0; y < SIZE_MAP; y++)
        {
            for(int z = 0; z < SIZE_MAP; z++)
            {
                if(map[x][y][z])
                {
                    glPushMatrix();
                        glTranslatef(x*SIZE_BOX, y*SIZE_BOX, z*SIZE_BOX);
                        glEnable(GL_TEXTURE_2D);
                        if(map[x][y][z] == 1) glBindTexture(GL_TEXTURE_2D, tex_iron);
                        else if(map[x][y][z] == 2) glBindTexture(GL_TEXTURE_2D, tex_moss);
                        draw_box();
                    glPopMatrix();
                }

            }
        }
    }
}

int is_in_map(int x, int y, int z)
{
    return x >= 0 && x < SIZE_MAP && y >= 0 && y < SIZE_MAP && z >= 0 && z < SIZE_MAP;
}

int touch()
{
    int btn = 0;
    if(GetKeyState(VK_LBUTTON) < 0) btn = 1;
    else if (GetKeyState(VK_RBUTTON) < 0) btn = 2;

    if(btn)
    {
        float x = camera.x, y = camera.y, z = camera.z;
        int xMap, yMap, zMap, xMapOld, yMapOld, zMapOld;
        int dist = 0;

        float zAngle = -camera.zRot/180*M_PI;
        float xAngle = camera.xRot/180*M_PI;

        float speedXY = sin(xAngle);

        while(dist++ < 100)
        {
            x += sin(zAngle)*speedXY/10;
            y += cos(zAngle)*speedXY/10;
            z -= cos(xAngle)/10;

            xMap = (int)(x/SIZE_BOX); yMap = (int)(y/SIZE_BOX); zMap = (int)(z/SIZE_BOX);

            if(is_in_map(xMap, yMap, zMap))
            {
                if(map[xMap][yMap][zMap])
                {
                    if(btn == 1)
                    {
                        if(is_in_map(xMapOld, yMapOld, zMapOld)) map[xMapOld][yMapOld][zMapOld] = 1;
                    }
                    else if (btn == 2) map[xMap][yMap][zMap] = 0;
                    return 1;
                }
                xMapOld = xMap; yMapOld = yMap; zMapOld = zMap;
            }
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          800,
                          600,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

//----------------------------------------------------------------------------------

    for(int i = 0; i < SIZE_MAP; i++)
        for(int j = 0; j < SIZE_MAP; j++)
            map[i][j][0] = 2;

    load_texture("moss_block.png", &tex_moss);
    load_texture("iron_block.png", &tex_iron);

    float timer = 0;
    float tempTime = clock();


    RECT rct;
    GetClientRect(hwnd, &rct);
    resize_window(rct.right, rct.bottom);


    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT, GL_LINE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    GLfloat posLight[] = {0,0,0,0};

//----------------------------------------------------------------------------------
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.3f, 0.7f, 1, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            timer += clock() - tempTime;
            tempTime = clock();

            glPushMatrix();


                if(GetForegroundWindow() == hwnd)
                {
                    move_camera_with_mouse(0.2, 0.2);
                    move_camera_with_button(0.1);
                    //printf("(X:%.1f, Y:%.1f, Z:%.1f, xR:%.1f, zR:%.1f)\n", camera.x, camera.y, camera.z, camera.xRot, camera.zRot);
                }
                apply_camera(camera);


                posLight[0] = 7-cos(theta)*10;
                posLight[1] = posLight[0];
                posLight[2] = sin(theta)*13;

                glEnable(GL_LIGHTING);
                    glLightfv(GL_LIGHT0, GL_POSITION, posLight);
                    draw_map();
                glDisable(GL_LIGHTING);

                glTranslatef(posLight[0],posLight[1],posLight[2]);
                glColor3f(1,1,0);
                draw_box();

                if(timer > 200) if(touch()) timer = 0;

            glPopMatrix();

            draw_cross();


            SwapBuffers(hDC);

            theta += 0.001;
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        //---------------------------------------------------------------------------------
        case WM_SIZE:
            windowWidth = LOWORD(lParam);
            windowHeight = HIWORD(lParam);
            resize_window(windowWidth, windowHeight);
        break;

        case WM_SETCURSOR:
            ShowCursor(FALSE);
        break;
        //-----------------------------------------------------------------------------------
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
