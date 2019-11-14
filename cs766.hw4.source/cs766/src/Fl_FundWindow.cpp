#include "Fl_FundWindow.h"
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#include <Fl/Fl_Draw.h>
#pragma warning(pop)
#include "globals.h"
#include <iostream>
using namespace std;

#define max(a, b) (a)>(b)?(a):(b)
Fl_FundWindow::Fl_FundWindow(VBPtr img1, VBPtr img2, VBPtr disp, EpiGeom geom) :
    Fl_Double_Window(img1->getWidth()+img2->getWidth(), max(img1->getHeight()+disp->getHeight(),img2->getHeight()+disp->getHeight())),
    m_x(-1), m_y(-1), m_img1(img1), m_img2(img2), m_geom(geom), m_disp(disp)
{
    Fl::visual(FL_RGB);
}
#undef max

void Fl_FundWindow::draw() {
    fl_draw_image((unsigned char*)m_img1->getImageData(),
        0, 0, m_img1->getWidth(), m_img1->getHeight(), 
        m_img1->getNumChannels(),
        m_img1->getSpan()-m_img1->getWidth()*m_img1->getNumChannels());
    fl_draw_image((unsigned char*)m_img2->getImageData(),
        m_img1->getWidth(), 0, m_img2->getWidth(), m_img2->getHeight(), 
        m_img2->getNumChannels(),
        m_img2->getSpan()-m_img2->getWidth()*m_img2->getNumChannels());	
    fl_color(FL_RED);
    for (int i=0; i<(int)m_pts.first.size(); ++i) {
        double x1 = m_pts.first[i].get(0,0);
        double y1 = m_pts.first[i].get(1,0);
        double x2 = m_pts.second[i].get(0,0);
        double y2 = m_pts.second[i].get(1,0);
        if (m_x != -1 && m_y != -1 &&
            (abs(x1-m_x) < 5 && abs(y1-m_y) < 5 ||
            abs(x2+m_img1->getWidth()-m_x)< 5 && abs(y2-m_y)<5)) {
                fl_color(FL_BLUE);
            }
            
        fl_rectf((int)(x1)-1, (int)(y1)-1, 3, 3);
        fl_rectf((int)(x2+m_img1->getWidth())-1, (int)(y2)-1, 3, 3);
        fl_color(FL_RED);
    }
    if (m_x != -1 && m_y != -1) {
        fl_color(FL_WHITE);
        fl_rectf((int)(m_x)-1, (int)(m_y)-1, 5, 5);
        fl_color(FL_RED);
        Matrix thisPt(3,1);
        Matrix line(3,1);
        thisPt.set3x1(m_x, m_y, 1);
        int adjust = 0;
        int width;
        if (m_x < m_img1->getWidth()) {
            line = m_geom.getLineParamsR(thisPt);
            adjust = m_img1->getWidth();
            width = m_img2->getWidth();
        } else {
            thisPt.set(0,0,m_x-m_img1->getWidth());
            line = m_geom.getLineParamsL(thisPt);
            width = m_img1->getWidth();
        }
        fl_push_clip(adjust, 0, width, h());
        fl_clip(adjust, 0, width, h());
        if (line.get(1,0) == 0) {
            int x = (int)(-line.get(2,0)/line.get(0,0));
            fl_line(x+adjust, 0, x+adjust, h());
        } else if (line.get(0,0) == 0) {
            int y = (int)(-line.get(2,0)/line.get(1,0));
            fl_line(adjust, y, adjust+width, y);
        } else if (line.get(0,0) < line.get(1,0)) {
            // y = mx+b
            double m = -line.get(0,0)/line.get(1,0);
            double b = -line.get(2,0)/line.get(1,0);
            fl_line(adjust, (int)b, (int)(adjust+width), (int)(width*m+b));
        } else {
            // x = my+b
            double m = -line.get(1,0)/line.get(0,0);
            double b = -line.get(2,0)/line.get(0,0);
            fl_line((int)(adjust+b), 0, (int)(adjust+(m*h()+b)), h());
        }
	fl_draw_image((unsigned char*)m_disp->getImageData(), 
		0, m_img1->getHeight(), m_disp->getWidth(), m_disp->getHeight(), m_disp->getNumChannels(), 
		m_disp->getSpan()-m_disp->getWidth()*m_disp->getNumChannels());
	fl_color(FL_BLACK);
	fl_rectf(m_disp->getWidth(), m_img2->getHeight(), m_img2->getWidth()+m_img1->getWidth()-m_disp->getWidth(),
		max(m_img1->getHeight()+m_disp->getHeight(), m_img2->getHeight()+m_disp->getHeight()) - m_img2->getHeight());
//        fl_pop_clip();

    }
}

int Fl_FundWindow::handle(int e) {
    int retVal = Fl_Double_Window::handle(e);
    if (e == FL_MOVE) {
        m_x = Fl::event_x();
        m_y = Fl::event_y();
        damage(1);
    }
    if (e == FL_LEAVE) {
        if (m_x != -1 && m_y != -1) {
            m_x = m_y = -1;
            damage(1);
        }
    }
    return retVal;
}
