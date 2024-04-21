#include<opencv2/opencv.hpp>
//#include "chuankou.cpp"
#include<iostream>
using namespace std;
using namespace cv;
const char *dev  = "/dev/ttyS0";
struct zuobiao  //定义一个结构体 储存三维坐标
{
    double x,y,z;
};
Mat Pretreatment(Mat img,int a0)//图像预处理
{
    Mat hsv,mask;
    cvtColor(img,hsv,COLOR_BGR2HSV);

    if(a0==1) //通过a0的值选择不同的颜色进行阀值化处理
    {
        Scalar lower_red(26, 41, 200);
        Scalar upper_red(52, 255, 255);

        inRange(hsv, lower_red, upper_red, mask);

    }
    else if(a0==2)
    {
        Scalar lower_red(69, 34, 255);
        Scalar upper_red(100, 255, 255);
        inRange(hsv, lower_red, upper_red, mask);
    }
    else{cout; "Unable to determine red and blue";<endl;waitkey(0);} mat="" element="getStructuringElement(MORPH_RECT,Size(5,5));" morphologyex(mask,mask,morph_close,element);="" imshow("yanmo",mask);="" return="" mask;="" }="" vector<vector<point="">&gt; Find_contour(Mat mask_find)//查找轮廓
{
    vector<vector<point>ann;
    vector<Vec4i>hir;
    findContours(mask_find,ann,hir,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);//查找轮廓

    return ann;
}
vector<Rect> Protract_contour(Mat img,vector<vector<point>&gt;ann_protract)//筛选轮廓
{

    vector<rect>rect;
    for (int i = 0; i &lt; ann_protract.size(); i++)
    {

        Rect rect_i=boundingRect(ann_protract[i]);


        if(rect_i.width*rect_i.height*310<img.rows*img.cols){continue;} if(rect_i.width="">rect_i.height)
        {
            if(rect_i.width/rect_i.height&gt;1.2){continue;}
        }
        else
        {
            if(rect_i.height/rect_i.width&gt;1.2){continue;}
        }

        rect.push_back(rect_i);

    }
    return rect;
}
Rect Select_priority_two(Mat img0,vector<Rect>rect0,int ck)//第二关的选择操作
{
    Rect rect_n1,rect_n2,rect_h;
    if(rect0.size()&gt;0)
    {
        for(int j=0;j<rect0.size()-1;j++) {="" for(int="" n="0;n<rect0.size()-j-1;n++)" rect_n1="rect0[n];" rect_n2="rect0[n+1];" if(rect_n1.x="">rect_n2.x)
                {swap(rect0[n],rect0[n+1]);}
            }
        }
        if(rect0.size()&gt;=3)
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
Rect Select_priority_four(Mat img1,vector<rect>rect1,int ck)//第四关的选择操作
{
    Rect rect_f1,rect_f2,rect_f;
    if(rect1.size()&gt;0)
    {
        for(int j=0;j<rect1.size()-1;j++) {="" for(int="" n="0;n<rect1.size()-j-1;n++)" rect_f1="rect1[n];" rect_f2="rect1[n+1];" if(rect_f1.x*rect_f1.y="">rect_f2.x*rect_f2.y)
                {swap(rect1[n],rect1[n+1]);}
            }
        }
        if(rect1.size()&gt;=3)
        {
            if(ck==5){rect_f=rect1[0];}
            else if(ck==6){rect_f=rect1[1];}
            else if(ck=7){rect_f=rect1[2];}
        }
        else if(rect1.size()&gt;=1)
        {
            rect_f=rect1[0];
        }
        rectangle(img1,Point(rect_f.x,rect_f.y),Point(rect_f.x+rect_f.width,rect_f.y+rect_f.height),Scalar(0,255,0),2);

    }
    return rect_f;
}
vector<double> Dimensions(Rect rect_ion)//pnp 得到相机位姿
{
    Mat cameraMatrix = (Mat_<double>(3, 3) &lt;&lt;542.5448628971645, 0, 308.6090582512243,0, 556.6297658902104, 67.84247127318397,0, 0, 1);
    Mat distCoeffs = (Mat_<double>(5, 1) &lt;&lt; 0.1030398998730105, -0.0166943280189579, -0.08003425365739675, 0.001095242436811841, -0.02006035890323307);

    vector<point2f> image=vector<point2f>
    {
        Point2f(rect_ion.x,rect_ion.y),
        Point2f(rect_ion.x+rect_ion.width,rect_ion.y),
        Point2f(rect_ion.x+rect_ion.width,rect_ion.y+rect_ion.height),
        Point2f(rect_ion.x,rect_ion.y+rect_ion.height)

    };
    vector<point3f> obj=vector<point3f>
    {
        Point3f(-2.5f,-2.5f,0),
        Point3f(2.5f,-2.5f,0),
        Point3f(2.5f,2.5f,0),
        Point3f(-2.5f,2.5f,0)

    };

    vector<double> rvec;
    vector<double> tvec;
    solvePnP(obj,image,cameraMatrix,distCoeffs,rvec,tvec);
    std::cout&lt;<rect_ion.x<<" "<<rect_ion.y<<endl;="" return="" tvec;="" }="" int="" main()="" {="" videocapture="" cap(2);="" mat="" fram;="" mask_main;="" vector<double=""> tvec_m2,tvec_m4;
    vector<vector<point>&gt;ann_main;
    vector<rect>rect_main;
    Rect rect_main_one;
    uint8_t buff[8];
    uint8_t accept[8];
    int num0=0;
    zuobiao zb;
    // serialPort myserial;
    cout&lt;&lt;"serialPort Test"&lt;<endl; myserial.openport(dev);="" myserial.setup(9600,0,8,1,'n');="" num0="accept[0];" int="" i="0;" a="0;" for(;;)="" {="" myserial.readbuffer(="" accept,="" 1);="" cin="">&gt;accept[0];
        num0=accept[0];
        cout&lt;<num0<<endl; if(num0="=49||num0==50){break;}" }="" a="num0-48;" for(;;)="" {="" cap="">&gt;fram;
        if(fram.empty())
        {
           cout&lt;&lt;"no void"&lt;<endl; break;="" }="" 接收="" myserial.readbuffer(="" accept,="" 1);="" num0="53;" if(num0="=52)" {="" i+="1;" mask_main="Pretreatment(fram,a);" ann_main="Find_contour(mask_main);" rect_main="Protract_contour(fram,ann_main);" if(i<1)="" if(rect_main.size()="">0)
                {
                    rect_main_one=Select_priority_two(fram,rect_main,num0);
                    tvec_m2=Dimensions(rect_main_one);
                    //发送
                    zb.x=tvec_m2[0];
                    zb.y=tvec_m2[1];
                    zb.z=tvec_m2[2];
                    cout&lt;&lt;"x"&lt;<tvec_m2[0]<<"\n"<<"y"<<tvec_m2[1]<<"\n"<<"z"<<tvec_m2[2]<<"\n"<<"aaaaa"<<endl; cout<<fram.cols<<"."<<fram.rows<<endl;640*360="" cout<<"x"<<(2*rect_main_one.x+rect_main_one.width)="" 2<<"\n"<<"y"<<(2*rect_main_one.y+rect_main_one.height)="" 2<<endl;="" 发送结构体给串口="" num0="a+48;" }="" else="" {="" if(rect_main.size()="">0)
                {
                    rect_main_one=Select_priority_four(fram,rect_main,num0);
                    tvec_m4=Dimensions(rect_main_one);

                    //发送
                    //num0=a+48;
                }
            }
        }

        imshow("jiance",fram);

        waitKey(1);
    }

    cap.release();
    return 0;
}</tvec_m2[0]<<"\n"<<"y"<<tvec_m2[1]<<"\n"<<"z"<<tvec_m2[2]<<"\n"<<"aaaaa"<<endl;></endl;></num0<<endl;></endl;></rect></vector<point></rect_ion.x<<"></double></double></point3f></point3f></point2f></point2f></double></double></double></rect1.size()-1;j++)></rect></rect0.size()-1;j++)></rect></img.rows*img.cols){continue;}></rect></vector<point></rect></vec4i></vector<point></endl;waitkey(0);}></iostream></opencv2>