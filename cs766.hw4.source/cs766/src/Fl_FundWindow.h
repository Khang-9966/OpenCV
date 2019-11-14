#ifndef FL_FUNDWINDOW_H
#define FL_FUNDWINDOW_H

#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#include <Fl/Fl_Double_Window.h>
#pragma warning(pop)

#include "VBPtr.h"
#include "EpiGeom.h"
#include "KltWrap.h"

class Fl_FundWindow : public Fl_Double_Window {
private:
    VBPtr m_img1, m_img2, m_disp;
    EpiGeom m_geom;
    int m_x, m_y;
public:
    std::pair<std::vector<Matrix>, std::vector<Matrix> > m_pts;

public:
    Fl_FundWindow(VBPtr img1, VBPtr img2, VBPtr disp, EpiGeom geom);

protected:
    virtual void draw();

    virtual int handle(int e);
};

#endif
