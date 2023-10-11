#include <windows.h>
#include <GL/gl.h>
#pragma comment(lib,"OpenGL32.lib")
#include <GL/glu.h>
#pragma comment(lib,"Glu32.lib")
#pragma comment (lib, "legacy_stdio_definitions.lib")
#include "glaux.h"
#pragma comment(lib,"Glaux.lib")

#include <math.h>
#define RADGRAD 0.0174532925199433


#define MAXH 100
#define MAXS 100
GLdouble MinX, MaxX, MinY, MaxY, NearZ, FarZ; // For Visible space memorization
GLint CurrentVp[4]; // For Viewport memorization
AUX_RGBImageRec* pimage; // For Background
AUX_RGBImageRec* TextureImage; // For Texture
unsigned int TextureID;
struct Color
{
    unsigned char r, g, b;
};



struct point2
{
    double x, y;
    point2(double initx = 0.0, double inity = 0.0)
    {
        x = initx;
        y = inity;
    }
};
//
// ============= Cone from segment (x1, y1, x2, y2, z = 1.0) =======================
//
class tr0
{
protected:
    int nh;
    int ns;
    double x1, y1, x2, y2;
    double c[MAXH][MAXS][3];
public:
    tr0(double, double, double, double, int, int);
    tr0(point2&, point2&, int, int);
    void draw();
};
tr0::tr0(double x1i, double y1i, double x2i, double y2i, int heightsegm, int sectors)
{
    int i, j;
    nh = heightsegm;
    ns = sectors;
    x1 = x1i;
    y1 = y1i;
    x2 = x2i;
    y2 = y2i;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    double hstep = 1.0 / nh;
    double tstep = 1.0 / (ns - 1);
    double t, hcur;
    t = 0.0;
    for (j = 0; j < ns; j++)
    {
        c[nh - 1][j][0] = ((1.0 - t) * x1 + t * x2) * 1.0;
        c[nh - 1][j][1] = ((1.0 - t) * y1 + t * y2) * 1.0;
        c[nh - 1][j][2] = 1.0;
        if (j < ns - 1)
            t += tstep;
        else
            t = 1.0;
    }
    hcur = 0.0;
    for (i = 0; i < nh - 1; i++)
    {
        hcur += hstep;
        for (j = 0; j < ns; j++)
        {
            c[i][j][0] = c[nh - 1][j][0] * hcur;
            c[i][j][1] = c[nh - 1][j][1] * hcur;
            c[i][j][2] = hcur;
        }
    }
}
tr0::tr0(point2& p1, point2& p2, int heightsegm, int sectors)
{
    int i, j;
    nh = heightsegm;
    ns = sectors;
    x1 = p1.x;
    y1 = p1.y;
    x2 = p2.x;
    y2 = p2.y;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    double hstep = 1.0 / nh;
    double tstep = 1.0 / (ns - 1);
    double t, hcur;
    t = 0.0;
    for (j = 0; j < ns; j++)
    {
        c[nh - 1][j][0] = ((1.0 - t) * x1 + t * x2) * 1.0;
        c[nh - 1][j][1] = ((1.0 - t) * y1 + t * y2) * 1.0;
        c[nh - 1][j][2] = 1.0;
        if (j < ns - 1)
            t += tstep;
        else
            t = 1.0;
    }
    hcur = 0.0;
    for (i = 0; i < nh - 1; i++)
    {
        hcur += hstep;
        for (j = 0; j < ns; j++)
        {
            c[i][j][0] = c[nh - 1][j][0] * hcur;
            c[i][j][1] = c[nh - 1][j][1] * hcur;
            c[i][j][2] = hcur;
        }
    }
}
void tr0::draw()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    for (j = 0; j < ns - 1; j++)
    {
        glVertex3dv(v);
        glVertex3dv(c[0][j]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            glVertex3dv(c[i][j]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}

//
// ============= Cone from Bezier3 (p0, p1, p2, p3, z = 1.0) =====================
//
class Bezier3
{
protected:
    int nh;
    int ns;
    point2 p[4];
    double a[4], b[4];
    double c[MAXH][MAXS][3];
public:
    Bezier3(point2*, int, int);
    void draw();
};
Bezier3::Bezier3(point2 cp[4], int heightsegm, int sectors)
{
    int i, j;
    nh = heightsegm;
    ns = sectors;
    for (i = 0; i < 4; i++)
        p[i] = cp[i];
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    double hstep = 1.0 / nh;
    double tstep = 1.0 / (ns - 1);
    double t, hcur;
    a[0] = -p[0].x + 3.0 * p[1].x - 3.0 * p[2].x + p[3].x;
    a[1] = 3.0 * p[0].x - 6.0 * p[1].x + 3.0 * p[2].x;
    a[2] = -3.0 * p[0].x + 3.0 * p[1].x;
    a[3] = p[0].x;
    b[0] = -p[0].y + 3.0 * p[1].y - 3.0 * p[2].y + p[3].y;
    b[1] = 3.0 * p[0].y - 6.0 * p[1].y + 3.0 * p[2].y;
    b[2] = -3.0 * p[0].y + 3.0 * p[1].y;
    b[3] = p[0].y;
    t = 0.0;
    for (j = 0; j < ns - 1; j++)
    {
        c[nh - 1][j][0] = ((a[0] * t + a[1]) * t + a[2]) * t + a[3];
        c[nh - 1][j][1] = ((b[0] * t + b[1]) * t + b[2]) * t + b[3];
        c[nh - 1][j][2] = 1.0;
        t += tstep;
    }
    c[nh - 1][ns - 1][0] = p[3].x;
    c[nh - 1][ns - 1][1] = p[3].y;
    c[nh - 1][ns - 1][2] = 1.0;
    hcur = 0.0;
    for (i = 0; i < nh - 1; i++)
    {
        hcur += hstep;
        for (j = 0; j < ns; j++)
        {
            c[i][j][0] = c[nh - 1][j][0] * hcur;
            c[i][j][1] = c[nh - 1][j][1] * hcur;
            c[i][j][2] = hcur;
        }
    }
}
void Bezier3::draw()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    for (j = 0; j < ns - 1; j++)
    {
        glVertex3dv(v);
        glVertex3dv(c[0][j]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            glVertex3dv(c[i][j]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
class arc
{
protected:
    int nh;
    int ns;
    double c[MAXH][MAXS][3];
    double r, stangle, endangle;
    point2 center;
public:
    arc(point2, double, double, double, int, int);
    void draw();
};
arc::arc(point2 init_center, double init_r, double init_start, double init_end,
    int heightsegm, int sectors)
{
    int i, j;
    nh = heightsegm;
    ns = sectors;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    r = init_r;
    stangle = init_start;
    endangle = init_end;
    center.x = init_center.x;
    center.y = init_center.y;
    double hstep = 1.0 / nh;
    double astep = (endangle - stangle) / (ns - 1);
    double hcur, acur;
    acur = stangle;
    for (j = 0; j < ns; j++)
    {
        c[nh - 1][j][0] = r * cos(RADGRAD * acur) + center.x;
        c[nh - 1][j][1] = r * sin(RADGRAD * acur) + center.y;
        c[nh - 1][j][2] = 1.0;
        acur += astep;
    }
    hcur = 0.0;
    for (i = 0; i < nh - 1; i++)
    {
        hcur += hstep;
        for (j = 0; j < ns; j++)
        {
            c[i][j][0] = c[nh - 1][j][0] * hcur;
            c[i][j][1] = c[nh - 1][j][1] * hcur;
            c[i][j][2] = hcur;
        }
    }
}
void arc::draw()
{
    int i, j;
    glBegin(GL_TRIANGLES);
    for (j = 0; j < ns - 1; j++)
    {
        glVertex3d(0.0, 0.0, 0.0);
        glVertex3dv(c[0][j]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            glVertex3dv(c[i][j]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
class coneA2
{
protected:
    int nh;
    int ns;
    arc* a1, * a2, * a3, * a4;
    tr0* t1, * t2, * t3, * t4;
public:
    coneA2(int, int);
    void draw();
};
coneA2::coneA2(int heightsegm, int sectors)
{
    nh = heightsegm;
    ns = sectors;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;


    t1 = new tr0(2.0, 1.0, 2.0, -1.0, nh, ns / 8);
    t2 = new tr0(1.0, -2.0, -1.0, -2.0, nh, ns / 8);
    t3 = new tr0(-2.0, -1.0, -2.0, 1.0, nh, ns / 8);
    t4 = new tr0(-1.0, 2.0, 1.0, 2.0, nh, ns / 8);


    a1 = new arc(point2(2.0, 2.0), 1.0, 0.0, 360.0, nh, ns / 8);
    a2 = new arc(point2(2.0, -2.0), 1.0, 0.0, 360.0, nh, ns / 8);
    a3 = new arc(point2(-2.0, -2.0), 1.0, 0.0, 360.0, nh, ns / 8);
    a4 = new arc(point2(-2.0, 2.0), 1.0, 0.0, 360.0, nh, ns / 8);

}
void coneA2::draw()
{
    a1->draw(); // Draw a cone based on arc
    t1->draw(); // Draw a cone based on segment
    a2->draw(); // Draw a cone based on arc
    t2->draw(); // Draw a cone based on segment
    a3->draw(); // Draw a cone based on arc
    t3->draw(); // Draw a cone based on segment
    a4->draw(); // Draw a cone based on arc
    t4->draw(); // Draw a cone based on segment
}

//
// ========== Cone A2_inv ==================================
// (Half own surface inverted in the base on arcs and segments)
//
class coneA2_inv
{
protected:
    int nh;
    int ns;
    arc* a1, * a2, * a3, * a4;
    tr0* t1, * t2, * t3, * t4;
public:
    coneA2_inv(int, int);
    void draw();
};
coneA2_inv::coneA2_inv(int heightsegm, int sectors)
{
    nh = heightsegm;
    ns = sectors;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    a1 = new arc(point2(2.0, 2.0), 1.0, -90.0, 180.0, nh, ns / 8);
    t1 = new tr0(1.0, 2.0, -1.0, 2.0, nh, ns / 8);
    a2 = new arc(point2(-2.0, 2.0), 1.0, 0.0, 270.0, nh, ns / 8);
    t2 = new tr0(-2.0, 1.0, -2.0, -1.0, nh, ns / 8);
    a3 = new arc(point2(-2.0, -2.0), 1.0, 90.0, 360.0, nh, ns / 8);
    t3 = new tr0(-1.0, -2.0, 1.0, -2.0, nh, ns / 8);
    a4 = new arc(point2(2.0, -2.0), 1.0, -180.0, 90.0, nh, ns / 8);
    t4 = new tr0(2.0, -1.0, 2.0, 1.0, nh, ns / 8);
}
void coneA2_inv::draw()
{
    a1->draw(); // Draw a cone based on arc
    t1->draw(); // Draw a cone based on segment
    a2->draw(); // Draw a cone based on arc
    t2->draw(); // Draw a cone based on segment
    a3->draw(); // Draw a cone based on arc
    t3->draw(); // Draw a cone based on segment
    a4->draw(); // Draw a cone based on arc
    t4->draw(); // Draw a cone based on segment
}
GLdouble* norm(const GLdouble p1[3], const GLdouble p2[3], const GLdouble p3[3])
{
    static GLdouble p[3];
    p[0] = (p1[1] * (p2[2] - p3[2]) + p2[1] * (p3[2] - p1[2]) + p3[1] * (p1[2] - p2[2]));
    p[1] = (p1[2] * (p2[0] - p3[0]) + p2[2] * (p3[0] - p1[0]) + p3[2] * (p1[0] - p2[0]));
    p[2] = (p1[0] * (p2[1] - p3[1]) + p2[0] * (p3[1] - p1[1]) + p3[0] * (p1[1] - p2[1]));
    return p;
}

GLdouble* norm_inv(const GLdouble p1[3], const GLdouble p2[3], const GLdouble p3[3])
{
    static GLdouble p[3];
    GLdouble* temp = norm(p1, p2, p3);
    p[0] = -temp[0];
    p[1] = -temp[1];
    p[2] = -temp[2];
    return p;
}

//
// ============= Cone from segment with normal vectors =======================
//
class tr0_norm : public tr0
{
public:
    tr0_norm(double, double, double, double, int, int);
    tr0_norm(point2&, point2&, int, int);
    void draw();
    void draw_inv();
};
tr0_norm::tr0_norm(double x1i, double y1i, double x2i, double y2i,
    int heightsegm, int sectors) :
    tr0(x1i, y1i, x2i, y2i, heightsegm, sectors)
{
}
tr0_norm::tr0_norm(point2& p1, point2& p2, int heightsegm, int sectors) :
    tr0(p1, p2, heightsegm, sectors)
{
}
void tr0_norm::draw()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    glNormal3dv(norm(v, c[nh - 1][0], c[nh - 1][ns - 1]));
    for (j = 0; j < ns - 1; j++)
    {
        // glNormal3dv(norm(v, c[0][j], c[0][j + 1]));
        glVertex3dv(v);
        glVertex3dv(c[0][j]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            // glNormal3dv(norm(c[i][j], c[i + 1][j], c[i][j + 1]));
            glVertex3dv(c[i][j]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i][j + 1]);
            // glNormal3dv(norm(c[i][j + 1], c[i + 1][j], c[i + 1][j + 1]));
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
void tr0_norm::draw_inv()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    glNormal3dv(norm_inv(v, c[nh - 1][0], c[nh - 1][ns - 1]));
    for (j = 0; j < ns - 1; j++)
    {
        // glNormal3dv(norm_inv(v, c[0][j], c[0][j + 1]));
        glVertex3dv(v);
        glVertex3dv(c[0][j]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            // glNormal3dv(norm_inv(c[i][j], c[i + 1][j], c[i][j + 1]));
            glVertex3dv(c[i][j]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i][j + 1]);
            // glNormal3dv(norm_inv(c[i][j + 1], c[i + 1][j], c[i + 1][j + 1]));
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
//
// ========== Cone from arc with normal vectors ==================================
//
class arc_norm : public arc
{
public:
    arc_norm(point2, double, double, double, int, int);
    void draw();
    void draw_inv();
};
arc_norm::arc_norm(point2 init_center, double init_r, double init_start, double init_end,
    int heightsegm, int sectors) :
    arc(init_center, init_r, init_start, init_end, heightsegm, sectors)
{
}
void arc_norm::draw()
{
    int i, j;
    glBegin(GL_TRIANGLES);
    for (j = 0; j < ns - 1; j++)
    {
        glNormal3d((c[0][j][0] + c[0][j + 1][0]) * 0.5,
            (c[0][j][1] + c[0][j + 1][1]) * 0.5,
            -(c[0][j][2] + c[0][j + 1][2]) * 0.5);
        glVertex3d(0.0, 0.0, 0.0);
        glNormal3d(c[0][j][0], c[0][j][1], -c[0][j][2]);
        glVertex3dv(c[0][j]);
        glNormal3d(c[0][j + 1][0], c[0][j + 1][1], -c[0][j + 1][2]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            glNormal3d(c[i][j][0], c[i][j][1], -c[i][j][2]);
            glVertex3dv(c[i][j]);
            glNormal3d(c[i + 1][j][0], c[i + 1][j][1], -c[i + 1][j][2]);
            glVertex3dv(c[i + 1][j]);
            glNormal3d(c[i][j + 1][0], c[i][j + 1][1], -c[i][j + 1][2]);
            glVertex3dv(c[i][j + 1]);
            glNormal3d(c[i][j + 1][0], c[i][j + 1][1], -c[i][j + 1][2]);
            glVertex3dv(c[i][j + 1]);
            glNormal3d(c[i + 1][j][0], c[i + 1][j][1], -c[i + 1][j][2]);
            glVertex3dv(c[i + 1][j]);
            glNormal3d(c[i + 1][j + 1][0], c[i + 1][j + 1][1],
                -c[i + 1][j + 1][2]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
void arc_norm::draw_inv()
{
    int i, j;
    glBegin(GL_TRIANGLES);
    for (j = 0; j < ns - 1; j++)
    {
        glNormal3d(-(c[0][j][0] + c[0][j + 1][0]) * 0.5,
            -(c[0][j][1] + c[0][j + 1][1]) * 0.5,
            (c[0][j][2] + c[0][j + 1][2]) * 0.5);
        glVertex3d(0.0, 0.0, 0.0);
        glNormal3d(-c[0][j][0], -c[0][j][1], c[0][j][2]);
        glVertex3dv(c[0][j]);
        glNormal3d(-c[0][j + 1][0], -c[0][j + 1][1], c[0][j + 1][2]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            glNormal3d(-c[i][j][0], -c[i][j][1], c[i][j][2]);
            glVertex3dv(c[i][j]);
            glNormal3d(-c[i + 1][j][0], -c[i + 1][j][1], c[i + 1][j][2]);
            glVertex3dv(c[i + 1][j]);
            glNormal3d(-c[i][j + 1][0], -c[i][j + 1][1], c[i][j + 1][2]);
            glVertex3dv(c[i][j + 1]);
            glNormal3d(-c[i][j + 1][0], -c[i][j + 1][1], c[i][j + 1][2]);
            glVertex3dv(c[i][j + 1]);
            glNormal3d(-c[i + 1][j][0], -c[i + 1][j][1], c[i + 1][j][2]);
            glVertex3dv(c[i + 1][j]);
            glNormal3d(-c[i + 1][j + 1][0], -c[i + 1][j + 1][1],
                c[i + 1][j + 1][2]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}

//
// ========== Cone A2n with normal vectors ==================================
// (Half own surface in the base on arcs and segments with normal vectors) 
//
class coneA2n
{
protected:
    int nh;
    int ns;
    arc_norm* a1, * a2, * a3, * a4;
    tr0_norm* t1, * t2, * t3, * t4;
public:
    coneA2n(int, int);
    void draw();
};
coneA2n::coneA2n(int heightsegm, int sectors)
{
    nh = heightsegm;
    ns = sectors;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    a1 = new arc_norm(point2(2.0, 2.0), 1.0, 180.0, -90.0, nh, ns / 8);
    t1 = new tr0_norm(2.0, 1.0, 2.0, -1.0, nh, ns / 8);
    a2 = new arc_norm(point2(2.0, -2.0), 1.0, 90.0, -180.0, nh, ns / 8);
    t2 = new tr0_norm(1.0, -2.0, -1.0, -2.0, nh, ns / 8);
    a3 = new arc_norm(point2(-2.0, -2.0), 1.0, 360.0, 90.0, nh, ns / 8);
    t3 = new tr0_norm(-2.0, -1.0, -2.0, 1.0, nh, ns / 8);
    a4 = new arc_norm(point2(-2.0, 2.0), 1.0, 270.0, 0.0, nh, ns / 8);
    t4 = new tr0_norm(-1.0, 2.0, 1.0, 2.0, nh, ns / 8);
}
void coneA2n::draw()
{
    a1->draw(); // Draw a cone based on arc
    t1->draw(); // Draw a cone based on segment
    a2->draw(); // Draw a cone based on arc
    t2->draw(); // Draw a cone based on segment
    a3->draw(); // Draw a cone based on arc
    t3->draw(); // Draw a cone based on segment
    a4->draw(); // Draw a cone based on arc
    t4->draw(); // Draw a cone based on segment
}
//
// ========== Inverted cone A2n with normal vectors ==================================
// (Half own surface inverted in the base on arcs and segments with normal vectors) 
//
class coneA2n_inv
{
protected:
    int nh;
    int ns;
    arc_norm* a1, * a2, * a3, * a4;
    tr0_norm* t1, * t2, * t3, * t4;
public:
    coneA2n_inv(int, int);
    void draw();
};
coneA2n_inv::coneA2n_inv(int heightsegm, int sectors)
{
    nh = heightsegm;
    ns = sectors;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    a1 = new arc_norm(point2(2.0, 2.0), 1.0, -90.0, 180.0, nh, ns / 8);
    t1 = new tr0_norm(1.0, 2.0, -1.0, 2.0, nh, ns / 8);
    a2 = new arc_norm(point2(-2.0, 2.0), 1.0, 0.0, 270.0, nh, ns / 8);
    t2 = new tr0_norm(-2.0, 1.0, -2.0, -1.0, nh, ns / 8);
    a3 = new arc_norm(point2(-2.0, -2.0), 1.0, 90.0, 360.0, nh, ns / 8);
    t3 = new tr0_norm(-1.0, -2.0, 1.0, -2.0, nh, ns / 8);
    a4 = new arc_norm(point2(2.0, -2.0), 1.0, -180.0, 90.0, nh, ns / 8);
    t4 = new tr0_norm(2.0, -1.0, 2.0, 1.0, nh, ns / 8);
}
void coneA2n_inv::draw()
{
    a1->draw(); // Draw a cone based on arc
    t1->draw_inv(); // Draw a cone based on segment
    a2->draw(); // Draw a cone based on arc
    t2->draw_inv(); // Draw a cone based on segment
    a3->draw(); // Draw a cone based on arc
    t3->draw_inv(); // Draw a cone based on segment
    a4->draw(); // Draw a cone based on arc
    t4->draw_inv(); // Draw a cone based on segment
}
class tr0_norm_text : public tr0_norm
{
protected:
    double* t;
    int indt(int i, int j, int k)
    {
        return (i * ns + j) * 2 + k;
    }
public:
    tr0_norm_text(double, double, double, double, int, int, double, double);
    tr0_norm_text(point2&, point2&, int, int, double, double);
    void draw();
    void draw_inv();
};
tr0_norm_text::tr0_norm_text(double x1i, double y1i, double x2i, double y2i, int
    heightsegm, int sectors,
    double tmin, double tmax) :
    tr0_norm(x1i, y1i, x2i, y2i, heightsegm, sectors)
{
    int i, j;
    t = new double[nh * ns * 2];
    double hstep = 1.0 / nh;
    double vstep = (tmax - tmin) / (ns - 1);
    double hcur, vcur;
    hcur = 0.0;
    for (i = 0; i < nh; i++)
    {
        hcur += hstep;
        vcur = tmin;
        for (j = 0; j < ns; j++)
        {
            *(t + indt(i, j, 0)) = hcur;
            *(t + indt(i, j, 1)) = vcur;
            vcur += vstep;
        }
    }
}
tr0_norm_text::tr0_norm_text(point2& p1, point2& p2, int heightsegm, int sectors, double
    tmin, double tmax) :
    tr0_norm(p1, p2, heightsegm, sectors)
{
    int i, j;
    t = new double[nh * ns * 2];
    double hstep = 1.0 / nh;
    double vstep = (tmax - tmin) / (ns - 1);
    double hcur, vcur;
    hcur = 0.0;
    for (i = 0; i < nh; i++)
    {
        hcur += hstep;
        vcur = tmin;
        for (j = 0; j < ns; j++)
        {
            *(t + indt(i, j, 0)) = hcur;
            *(t + indt(i, j, 1)) = vcur;
            vcur += vstep;
        }
    }
}
void tr0_norm_text::draw()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    glNormal3dv(norm(v, c[nh - 1][0], c[nh - 1][ns - 1]));
    for (j = 0; j < ns - 1; j++)
    {
        // glNormal3dv(norm(v, c[0][j], c[0][j + 1]));
        glTexCoord2d(0., 0.5);glVertex3dv(v);
        glTexCoord2dv(t + indt(0, j, 0)); glVertex3dv(c[0][j]);
        glTexCoord2dv(t + indt(0, j + 1, 0)); glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            // glNormal3dv(norm(c[i][j], c[i + 1][j], c[i][j + 1]));
            glTexCoord2dv(t + indt(i, j, 0)); glVertex3dv(c[i][j]);
            glTexCoord2dv(t + indt(i + 1, j, 0)); glVertex3dv(c[i + 1][j]);
            glTexCoord2dv(t + indt(i, j + 1, 0)); glVertex3dv(c[i][j + 1]);
            // glNormal3dv(norm(c[i][j + 1], c[i + 1][j], c[i + 1][j + 1]));
            glTexCoord2dv(t + indt(i, j + 1, 0)); glVertex3dv(c[i][j + 1]);
            glTexCoord2dv(t + indt(i + 1, j, 0)); glVertex3dv(c[i + 1][j]);
            glTexCoord2dv(t + indt(i + 1, j + 1, 0)); glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
void tr0_norm_text::draw_inv()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    glNormal3dv(norm_inv(v, c[nh - 1][0], c[nh - 1][ns - 1]));
    for (j = 0; j < ns - 1; j++)
    {
        // glNormal3dv(norm_inv(v, c[0][j], c[0][j + 1]));
        glVertex3dv(v);
        glVertex3dv(c[0][j]);
        glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            // glNormal3dv(norm_inv(c[i][j], c[i + 1][j], c[i][j + 1]));
            glVertex3dv(c[i][j]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i][j + 1]);
            // glNormal3dv(norm_inv(c[i][j + 1], c[i + 1][j], c[i + 1][j + 1]));
            glVertex3dv(c[i][j + 1]);
            glVertex3dv(c[i + 1][j]);
            glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
//
// ======= Cone from arc with normal vectors and texture coordinates ==================
//
class arc_norm_text : public arc_norm
{
protected:
    double* t;
    int indt(int i, int j, int k)
    {
        return (i * ns + j) * 2 + k;
    }
public:
    arc_norm_text(point2, double, double, double, int, int, double, double);
    void draw();
    void draw_inv();
};
arc_norm_text::arc_norm_text(point2 init_center, double init_r, double init_start, double
    init_end, int heightsegm, int sectors,
    double tmin, double tmax) :
    arc_norm(init_center, init_r, init_start, init_end, heightsegm, sectors)
{
    int i, j;
    t = new double[nh * ns * 2];
    double hstep = 1.0 / nh;
    double vstep = (tmax - tmin) / (ns - 1);
    double hcur, vcur;
    hcur = 0.0;
    for (i = 0; i < nh; i++)
    {
        hcur += hstep;
        vcur = tmin;
        for (j = 0; j < ns; j++)
        {
            *(t + indt(i, j, 0)) = hcur;
            *(t + indt(i, j, 1)) = vcur;
            vcur += vstep;
        }
    }
}
void arc_norm_text::draw()
{
    int i, j;
    double v[3] = { 0.0, 0.0, 0.0 };
    glBegin(GL_TRIANGLES);
    for (j = 0; j < ns - 1; j++)
    {
        glNormal3d((c[0][j][0] + c[0][j + 1][0]) * 0.5,
            (c[0][j][1] + c[0][j + 1][1]) * 0.5,
            -(c[0][j][2] + c[0][j + 1][2]) * 0.5);
        glTexCoord2d(0., 0.5); glVertex3d(0.0, 0.0, 0.0);
        glNormal3d(c[0][j][0], c[0][j][1], -c[0][j][2]);
        glTexCoord2dv(t + indt(0, j, 0)); glVertex3dv(c[0][j]);
        glNormal3d(c[0][j + 1][0], c[0][j + 1][1], -c[0][j + 1][2]);
        glTexCoord2dv(t + indt(0, j + 1, 0)); glVertex3dv(c[0][j + 1]);
    }
    for (i = 0; i < nh - 1; i++)
    {
        for (j = 0; j < ns - 1; j++)
        {
            glNormal3d(c[i][j][0], c[i][j][1], -c[i][j][2]);
            glTexCoord2dv(t + indt(i, j, 0)); glVertex3dv(c[i][j]);
            glNormal3d(c[i + 1][j][0], c[i + 1][j][1], -c[i + 1][j][2]);
            glTexCoord2dv(t + indt(i + 1, j, 0)); glVertex3dv(c[i + 1][j]);
            glNormal3d(c[i][j + 1][0], c[i][j + 1][1], -c[i][j + 1][2]);
            glTexCoord2dv(t + indt(i, j + 1, 0)); glVertex3dv(c[i][j + 1]);
            glNormal3d(c[i][j + 1][0], c[i][j + 1][1], -c[i][j + 1][2]);
            glTexCoord2dv(t + indt(i, j + 1, 0)); glVertex3dv(c[i][j + 1]);
            glNormal3d(c[i + 1][j][0], c[i + 1][j][1], -c[i + 1][j][2]);
            glTexCoord2dv(t + indt(i + 1, j, 0)); glVertex3dv(c[i + 1][j]);
            glNormal3d(c[i + 1][j + 1][0], c[i + 1][j + 1][1], -c[i + 1][j +
                1][2]);
            glTexCoord2dv(t + indt(i + 1, j + 1, 0)); glVertex3dv(c[i + 1][j + 1]);
        }
    }
    glEnd();
}
//
// ===== Cone A2nt with normal vectors and texture coordinates =======================
// (Own surface in the base on arcs and segments with normal vectors and texture 
// coordinates)
//
class coneA2nt
{
protected:
    int nh;
    int ns;
    arc_norm_text* a1, * a2, * a3, * a4;
    tr0_norm_text* t1, * t2, * t3, * t4;
public:
    coneA2nt(int, int);
    void draw();
};
coneA2nt::coneA2nt(int heightsegm, int sectors)
{
    nh = heightsegm;
    ns = sectors;
    if (nh > MAXH)
        nh = MAXH;
    else
        if (nh < 1)
            nh = 10;
    if (ns > MAXS)
        ns = MAXS;
    else
        if (ns < 3)
            ns = 10;
    t1 = new tr0_norm_text(2.0, 1.0, 2.0, -1.0, nh, ns / 8, 0.125, 0.250);
    t2 = new tr0_norm_text(1.0, -2.0, -1.0, -2.0, nh, ns / 8, 0.375, 0.5);
    t3 = new tr0_norm_text(-2.0, -1.0, -2.0, 1.0, nh, ns / 8, 0.625, 0.750);
    t4 = new tr0_norm_text(-1.0, 2.0, 1.0, 2.0, nh, ns / 8, 0.875, 1.0);

    a1 = new arc_norm_text(point2(2.0, 2.0), 1.0, 0.0, 360.0, nh, ns / 8, 0.0, 0.125);
    a2 = new arc_norm_text(point2(2.0, -2.0), 1.0, 0.0, 360.0, nh, ns / 8, 0.250, 0.375);
    a3 = new arc_norm_text(point2(-2.0, -2.0), 1.0, 0.0, 360.0, nh, ns / 8, 0.5, 0.625);
    a4 = new arc_norm_text(point2(-2.0, 2.0), 1.0, 0.0, 360.0, nh, ns / 8, 0.750, 0.875);

}
void coneA2nt::draw()
{
    a1->draw(); // Draw a cone based on arc
    t1->draw(); // Draw a cone based on segment
    a2->draw(); // Draw a cone based on arc
    t2->draw(); // Draw a cone based on segment
    a3->draw(); // Draw a cone based on arc
    t3->draw(); // Draw a cone based on segment
    a4->draw(); // Draw a cone based on arc
    t4->draw(); // Draw a cone based on segment
}


static coneA2 caxis(6, 12);
static coneA2 c1(10, 30);

void drawaxis()
{
    glPointSize(10.0f);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glColor3d(0.0, 0.0, 0.0);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
    glLineWidth(1.5f);
    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINES);
    // Axis X
    glColor3d(0.0, 0.0, 0.0);
    glVertex3d(-5.5, 0.0, 0.0);
    glColor3d(1.0, 0.0, 0.0);
    glVertex3d(5.5, 0.0, 0.0);
    // Axis Y
    glColor3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, -5.5, 0.0);
    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(0.0, 5.5, 0.0);
    // Axis Z
    glColor3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, -5.5);
    glColor3d(0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 5.5);
    glEnd();
    // Cone X
    glColor3d(1.0, 0.0, 0.0);
    glPushMatrix();
    glRotated(180.0, 0.0f, 0.0f, 1.0f);
    glTranslated(-5.6f, 0.0f, 0.0f);
    glRotated(90.0, 0.0f, 1.0f, 0.0f);
    glScaled(0.08, 0.08, 0.2);
    caxis.draw();
    glPopMatrix();
    // Cone Y
    glColor3d(0.0, 1.0, 0.0);
    glPushMatrix();
    glRotated(180.0, 0.0f, 0.0f, 1.0f);
    glTranslated(0.0f, -5.6f, 0.0f);
    glRotated(-90.0, 1.0f, 0.0f, 0.0f);
    glScaled(0.08, 0.08, 0.2);
    caxis.draw();
    glPopMatrix();
    // Cone Z
    glColor3d(0.0, 0.0, 1.0);
    glPushMatrix();
    glTranslated(0.0f, 0.0f, 5.6f);
    glRotated(180.0, 1.0f, 0.0f, 0.0f);
    glScaled(0.08, 0.08, 0.2);
    caxis.draw();
    glPopMatrix();
}


AUX_RGBImageRec* GenChecker(int nu, int nv, int ku = 8, int kv = 8)
{
    AUX_RGBImageRec* pimage = new AUX_RGBImageRec;
    Color* pcolor = new Color[nu * nv];
    int nl = nv / ku;
    int nc = nu / kv;
    bool fl = true;
    bool fc;
    pimage->sizeX = nu;
    pimage->sizeY = nv;
    pimage->data = (unsigned char*)pcolor;
    for (int i = 0; i < nu; i++)
    {
        if (i % nl == 0)
            fl = !fl;
        fc = fl;
        for (int j = 0; j < nv; j++)
        {
            if (j % nc == 0)
                fc = !fc;
            if (fc)
            {
                pcolor[i * nu + j].r = 0;
                pcolor[i * nu + j].g = 255;
                pcolor[i * nu + j].b = 255;
            }
            else
            {
                pcolor[i * nu + j].r = 255;
                pcolor[i * nu + j].g = 0;
                pcolor[i * nu + j].b = 255;
            }
        }
    }
    return pimage;
}









tr0* cone;

//
// ============== resize ============================
//
void CALLBACK resize(int width, int height)
{
    GLuint vp = width < height ? width - 10 : height - 10;
    glViewport(5, 5, vp, vp);
    // glViewport(0, 0, width, height);
    glGetIntegerv(GL_VIEWPORT, CurrentVp);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ///*
    //
    // Orthogonal projection
    //
    MinX = -6.2, MaxX = 6.2;
    MinY = -6.2, MaxY = 6.2;
    NearZ = -1.0, FarZ = -13.0;
    glOrtho(MinX, MaxX, MinY, MaxY, -NearZ, -FarZ);
    //*/
    /*
//
// Perspective projection
//
MinX = -5.2, MaxX = 5.2;
MinY = -5.2, MaxY = 5.2;
NearZ = -4.0, FarZ = -20.0;
glFrustum(MinX, MaxX, MinY, MaxY, -NearZ, -FarZ);
MinX *= FarZ/NearZ, MaxX *= FarZ/NearZ;
MinY *= FarZ/NearZ, MaxY *= FarZ/NearZ;
*/
    glMatrixMode(GL_MODELVIEW);
}
//
// ============== Drawing axes ============================
//
//
// ============= display(void) =======================
//
static GLdouble MSym[16] =
{
-1.0, 0.0, 0.0, 0.0,
0.0, -1.0, 0.0, 0.0,
0.0, 0.0, -1.0, 0.0,
0.0, 0.0, 0.0, 1.0
};
double fi = 35.0;
static coneA2 cA2(6, 96); // Half own surface arc method
static coneA2_inv cA2i(6, 96); // Half own surface inverted arc method
static coneA2n cA2n(6, 96); // Half own surface with normal vectors
 // arc method
static coneA2n_inv cA2ni(6, 96); // Half own surface inverted with normal vectors
 // arc method
static coneA2nt cA2nt(6, 96); // Half own surface with normal vectors and 
 // texture arc method
void CALLBACK display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRasterPos3d(MinX + 0.01, MinY + 0.01, FarZ + 0.001);
    glPixelZoom((float)CurrentVp[2] / pimage->sizeX,
        (float)CurrentVp[3] / pimage->sizeY);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDrawPixels(pimage->sizeX, pimage->sizeY,
        GL_RGB, GL_UNSIGNED_BYTE, pimage->data);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_COLOR_MATERIAL);
    glPushMatrix();

    fi += 0.05;
    glTranslated(0.0, 0.0, (NearZ + FarZ) * 0.5);
    glRotated(0.0 + fi, 1.0, 0.0, 0.0);
    glRotated(0.0 - 2.0 * fi, 0.0, 1.0, 0.0);

    drawaxis();

    glDisable(GL_COLOR_MATERIAL);
    GLfloat fd[4] = { 0.8f, 0.8f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, fd);
    GLfloat fa[4] = { 0.3f, 0.3f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, fa);
    GLfloat fs[4] = { 0.1f, 0.1f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, fs);
    GLfloat bd[4] = { 0.7f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_BACK, GL_DIFFUSE, bd);
    GLfloat ba[4] = { 0.2f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_BACK, GL_AMBIENT, ba);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glPushMatrix();

    glScaled(1.0, 1.0, 3.0);
    glColor3d(1.0, 0.0, 1.0);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    cA2nt.draw(); 
    glDisable(GL_TEXTURE_2D);
    glCullFace(GL_FRONT);

    cA2n.draw(); 

    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glPushMatrix();
    glMultMatrixd(MSym);
    cA2ni.draw(); 

    glPopMatrix();
    glPopMatrix();
    glEnable(GL_COLOR_MATERIAL);

    glPopMatrix();
    auxSwapBuffers();

}

void main()
{
    auxInitPosition(0, 0, 500, 500);
    auxInitDisplayMode(AUX_RGB | AUX_DEPTH | AUX_DOUBLE);
    auxInitWindow((LPCWSTR)("OpenGL N 43"));
    auxIdleFunc(display);
    auxReshapeFunc(resize);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat pos[4] = { 0.0f, 2.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    GLfloat dir[3] = { 0.0f, 0.0f, -5.0f };
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
    GLfloat col[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
    pimage = GenChecker(128, 128, 8, 8);
    //pimage = auxDIBImageLoadA("Mybitmap.bmp");
    // TextureImage = auxDIBImageLoadA("Texture_512_256.bmp");
     TextureImage = GenChecker(128, 128, 8, 8);
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX,
        TextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
        TextureImage->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    auxMainLoop(display);
}
