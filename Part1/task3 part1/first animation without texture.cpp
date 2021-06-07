#include <GL/glut.h>
#include <math.h>

#define PI 3.14
GLfloat angle = 0.0;   /* in degrees */
GLfloat angle2 = 0.0;   /* in degrees */
double eye[] = { 0.0, 0.0, 5.0 };
double center[] = { 0.0, 0.0, 0.0 };
double up[] = { 0.0, 1.0, 0.0 };
static int shoulder = 0, LA = -90, LA2 = 0, LAR = 0, RA = -90, RA2 = 0, RAR = 0, LL = 0, LL2 = 0, RL = 0, RL2 = 0;
int moving, startx, starty;

int poses[5][11] = {
    {0,90,0,0,90,0,0,0,0,0,0},
    {0,90,0,0,90,0,0,45,0,0,0},
    {0,90,0,45,90,0,-45,60,40,-60,40},
    {0,90,0,0,90,0,0,0,0,45,0},
    {0,90,0,-45,90,0,45,-60,40,60,40}
    
};


void SetPoses(int frameNum) {
    shoulder = poses[frameNum][0];
    LA = poses[frameNum][1];
    LA2 = poses[frameNum][2];
    LAR = poses[frameNum][3];
    RA = poses[frameNum][4];
    RA2 = poses[frameNum][5];
    RAR = poses[frameNum][6];
    LL = poses[frameNum][7];
    LL2 = poses[frameNum][8];
    RL = poses[frameNum][9];
    RL2 = poses[frameNum][10];






}
static int f = 0;
void timer(int value) {
    f = f % 5;
    SetPoses(f);
    f++;
    glutPostRedisplay();
    glutTimerFunc(150, timer, 0);
}




void rotatePoint(double a[], double theta, double p[])
{

    double temp[3];
    temp[0] = p[0];
    temp[1] = p[1];
    temp[2] = p[2];

    temp[0] = -a[2] * p[1] + a[1] * p[2];
    temp[1] = a[2] * p[0] - a[0] * p[2];
    temp[2] = -a[1] * p[0] + a[0] * p[1];

    temp[0] *= sin(theta);
    temp[1] *= sin(theta);
    temp[2] *= sin(theta);

    temp[0] += (1 - cos(theta)) * (a[0] * a[0] * p[0] + a[0] * a[1] * p[1] + a[0] * a[2] * p[2]);
    temp[1] += (1 - cos(theta)) * (a[0] * a[1] * p[0] + a[1] * a[1] * p[1] + a[1] * a[2] * p[2]);
    temp[2] += (1 - cos(theta)) * (a[0] * a[2] * p[0] + a[1] * a[2] * p[1] + a[2] * a[2] * p[2]);

    temp[0] += cos(theta) * p[0];
    temp[1] += cos(theta) * p[1];
    temp[2] += cos(theta) * p[2];

    p[0] = temp[0];
    p[1] = temp[1];
    p[2] = temp[2];
}

void crossProduct(double a[], double b[], double c[])
{
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(double a[])
{
    double norm;
    norm = a[0] * a[0] + a[1] * a[1] + a[2] * a[2];
    norm = sqrt(norm);
    a[0] /= norm;
    a[1] /= norm;
    a[2] /= norm;
}

// Rotation about vertical direction
void lookRight()
{
    rotatePoint(up, PI / 8, eye);
}

void lookLeft()
{
    rotatePoint(up, -PI / 8, eye);
}

// Rotation about horizontal direction

void lookUp()
{
    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, PI / 8, eye);
    rotatePoint(horizontal, PI / 8, up);
}

void lookDown()
{
    double horizontal[3];
    double look[] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    crossProduct(up, look, horizontal);
    normalize(horizontal);
    rotatePoint(horizontal, -PI / 8, eye);
    rotatePoint(horizontal, -PI / 8, up);
}

// Forward and Backward
void moveForward()
{
    double direction[3];
    direction[0] = center[0] - eye[0];
    direction[1] = center[1] - eye[1];
    direction[2] = center[2] - eye[2];
    float speed = 0.1;
    eye[0] += direction[0] * speed;
    eye[1] += direction[1] * speed;
    eye[2] += direction[2] * speed;

    center[0] += direction[0] * speed;
    center[1] += direction[1] * speed;
    center[2] += direction[2] * speed;
}

