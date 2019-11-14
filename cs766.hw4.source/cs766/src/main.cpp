#include <ITImage.h>
#include <ITImageIO.hpp>

#include "VBPtr.h"
#include <Matrix.h>

#include "LargeInt.h"
#include <debug.h>
#include "globals.h"
#include "Align.h"
#include "Util.h"
#include "Fl_FundWindow.h"
#include <Fl/Fl.h>
#include <iostream>
#include <cvaux.h>
using namespace std;
using namespace ImageMan;

LargeInt sumSqDif(VBPtr one, VBPtr two) {
    if (one->getNumChannels() != two->getNumChannels())
        IMERR_THROW("Channels don't match");
    int channels = one->getNumChannels();

    LargeInt retVal;
    Region iterRegion = one->getRegion().unionWith(two->getRegion());
    ValueBlockIterator iOne = one->getIterator(iterRegion);
    ValueBlockIterator iTwo = two->getIterator(iterRegion);
    for (; !iOne.passedRegionEnd(); iOne.inc(), iTwo.inc()) {
        for (int i=0; i<channels; ++i) {
            int dif = iOne.getUChar(i)-iTwo.getUChar(i);
            retVal += dif*dif;
        }
    }
    return retVal;
}

int main(int argc, char** argv) {
    try {
        ArgManager am(argc, argv);
        Globals::gArgManager = &am;

/*        VBPtr srcImg = VBPtr(new ValueBlock(imageio::read("../lab13.png")));
       
        Region reg1 = srcImg->getRegion();
        reg1.adjustBy((int)(1/100.0*reg1.width()), 0, 0, 0);
        VBPtr img1 = VBPtr(new ValueBlock(srcImg->getNumChannels(), reg1, srcImg->getDepth()));
        Util::copy(img1, srcImg);
        imageio::write(img1.get(), "../lab13-1.png");

        Region reg2 = srcImg->getRegion();
        reg2.adjustBy(0, -(int)(1/100.0*reg2.width()), 0, 0);
        VBPtr img2 = VBPtr(new ValueBlock(srcImg->getNumChannels(), reg2, srcImg->getDepth()));
        Util::copy(img2, srcImg);
        imageio::write(img2.get(), "../lab13-2.png");*/

//        VBPtr img1 = VBPtr(new ValueBlock(imageio::read("../mars1-l.jpg")));
//        VBPtr img2 = VBPtr(new ValueBlock(imageio::read("../mars1-r.jpg")));
        
		VBPtr img1 = VBPtr(new ValueBlock(imageio::read("scene1.row3.col1.png")));
        VBPtr img2 = VBPtr(new ValueBlock(imageio::read("scene1.row3.col5.png")));

        Region reg1 = img1->getRegion();
        Region reg2 = img2->getRegion();

        VBPtr img3 = VBPtr(new ValueBlock(img1->getNumChannels(),
                                            reg1,
                                            IT_UNSIGNED_CHAR));
        VBPtr img4 = VBPtr(new ValueBlock(img2->getNumChannels(), 
                                            reg2,
                                            IT_UNSIGNED_CHAR));
        cvSmooth(img1.get(), img3.get());
        cvSmooth(img2.get(), img4.get());		
        img3->convert(1, IT_UNSIGNED_CHAR);
        img4->convert(1, IT_UNSIGNED_CHAR);
		VBPtr disparity = VBPtr(new ValueBlock(1, reg1, IT_UNSIGNED_CHAR));
        // Now img 3&4 are the smoothed, grayscale images I want to work with
        // Chop them into 4ths;
        Region regs1[4] = { reg1, reg1, reg1, reg1 };
        Region regs2[4] = { reg2, reg2, reg2, reg2 };
        VBPtr imgs1[4], imgs2[4];
        regs1[0].adjustBy(0, -reg1.width()/2, 0, -reg1.height()/2);
        regs1[1].adjustBy(reg1.width()/2, 0, 0, -reg1.height()/2);
        regs1[2].adjustBy(0, -reg1.width()/2, reg1.height()/2, 0);
        regs1[3].adjustBy(reg1.width()/2, 0, reg1.height()/2, 0);
        regs2[0].adjustBy(0, -reg2.width()/2, 0, -reg2.height()/2);
        regs2[1].adjustBy(reg2.width()/2, 0, 0, -reg2.height()/2);
        regs2[2].adjustBy(0, -reg2.width()/2, reg2.height()/2, 0);
        regs2[3].adjustBy(reg2.width()/2, 0, reg2.height()/2, 0);
        for (int i=0; i<4; ++i) {
            imgs1[i] = VBPtr(new ValueBlock(1, regs1[i], IT_UNSIGNED_CHAR));
            Util::copy(imgs1[i], img3);
            imgs2[i] = VBPtr(new ValueBlock(1, regs2[i], IT_UNSIGNED_CHAR));
            Util::copy(imgs2[i], img4);
        }
        std::pair<std::vector<Matrix>, std::vector<Matrix> > pts;
        for (int i=0; i<4; ++i) {
            std::pair<std::vector<Matrix>, std::vector<Matrix> > tpts =
                Util::pilu(imgs1[i], imgs2[i], Globals::NUM_PTS);
            for (int j=0; j<Globals::NUM_PTS; ++j) {
                tpts.first[j].set(0, 0, tpts.first[j].get(0, 0)+regs1[i].left());
                tpts.first[j].set(1, 0, tpts.first[j].get(1, 0)+regs1[i].top());
                tpts.second[j].set(0, 0, tpts.second[j].get(0, 0)+regs2[i].left());
                tpts.second[j].set(1, 0, tpts.second[j].get(1, 0)+regs2[i].top());
                pts.first.push_back(tpts.first[j]);
                pts.second.push_back(tpts.second[j]);
            }
        }
            

//        VBPtr img1 = VBPtr(new ValueBlock(imageio::read("../mars3-l.jpg")));
//        VBPtr img2 = VBPtr(new ValueBlock(imageio::read("../mars3-r.jpg")));
//        VBPtr img1 = VBPtr(new ValueBlock(imageio::read("../mars2-l.jpg")));
//        VBPtr img2 = VBPtr(new ValueBlock(imageio::read("../mars2-r.jpg")));
/*        VBPtr depth = VBPtr(new ValueBlock(1, img2->getRegion(), IT_UNSIGNED_CHAR));
        img1->convert(1, IT_UNSIGNED_CHAR);
        img2->convert(1, IT_UNSIGNED_CHAR);
        cvFindStereoCorrespondence(img1.get(), img2.get(), 
            CV_DISPARITY_BIRCHFIELD, depth.get(), 50, 15, 3, 6, 8, 15 );
        imageio::write(*depth, "../depth.png");

        double pts1[] = {
            124, 149,
            159, 221,
            47, 144,
            29, 219,
            164, 73,
            53, 103,
            215, 188,
            79, 241
        };
        double pts2[] = {
            132, 149,
            178, 221,
            56, 144,
            46, 219,
            164, 73,
            55, 103,
            229, 188,
            98, 241
        };

        std::vector<Matrix> pass1, pass2;
        Matrix temp(3,1);
        for (int i=0; i<8; ++i) {
            temp.set3x1(pts1[2*i], pts1[2*i+1], 1);
            pass1.push_back(temp);
            temp.set3x1(pts2[2*i], pts2[2*i+1], 1);
            pass2.push_back(temp);
        }*/
//        EpiGeom geom(pass1, pass2);
//        EpiGeom geom = getFundamentalMatrix4(img1, img2);
/*        std::pair<std::vector<Matrix>, std::vector<Matrix> > pts =
            Util::pilu(img3, img4, Globals::NUM_PTS);
        for (int i=0; i<Globals::NUM_PTS; ++i) {
            cout << pts.first[i].get(0,0) << ", " << pts.first[i].get(1,0) << " : ";
            cout << pts.second[i].get(0, 0) << ", " << pts.second[i].get(1,0) << endl;
        }*/
        EpiGeom geom(pts.first, pts.second);
        cout << "Fund: " << endl << geom.getFund();
//        int transX = reg2.left()-reg1.left();
//        cout << "Translation should be: " << transX << endl;
/*        cout << "Fund matrix is: " << endl;
        cout << geom.getFund();
*/
/*        std::vector<Matrix> testPts1;
        std::vector<Matrix> testPts2;
        for (int j=0; j<10; ++j) { 
            for (int i=0; i<10; ++i) {
                testPts1.push_back(Matrix(3,1));
                testPts1.back().set(0,0, i*10);
                testPts1.back().set(1,0, i*10);
                testPts1.back().set(2,0, 1);
            }
        }
        for (int i=0; i<(int)testPts1.size(); ++i) {
            testPts2.push_back(testPts1[i]);
            testPts2.back().set(0,0,testPts2.back().get(0,0)-transX);
        }
//        for (int i=0; i<testPts1.size(); ++i)
//            cout << "!" << endl << testPts1[i];
        EpiGeom testGeom(testPts1, testPts2);
        cout << "Test Fund is: " << endl;
        cout << testGeom.getFund();

/*        testPts1.clear();
        testPts2.clear();
        for (int j=0; j<20; ++j) { 
            for (int i=0; i<20; ++i) {
                testPts1.push_back(Matrix(3,1));
                testPts1.back().set(0,0, i*10);
                testPts1.back().set(1,0, j*10);
                testPts1.back().set(2,0, 1);
            }
        }
        for (int i=0; i<(int)testPts1.size(); ++i) {
            testPts2.push_back(testPts1[i]);
            testPts2.back().set(0,0,testPts2.back().get(0,0)-transX);
        }
        EpiGeom testGeom2(testPts1, testPts2);
        cout << "Test Fund2 is: " << endl;
        cout << testGeom2.getFund();
*/
  /*      Matrix pt(3,1);
        pt.set3x1(100, 0, 1);

 
        cout << Util::homogenize(testGeom.getLineParamsL(pt)) << endl;
        cout << Util::homogenize(testGeom.getLineParamsR(pt)) << endl;*/
/*        cout << Util::homogenize(testGeom2.getLineParamsL(pt)) << endl;
        cout << Util::homogenize(testGeom2.getLineParamsR(pt)) << endl;
*/
        
		cvFindStereoCorrespondence(img3.get(), img4.get(), CV_DISPARITY_BIRCHFIELD, disparity.get(), 100);
		Fl_FundWindow win(img1, img2, disparity, geom);
        win.m_pts = pts;
        win.show();
        Fl::run();
		
		
    } catch (ImageMan::Exception& e) {
        IMERR_WARN(e.what());
    }

    return 0;
}