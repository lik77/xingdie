#include<opencv2/opencv.hpp>
//#include "serialPort.cpp"
#include<iostream>
#include <cstring>
using namespace std;
using namespace cv;
const char *dev  = "/dev/ttyS0";
Mat Pretreatment(Mat img,int a0)
{
    Mat hsv,mask;
    cvtColor(img,hsv,COLOR_BGR2HSV);

    if(a0==1)
    {
        Scalar lower_red(26, 41, 200);
        Scalar upper_red(52, 255, 255);

        inRange(hsv, lower_red, upper_red, mask);

    }
    else if(a0==2)
    {
        Scalar lower_blue(69, 34, 255);
        Scalar upper_blue(100, 255, 255);
        inRange(hsv, lower_blue, upper_blue, mask);
    }
    else{cout<<"Unable to determine red and blue"<<endl;waitKey(0);}
   
    Mat element=getStructuringElement(MORPH_RECT,Size(5,5));
    morphologyEx(mask,mask,MORPH_CLOSE,element);
   
    //imshow("yanmo",mask);
    return mask;

}
vector<vector<Point>> Find_contour(Mat mask_find)
{
    vector<vector<Point>>ann;
    vector<Vec4i>hir;
    findContours(mask_find,ann,hir,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

    return ann;
}
vector<Rect> Protract_contour(Mat img,vector<vector<Point>>ann_protract)
{

    vector<Rect>rect;
    for (int i = 0; i < ann_protract.size(); i++)
    {

        Rect rect_i=boundingRect(ann_protract[i]);


        if(rect_i.width*rect_i.height*310<img.rows*img.cols){continue;}


        if(rect_i.width>rect_i.height)
        {
            if(rect_i.width/rect_i.height>1.2){continue;}
        }
        else
        {
            if(rect_i.height/rect_i.width>1.2){continue;}
        }
 
        rect.push_back(rect_i);

    }
    return rect;
}
Rect Select_priority_two(Mat img0,vector<Rect>rect0,int ck)
{
    Rect rect_n1,rect_n2,rect_h;
    if(rect0.size()>0)
    {
        for(int j=0;j<rect0.size()-1;j++)
        {
            for(int n=0;n<rect0.size()-j-1;n++)
            {
                rect_n1=rect0[n];
                rect_n2=rect0[n+1];
                if(rect_n1.x>rect_n2.x)
                {swap(rect0[n],rect0[n+1]);}
            }
        }
        if(rect0.size()>=3)
        {
            if(ck==5){rect_h=rect0[0];}
            if(ck==6){rect_h=rect0[1];}
            if(ck==7){rect_h=rect0[2];}
        }
        else if(rect0.size()==2)
        {
            if(ck==5){rect_h=rect0[0];}
            else if(ck==6){rect_h=rect0[1];}
            else if(ck==7){rect_h=rect0[1];}
        }
        else if(rect0.size()==1)
        {
            rect_h=rect0[0];
        }

        rectangle(img0,Point(rect_h.x,rect_h.y),Point(rect_h.x+rect_h.width,rect_h.y+rect_h.height),Scalar(0,255,0),2);
    }
    return rect_h;
}
Rect Select_priority_four(Mat img1,vector<Rect>rect1,int ck)
{
    Rect rect_f1,rect_f2,rect_f;
    if(rect1.size()>0)
    {
        for(int j=0;j<rect1.size()-1;j++)
        {
            for(int n=0;n<rect1.size()-j-1;n++)
            {
                rect_f1=rect1[n];
                rect_f2=rect1[n+1];
                if(rect_f1.x*rect_f1.y>rect_f2.x*rect_f2.y)
                {swap(rect1[n],rect1[n+1]);}
            }
        }
        if(rect1.size()>=3)
        {
            if(ck==5){rect_f=rect1[0];}
            else if(ck==6){rect_f=rect1[1];}
            else if(ck==7){rect_f=rect1[2];}
        }
        else if(rect1.size()>=1)
        {
            rect_f=rect1[0];
        }
        rectangle(img1,Point(rect_f.x,rect_f.y),Point(rect_f.x+rect_f.width,rect_f.y+rect_f.height),Scalar(0,255,0),2);
       
    }
    return rect_f;
}
vector<double> Dimensions(Rect rect_ion)//pnp
{
    Mat cameraMatrix = (Mat_<double>(3, 3) <<542.5448628971645, 0, 308.6090582512243,0, 556.6297658902104, 67.84247127318397,0, 0, 1);
    Mat distCoeffs = (Mat_<double>(5, 1) << 0.1030398998730105, -0.0166943280189579, -0.08003425365739675, 0.001095242436811841, -0.02006035890323307);

    vector<Point2f> image=vector<Point2f>
    {
        Point2f(rect_ion.x,rect_ion.y),
        Point2f(rect_ion.x+rect_ion.width,rect_ion.y),
        Point2f(rect_ion.x+rect_ion.width,rect_ion.y+rect_ion.height),
        Point2f(rect_ion.x,rect_ion.y+rect_ion.height)
       
    };
    vector<Point3f> obj=vector<Point3f>
    {
        Point3f(-2.5f,-2.5f,0),
        Point3f(2.5f,-2.5f,0),
        Point3f(2.5f,2.5f,0),
        Point3f(-2.5f,2.5f,0)

    };
   
    vector<double> rvec;
    vector<double> tvec;
    solvePnP(obj,image,cameraMatrix,distCoeffs,rvec,tvec);
    return tvec;
}
int main()
{
    VideoCapture cap(0);
    Mat fram;
    Mat mask_main;
    vector<double> tvec_m2,tvec_m4;
    vector<vector<Point>>ann_main;
    vector<Rect>rect_main;
    Rect rect_main_one;
    string x,y,z,pitch,yaw;
    double pitch_d,yaw_d;
    uint8_t buff[x.length()+z.length()+1];
    uint8_t accept[8];
    int num0=0;
    //serialPort myserial;
    cout<<"serialPort Test"<<endl;
    //myserial.OpenPort(dev);
    //myserial.setup(9600,0,8,1,'N');
    num0=accept[0];
    int i=0;
    int a=0;
   

    for(;;)
    {
       // myserial.readBuffer( accept, 1);
       cin>>accept[0];
        num0=accept[0];
        cout<<num0<<endl;
        if(num0==49||num0==50){break;}
    }
    a=num0-48;
    for(;;)
    {
   
        cap>>fram;
        if(fram.empty())
        {
           cout<<"no void"<<endl;
           break;
        }

        //myserial.readBuffer( accept, 1);
        cin>>accept[0];
        num0=accept[0];
       
       
        if(num0==52)
        {
           i+=1;
        }

        if(num0==53||num0==54||num0==55)
        {
            mask_main=Pretreatment(fram,a);      

            ann_main=Find_contour(mask_main);

            rect_main=Protract_contour(fram,ann_main);    
           
            if(i<1)
            {
                if(rect_main.size()>0)
                {
                    rect_main_one=Select_priority_two(fram,rect_main,num0);
                    tvec_m2=Dimensions(rect_main_one);
                   
                    x=to_string(tvec_m2[0]);
                    y=to_string(tvec_m2[1]);
                    z=to_string(tvec_m2[2]);
                    memcpy(buff, x.c_str(), x.length());
                    memcpy(buff + x.length()+1, z.c_str(), z.length());
                    buff[x.length()] = '\0';
                   
                    //myserial.writeBuffer( buff, x.length()+z.length()+1);
                    //cout<<"x"<<to_string(x)<<"\n"<<"y"<<to_string(y)<<"\n"<<"z"<<to_string(z)<<endl;
                   
                }
            }
            else
            {
                if(rect_main.size()>0)
                {
                    rect_main_one=Select_priority_four(fram,rect_main,num0);
                    tvec_m4=Dimensions(rect_main_one);
                    x=to_string(tvec_m2[0]);
                    y=to_string(tvec_m2[1]);
                    z=to_string(tvec_m2[2]);
             
                    pitch_d=atan(stod(y)/stod(z));
                    yaw_d=atan(stod(x)/stod(z));
                    pitch=to_string(pitch_d);
           
                    memcpy(buff, pitch.c_str(), pitch.length());
                    memcpy(buff + yaw.length()+1, yaw.c_str(), yaw.length());
                    buff[pitch.length()] = '\0';
                   
                    //myserial.writeBuffer( buff, pitch.length()+yaw.length()+1);
                }
            }
           
        }
           
        imshow("jiance",fram);

        waitKey(1);
    }
   
    cap.release();
    return 0;
}