void moveBackward()
{
    double direction[3];
    direction[0] = center[0] + eye[0];
    direction[1] = center[1] + eye[1];
    direction[2] = center[2] + eye[2];
    float speed = 0.1;
    eye[0] += direction[0] * speed;
    eye[1] += direction[1] * speed;
    eye[2] += direction[2] * speed;

    center[0] += direction[0] * speed;
    center[1] += direction[1] * speed;
    center[2] += direction[2] * speed;
}

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2],
        center[0], center[1], center[2],
        up[0], up[1], up[2]);
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glPushMatrix();
    glScalef(1.0, 2.0, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    //  create Left arm

    //glTranslatef(-1.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.5, 0.875, 0.0);
    glRotatef((GLfloat)LAR, 1, 0, 0);
    glTranslatef(-0.5, -0.875, 0);
    glPushMatrix();
    glTranslatef(0.5, 0.875, 0.0);
    glRotatef((GLfloat)LA, 0, 0, -1.0);
    glTranslatef(-0.5, -0.875, 0);
    glTranslatef(1, 0.875, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0.5, 0, 0.0);
    glRotatef((GLfloat)LA2, 0.0, 0.0, 1.0);
    glTranslatef(-0.5, 0, 0.0);
    glTranslatef(1, 0, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    // finger 1

    glPushMatrix();

    glTranslatef(0.575, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 2

    glPushMatrix();

    glTranslatef(0.575, 0.0, 0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 3

    glPushMatrix();

    glTranslatef(0.575, 0.0, 0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 4
    glPushMatrix();

    glTranslatef(0.575, 0.0, -0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 5

    glPushMatrix();
    glTranslatef(0.575, 0.0, -0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();

    //Create Right Arm
    glPushMatrix();
    glTranslatef(-0.5, 0.875, 0.0);
    glRotatef((GLfloat)RAR, 1.0, 0.0, 0);
    glTranslatef(0.5, -0.875, 0);
    glPushMatrix();
    glTranslatef(-0.5, 0.875, 0.0);
    glRotatef((GLfloat)RA, 0.0, 0.0, 1.0);
    glTranslatef(0.5, -0.875, 0);
    glTranslatef(-1, 0.875, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(-0.5, 0, 0.0);
    glRotatef((GLfloat)RA2, 0.0, 0.0, 1.0);
    glTranslatef(0.5, 0, 0.0);
    glTranslatef(-1, 0, 0.0);
    glPushMatrix();
    glScalef(1.0, 0.25, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    // finger 1
    glPushMatrix();

    glTranslatef(-0.575, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0.0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 2

    glPushMatrix();

    glTranslatef(-0.575, 0.0, 0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 3

    glPushMatrix();

    glTranslatef(-0.575, 0.0, 0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 4
    glPushMatrix();

    glTranslatef(-0.575, 0.0, -0.08);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    // finger 5

    glPushMatrix();

    glTranslatef(-0.575, 0.0, -0.155);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();

    glTranslatef(-0.15, 0.0, 0);
    glPushMatrix();
    glScalef(0.15, 0.1, 0.075);
    glutWireCube(1);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
    //Create left Leg
    glPushMatrix();
    glTranslatef(0.375, -1.25, 0.0);
    glRotatef((GLfloat)LL, 1.0, 0.0, 0.0);
    glTranslatef(-0.375, 1.25, 0.0);
    glTranslatef(0.375, -1.5, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.5, 0.0);
    glRotatef((GLfloat)LL2, 1.0, 0.0, 0.0);
    glTranslatef(0, 0.5, 0.0);
    glTranslatef(0, -1, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.65, 0.25);
    glPushMatrix();
    glScalef(0.25, 0.3, 1);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //Create Right Leg
    glPushMatrix();
    glTranslatef(-0.375, -1.25, 0.0);
    glRotatef((GLfloat)RL, 1.0, 0.0, 0);
    glTranslatef(0.375, 1.25, 0.0);
    glTranslatef(-0.375, -1.5, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.5, 0.0);
    glRotatef((GLfloat)RL2, 1.0, 0.0, 0);
    glTranslatef(0, 0.5, 0.0);
    glTranslatef(0, -1, 0.0);
    glPushMatrix();
    glScalef(0.25, 1, 0.5);
    glutWireCube(1.0);
    glPopMatrix();

    glTranslatef(0, -0.65, 0.25);
    glPushMatrix();
    glScalef(0.25, 0.3, 1);
    glutWireCube(1.0);
    glPopMatrix();

    glPopMatrix();

    //Create Head
    glPushMatrix();
    glTranslatef(0, 1.5, 0.0);
    glPushMatrix();
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();

    glPopMatrix();
    glutSwapBuffers();
}

void init(void)
{
    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0,
        1.0,
        1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
}

void reset()
{
    double e[] = { 0.0, 0.0, 5.0 };
    double c[] = { 0.0, 0.0, 0.0 };
    double u[] = { 0.0, 1.0, 0.0 };
    for (int i = 0; i < 3; i++)
    {
        eye[i] = e[i];
        center[i] = c[i];
        up[i] = u[i];
    }
}
void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        lookLeft();
        break;
    case GLUT_KEY_RIGHT:
        lookRight();
        break;
    case GLUT_KEY_UP:
        lookUp();
        break;
    case GLUT_KEY_DOWN:
        lookDown();
        break;
    }
    glutPostRedisplay();
}

void keyBoard(unsigned char key, int x, int y)
{
    switch (key)

    {
    case 'q':
        if (LA < 90)
        {
            LA = LA + 5;
        }
        break;
    case 'Q':
        if (LA > -90)
        {
            LA = LA - 5;
        }
        break;
    case 't':
        if (LA2 < 120)
        {
            LA2 = LA2 + 5;
        }
        break;
    case 'T':
        if (LA2 > 0)
        {
            LA2 = LA2 - 5;
        }
        break;
    case 'o':
        if (LAR < 180)
        {
            LAR = LAR + 5;
        }
        break;
    case 'O':
        if (LAR > 0)
        {
            LAR = LAR - 5;
        }
        break;
    case 'w':
        if (RA < 90)
        {
            RA = RA + 5;
        }
        break;
    case 'W':
        if (RA > -90)
        {
            RA = RA - 5;
        }
        break;
    case 'y':
        if (RA2 < 120)
        {
            RA2 = RA2 + 5;
        }
        break;
    case 'Y':
        if (RA2 > -120)
        {
            RA2 = RA2 - 5;
        }
        break;
    case 'p':
        if (RAR < 180)
        {
            RAR = RAR + 5;
        }
        break;
    case 'P':
        if (RAR > 0)
        {
            RAR = RAR - 5;
        }
        break;
    case 'e':
        if (LL < 90)
        {
            LL = LL + 5;
        }
        break;
    case 'E':
        if (LL > -90)
        {
            LL = LL - 5;
        }
        break;
    case 'u':
        if (LL2 < 90)
        {
            LL2 = LL2 + 5;
        }
        break;
    case 'U':
        if (LL2 > 0)
        {
            LL2 = LL2 - 5;
        }
        break;
    case 'r':
        if (RL < 90)
        {
            RL = RL + 5;
        }
        break;
    case 'R':
        if (RL > -90)
        {
            RL = RL - 5;
        }
        break;
    case 'i':
        if (RL2 < 90)
        {
            RL2 = RL2 + 5;
        }
        break;
    case 'I':
        if (RL2 > 0)
        {
            RL2 = RL2 - 5;
        }
        break;
    case 'z':
        moveBackward();
        break;
    case 'Z':
        moveForward();
        break;
    case 'd':
        reset();
        break;
    }
    glutPostRedisplay();
}
static void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            moving = 1;
            startx = x;
            starty = y;
        }
        if (state == GLUT_UP) {
            moving = 0;
        }
    }
}
static void motion(int x, int y)
{
    if (moving) {
        angle = angle + (x - startx);
        angle2 = angle2 + (y - starty);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Robot");
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyBoard);
    glutSpecialFunc(specialKeys);
    glutDisplayFunc(display);
    init();
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
