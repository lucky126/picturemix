#include<iostream>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include "sharedmatting.h"

using namespace std;
using namespace cv;

#define threshold 220

Mat orgpic,maskpic,outputpic;

int mix1(uchar cb,uchar cg, uchar cr){
    // Convert Mat to float data type
    orgpic.convertTo(orgpic, CV_32FC3);
	Mat background=Mat(orgpic.rows,orgpic.cols,CV_8UC3,Scalar(cb,cg,cr));
    background.convertTo(background, CV_32FC3);
	// Normalize the alpha mask to keep intensity between 0 and 1
    maskpic.convertTo(maskpic, CV_32FC3, 1.0/255); // 

    // Storage for output image
    Mat ouImage = Mat::zeros(orgpic.size(), orgpic.type());

	printf("mix init ready ... \n");
    // Multiply the foreground with the alpha matte
	cout << "MaskType:" << maskpic.type() << " " <<
			"OrgPicType:" << orgpic.type() << endl;

    multiply(maskpic, orgpic, orgpic); 
	printf("mask * org ready ... \n");

    // Multiply the background with ( 1 - alpha )
    multiply(Scalar::all(1.0)-maskpic, background, background); 
	printf("mask * background ready ... \n");

    // Add the masked foreground and background.
    add(orgpic, background, ouImage); 
	printf("add ready ... \n");
 
	outputpic=ouImage/255;
	
    return 0;
}
//single pixel access demo
void Flip(Mat &img)
{
    int rows=img.rows;
    int cols=img.cols;
    for(int i=0; i<rows; i++)
    {
        for(int j=0; j<cols/2; j++)
        {
            uchar t;
            if(img.channels()==1)
            {
                t=img.at<uchar>(i,j);
                img.at<uchar>(i,j)=img.at<uchar>(i,cols-1-i);
                img.at<uchar>(i,cols-1-i)=t;
            }
            else if(img.channels()==3)
            {
                for(int k=0; k<3; k++)
                {
                    t=img.at<Vec3b>(i,j)[k];
                    img.at<Vec3b>(i,j)[k]=img.at<Vec3b>(i,cols-1-j)[k];
                    img.at<Vec3b>(i,cols-1-j)[k]=t;
                }
            }
        }
    }
}

void waitESC(){
	while(int key=waitKey(0) != 27){};
}

void makeMask(char *orgname,char *rawmaskname, Mat &mask){
	SharedMatting sm;
	Mat m;
	sm.loadImage(orgname);
	sm.loadTrimap(rawmaskname);
	sm.solveAlpha();
	m=sm.getMaskMat();
	m.copyTo(mask);
}

int main(int argc, char **argv) {
	Mat mask1;
	if (argc != 5) {
		printf("usage: ./mix orgpic.jpg rawmask.jpg output.jpg 0x00ff00\n");
		exit(1);
	}
	char *endptr = NULL; 
	unsigned int nFillColor=strtoul(argv[4],&endptr,16);
	
	makeMask(argv[1],argv[2],mask1);
	//mask1.convertTo(maskpic, CV_8UC3);
	cvtColor(mask1,maskpic,CV_GRAY2RGB);
	//maskpic = imread(argv[2]);
	orgpic = imread(argv[1]);
	//imshow("mask",maskpic);
	//imshow("org",orgpic);
	printf("Mask[w:%d,h:%d]   orgPic[w:%d,h:%d]\n",
		maskpic.rows,maskpic.cols,
		orgpic.rows,orgpic.cols);
	cout << "MaskType:" << maskpic.type() << " " <<
			"OrgPicType:" << orgpic.type() << endl;

	//waitESC();
	//maskpic = imread(argv[2]);
	
	/*
	//Mask pic must be gray
	if (maskpic1.type() == CV_8UC1) maskpic = maskpic1;
	    else if (maskpic1.type() == CV_8UC3) cvtColor(maskpic1, maskpic, CV_BGR2GRAY);
	    else if (maskpic1.type() == CV_8UC4) cvtColor(maskpic1, maskpic, CV_BGRA2GRAY);
		else {
			printf("Mask picture must be GRAY mask\n");
			exit(2);
		}
	if (orgpic1.type() == CV_8UC3) orgpic = orgpic1;
	    else if (maskpic1.type() == CV_8UC4) cvtColor(orgpic1, orgpic, CV_BGRA2BGR);
		else {
			printf("Org picture must be RGB color photo\n");
			exit(3);
		}
	*/
	//outputpic=orgpic;

	uchar cb=nFillColor >> 16 & 255;
	uchar cg=nFillColor >> 8 & 255;
	uchar cr=nFillColor & 255;
	//printf("Fill color with: BLUE(%02x) GREEN(%02x) RED(%02x)\n",cb,cg,cr);
	//outputpic=Mat(orgpic.rows,orgpic.cols,CV_8UC3,Scalar(cb,cg,cr));
/*	
	if (maskpic.channels() != 1) {
		cout << "Mask pic must be a gray picture!." << endl;
		printf("current mask pic has %d channels \n",maskpic.channels());
		exit(1);
	}
	*/
	if ((maskpic.rows != orgpic.rows) || (maskpic.cols != orgpic.cols)) {
		cout << "Org picture and Mask picture must have same size!" << endl;
	}

	mix1(cb,cg,cr);
	outputpic.convertTo(outputpic, CV_8UC3,255);
	//imshow("out",outputpic);
	//waitESC();
	imwrite(argv[3],outputpic);
	
	return 0;
}

