#include "view/GLCanvas.h"
#include <QFontMetrics>
#include <QString>
#include <QBitmap>
#include <QPixmap>
#include <QPainter>
#include <QImage>
#include "datatype/RGBt.h"
#include "color/ColorManager.h"

#include <QBitmap>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QGLWidget>
#include <assert.h>

#include "color/ColorManager.h"

#define SLICES 2
#define SLICES2 1
GLCanvas::GLCanvas(QGLWidget *glWidget = 0) : zoom(1.0,1.0)
{
	if (glWidget) setGLWidget(glWidget);
	this->byps = true;
    xxor = false;
    translucent = false;
    flatcolor = false;
	stencilmode = false;
    alpha = 1.0;
}

GLCanvas::~GLCanvas()
{
}

void GLCanvas::setGLWidget(QGLWidget *glWidget) {
	this->glWidget = glWidget;
}

QGLWidget *GLCanvas::getGLWidget() {
	return this->glWidget;
}

void GLCanvas::flush()
{
	glFlush();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GLCanvas::makeCurrent()
{
	this->glWidget->makeCurrent();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GLCanvas::setForeground( const RGBt &color, double a )
{
	// Set normal foreground
	fg_color = color;
	alpha = a;
	glColor4d(color.R, color.G, color.B, alpha );
	if (a!=1.0)
		translucent = true;
	else
		translucent = false;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GLCanvas::setForeground( unsigned long color, double a )
{
	setForeground(RGBt(color),a);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GLCanvas::setBackground( unsigned long color )
{
	// Set normal background
	bg_color = RGBt(color);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void GLCanvas::beginDraw()
{
	// PAUL: draw_org
	// Huh?! This called in grDisplayEvent
	//MakeCurrent();

	if (xxor == true)
	{
		// Setup XOR drawing mode
		glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_INVERT);
		glDrawBuffer(GL_FRONT);
	}
	else
	{
		glColor4d(fg_color.R, fg_color.G, fg_color.B, alpha );
	}

	if (translucent == true) // && asize != 0)
	{
		// Turn on Translucency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	if (stencilmode == true)
	{
		glStencilFunc(GL_ALWAYS, 1, 1);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GLCanvas::endDraw() {
	makeCurrent();

	if (xxor == true) {
		glDisable(GL_COLOR_LOGIC_OP);
		glDrawBuffer(GL_BACK);
	}
	if (translucent == true) { // && asize != 0)
		glDisable(GL_BLEND);
	}
	if (stencilmode == true) {
		glStencilFunc(GL_NOTEQUAL, 1, 1);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void GLCanvas::drawLine( double x1, double y1, double x2, double y2)
{

	this->beginDraw();
	glBegin(GL_LINES);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glEnd();
	this->endDraw();
}

//-----------------------------------------------------------------------------
void GLCanvas::drawLine( std::vector<Vec2> buffer, int pos)
{
	beginDraw();
	glBegin(GL_LINES);
	for(int i = 0; i < pos; i++)
		glVertex2d(buffer[i].X, buffer[i].Y);
	glEnd();
	endDraw();
}

//the circle is acturally a disk, inner radius: radius-offset, out radius: radius+offset
void GLCanvas::drawCircle( double centerx, double centery, double radius, double offset)
{
	glPushMatrix();

	glTranslated(centerx, centery, 0.0f);
	GLUquadricObj* quadObj;
	quadObj=gluNewQuadric();
	gluQuadricDrawStyle(quadObj,GLU_FILL);
	gluQuadricNormals(quadObj,GLU_FLAT);
	int slice = (int) (SLICES * radius * 360 * MAX(zoom.X, zoom.Y));
	if ( slice < 6 )
		slice = 6;
	gluDisk(quadObj,radius-offset, radius+offset, slice, 1);

	glPopMatrix();
}


void GLCanvas::drawArc( double centerx, double centery, double radius, double start_angle, double sweep_angle)
{
	glPushMatrix();

	glTranslated(centerx, centery, 0.0);
	GLUquadricObj* quadObj;
	quadObj=gluNewQuadric();
	gluQuadricDrawStyle(quadObj,GLU_LINE);
	gluQuadricNormals(quadObj,GLU_FLAT);
	int slice = (int) (SLICES * sweep_angle * radius * MAX(zoom.X, zoom.Y));
	if ( slice < 2 )
		slice = 2;
	gluPartialDisk(quadObj,radius, radius, slice, 1, start_angle, sweep_angle);

	glPopMatrix();
}

void GLCanvas::drawPartialDisk( double centerx, double centery, double inner_radius,
		double outer_radius, double start_angle, double sweep_angle)
{
	glPushMatrix();

	glTranslated(centerx, centery, 0.0);
	GLUquadricObj* quadObj;
	quadObj=gluNewQuadric();
	gluQuadricDrawStyle(quadObj,GLU_FILL);
	gluQuadricNormals(quadObj,GLU_SMOOTH);
	int slice = (int)(sweep_angle * SLICES2 * outer_radius * MAX(zoom.X, zoom.Y));
	if ( slice < 2 )
		slice = 2;
	gluPartialDisk(quadObj,inner_radius, outer_radius, slice, 1, start_angle, sweep_angle);

	glPopMatrix();
}


void GLCanvas::drawLines( const std::vector<Vec2> &pts )
{
	glBegin(GL_LINE_STRIP);
	glVertex2d(pts[0].X, pts[0].Y);
	for (int i=1; i<(int)pts.size(); i++) {
		glVertex2d(pts[i].X, pts[i].Y);
	}
	glEnd();
}

void GLCanvas::drawRectangle(double x, double y, double width, double ht )
{
	glBegin(GL_LINE_LOOP);
	glVertex2d( x, y );
	glVertex2d( (x+width), y);
	glVertex2d( (x+width), (y+ht));
	glVertex2d( x, (y+ht) );
	glEnd();
}


void GLCanvas::drawRectangle2(double x1, double x2, double y1, double y2 )
{
	glBegin(GL_LINE_LOOP);
	glVertex2d( x1, y1);
	glVertex2d( x2, y1);
	glVertex2d( x2, y2);
	glVertex2d( x1, y2);
	glEnd();
}


void GLCanvas::fillRectangle( double x, double y, double width, double ht)
{
	// PAUL: draw_org
	glBegin(GL_QUADS);
	glVertex2d(x, y);
	glVertex2d((x+width), y);
	glVertex2d((x+width), (y+ht));
	glVertex2d(x, (y+ht));
	glEnd();
}

void GLCanvas::drawPoint( double x, double y)
{
	glBegin(GL_POINTS);
	glVertex2d(x, y);
	glEnd();
}

void GLCanvas::drawPoints( const std::vector<Vec2> &pts, double size)
{
	glPointSize((float) size);
	glBegin(GL_POINTS);
	for (int i=0;i<(int)pts.size();i++) {
		glVertex2d(pts[i].X, pts[i].Y);
	}
	glEnd();
}

void GLCanvas::drawPolygon( const std::vector<Vec2> &pts)
{
	glBegin(GL_LINE_LOOP);
	for (int i=0;i<(int)pts.size();i++) {
		glVertex2d(pts[i].X, pts[i].Y);
	}
	glEnd();
}

void GLCanvas::fillPolygon( const std::vector<Vec2> &pts)
{
	glBegin(GL_POLYGON);
	for (int i=0;i<(int)pts.size();i++) {
		glVertex2d( pts[i].X,  pts[i].Y);
	}
	glEnd();
}

void GLCanvas::fillBand( const std::vector<Vec2> &pts, double ratio)
{
	assert(pts.size()==4);

	Vec2 mid1, mid2;
	mid1 = (1-ratio)*pts[0] + (ratio)*pts[3];
	mid2 = (1-ratio)*pts[1] + (ratio)*pts[2];

	glBegin(GL_POLYGON);
	glColor3d( bg_color.R,  bg_color.G,  bg_color.B);
	glVertex2d( pts[0].X,  pts[0].Y);
	glVertex2d( pts[1].X,  pts[1].Y);
	glColor3d( fg_color.R,  fg_color.G,  fg_color.B);
	glVertex2d( mid2.X,  mid2.Y);
	glVertex2d( mid1.X,  mid1.Y);
	glEnd();
	glBegin(GL_POLYGON);
	glColor3d( fg_color.R,  fg_color.G,  fg_color.B);
	glVertex2d( mid1.X,  mid1.Y);
	glVertex2d( mid2.X,  mid2.Y);
	glColor3d( bg_color.R,  bg_color.G,  bg_color.B);
	glVertex2d( pts[2].X,  pts[2].Y);
	glVertex2d( pts[3].X,  pts[3].Y);
	glEnd();
}


void GLCanvas::drawTransparentBand2( const std::vector<Vec2> &pts, double opacity_bottom,
		double opacity_top, double frac)
{
	Vec2 mid1 = (1-frac)*pts[0] + (frac)*pts[3];
	Vec2 mid2 = (1-frac)*pts[1] + (frac)*pts[2];

	std::vector<Vec2> lo(4), hi(4);
	lo[0] = pts[0];
	lo[1] = pts[1];
	lo[2] = mid2;
	lo[3] = mid1;

	hi[0] = mid1;
	hi[1] = mid2;
	hi[2] = pts[2];
	hi[3] = pts[3];

	RGBt highlight = RGBt(0x009900);

	if (frac < 0.5) {
		drawTransparentBand( hi, opacity_bottom, opacity_top, fg_color, fg_color);
		drawTransparentBand( lo, opacity_bottom, opacity_bottom, highlight, fg_color);
	} else {
		drawTransparentBand( lo, opacity_bottom, opacity_top, fg_color, fg_color);
		drawTransparentBand( hi, opacity_top, opacity_top, fg_color, highlight);
	}
}


void GLCanvas::drawTransparentBand( const std::vector<Vec2> &pts,
		double opacity_bottom, double opacity_top,
		RGBt color_bottom, RGBt color_top)
{
	assert(pts.size()==4);
	glBegin(GL_QUAD_STRIP);
	glColor4d( color_bottom.R,  color_bottom.G,  color_bottom.B,  (alpha*opacity_bottom) );
	glVertex2d( pts[1].X,  pts[1].Y);
	glVertex2d( pts[0].X,  pts[0].Y);

	glColor4d( color_top.R,  color_top.G,  color_top.B,  (alpha*opacity_top) );
	glVertex2d( pts[2].X,  pts[2].Y);
	glVertex2d( pts[3].X,  pts[3].Y);
	glEnd();

}

//jing
void GLCanvas::drawTransparentSquare2( const std::vector<Vec2> &pts, double opacity_bottom,
		double opacity_top, double)
{
	drawTransparentSquare( pts, opacity_top, opacity_bottom, fg_color, fg_color);
}

void GLCanvas::drawTransparentSquare( const std::vector<Vec2> &pts,
		double opacity_bottom, double opacity_top,
		RGBt color_bottom, RGBt color_top)
{
	assert(pts.size()==5);
	assert(xxor == false);

	flatcolor = false;
	translucent = true;

	beginDraw();
	glBegin(GL_TRIANGLES);

	glColor4d( color_bottom.R,  color_bottom.G,  color_bottom.B,  (alpha*opacity_bottom) );
	glVertex2d( pts[0].X,  pts[0].Y);
	glColor4d( color_top.R,  color_top.G,  color_top.B,  (alpha*opacity_top) );
	glVertex2d( pts[1].X,  pts[1].Y);
	glVertex2d( pts[2].X,  pts[2].Y);

	glVertex2d( pts[2].X,  pts[2].Y);
	glVertex2d( pts[3].X,  pts[3].Y);
	glColor4d( color_bottom.R,  color_bottom.G,  color_bottom.B,  (alpha*opacity_bottom) );
	glVertex2d( pts[0].X,  pts[0].Y);


	glVertex2d( pts[0].X,  pts[0].Y);
	glColor4d( color_top.R,  color_top.G,  color_top.B,  (alpha*opacity_top) );
	glVertex2d( pts[3].X,  pts[3].Y);
	glVertex2d( pts[4].X,  pts[4].Y);


	glVertex2d( pts[4].X,  pts[4].Y);
	glVertex2d( pts[1].X,  pts[1].Y);
	glColor4d( color_bottom.R,  color_bottom.G,  color_bottom.B,  (alpha*opacity_bottom) );
	glVertex2d( pts[0].X,  pts[0].Y);
	glEnd();

	endDraw();

	flatcolor = true;
	translucent = false;
}

void GLCanvas::drawString( int x, int y, char* text, QFont *font)
{
	makeCurrent();
	getGLWidget()->renderText(x, y, QString(text), *font);
}

void GLCanvas::drawConfinedString(double x, double y, char *text, double width, QFont *font)
{
	QFontMetrics metrics(*font);
	int N = strlen(text);
	for (;N>0;N--) {
		int font_size_x	= metrics.width(text, N);
		if (font_size_x<width) {
			char newString[N+1];
			// create new substring
			for(int i=0; i <= N; i++) {
				if(i == N) newString[i] = '\0';
				else newString[i] = text[i];
			}
			drawString((int)x, (int)y, newString, font);
			return;
		}
	}
}

void GLCanvas::drawChar( int x, int y, char ch)
{
	char temp[2];
	temp[0] = ch;
	temp[1] = '\0';
	getGLWidget()->renderText(x, y, temp, g_globals.textFont);
}

//-----------------------------------------------------------------------------
Vec2 GLCanvas::mapScreenToData( const Vec2 &sc )
{
	// The call to makeCurrent() can make sure
	// that we are working on the current opengl rendering area
	makeCurrent();
	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);

	double wc[3];
	int ret = gluUnProject(sc.X,sc.Y,0,modelMatrix,projMatrix,viewport,wc,wc+1,wc+2);
	if (ret==GL_FALSE) {
		// some serious errors happened.
		// assert(0);
		printf ("Failed to mapScreenToData()\n");
		return Vec2(0.0, 0.0);
	}
	double v1 = wc[0];
	double v2 = wc[1];

	return Vec2(v1,v2);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
Vec2 GLCanvas::mapDataToScreen( const Vec2 &p )
{
	// The call to makeCurrent() can make sure
	// that we are working on the current opengl rendering area
	makeCurrent();
	GLdouble modelMatrix[16], projMatrix[16];
	GLint viewport[4];
	glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);

	double sc[3];
	int ret = gluProject(p.X,p.Y,0,modelMatrix,projMatrix,viewport,sc,sc+1,sc+2);
	if (ret==GL_FALSE) {
		// some serious errors happened.
		// assert(0);
		printf ("Failed to mapDataToScreen()\n");
		return Vec2(0.0, 0.0);
	}

	return Vec2(sc[0],viewport[3]-sc[1]);
}
//-----------------------------------------------------------------------------


