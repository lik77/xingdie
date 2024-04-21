#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
Mat yvchuli(Mat img)//用于图像的预处理
{
    //通道分离
    vector<Mat>ann;
    split(img,ann);
    Mat img0=ann[2]-ann[0]/6.3;

    // 二值化
    Mat img1;
    threshold(img0,img1,215,255,THRESH_BINARY);
    // imshow("img1",img1);

    // 高斯
    GaussianBlur(img1,img1,Size(5,5),0);

    // 形态学
    Mat img2;
    Mat element=getStructuringElement(MORPH_RECT,Size(7,7));
    morphologyEx(img1,img2,MORPH_CLOSE,element);
    // imshow("img2",img2);

    return img2;
}
vector<vector<Point>> findlunkuo(Mat img2,Mat img)//查找轮廓
{
    vector<vector<Point>>ann;
    vector<Vec4i>hir;
    findContours(img2,ann,hir,RETR_TREE,CHAIN_APPROX_NONE);

    // drawContours(img,ann,-1,Scalar(0,0,0),5);   //放个绘制轮廓便于观察

    return ann;
}
RotatedRect rrr[2];Rect sanwei[2];
void shaixuan(vector<vector<Point>>ann0,Mat imgg)//设置条件筛选轮廓
{
    for(int i=0;i<ann0.size();i++)
    {
        RotatedRect roi=minAreaRect(ann0[i]);
        Rect aaa0=boundingRect(ann0[i]);//三维

        //轮廓过小\过小则排除
        if(roi.size.height*roi.size.width<30&&roi.size.height*roi.size.width>(imgg.cols*imgg.rows)/6)
        {continue;}

        //长宽比小于2.2的排除
        if(roi.size.height>roi.size.width)
        {
            if(roi.size.height/roi.size.width<2.2){continue;}
        }
        else if(roi.size.width>roi.size.height)
        {
            if(roi.size.width/roi.size.height<2.2){continue;}
        }

        for(int j=0;j<ann0.size();j++)//轮廓矩形之间对比
        {
            if(i==j){continue;}

            RotatedRect roj=minAreaRect(ann0[j]);
            Rect aaa1=boundingRect(ann0[j]);//三维
           
            //角度尝试
            if(roj.size.width>roj.size.height)
            {
                if(roi.angle<50||roj.angle<50){continue;}
            }
            else if(roj.size.height>roj.size.width)
            {
                if(roi.angle>40||roj.angle>40){continue;}
                else if(roj.angle==90||roi.angle==90){continue;}//横着的矩形排除
            }

            //面积差不可大于两个轮廓的最大面积的一半
            if(abs(roj.size.height*roj.size.width-roi.size.height*roj.size.width)>max(roj.size.height*roj.size.width,roi.size.height*roj.size.width)/2)
            {continue;}

            //角度差大则排除
            if(abs(roi.angle-roj.angle)>15){continue;}
           

            // //中心点y值相差大则排除
            if(abs (roi.center.y-roj.center.y)>abs((roi.size.width-roi.size.height)) || abs (roi.center.y-roj.center.y)>abs( (roj.size.width-roj.size.height)))
            {continue;}

            // //中心值x值相差大则排除
            if(abs (roi.center.x-roj.center.x)> abs((roi.size.width-roi.size.height)*4) || abs (roi.center.x-roj.center.x)>abs( (roj.size.width-roj.size.height)*4 ))
            {continue;}

            // //中心值x值相差小也排除
            if(abs (roi.center.x-roj.center.x)< abs((roi.size.width-roi.size.height)*2) || abs (roi.center.x-roj.center.x)<abs( (roj.size.width-roj.size.height)*2 ))
            {continue;}
           
            // cout<<"roi的角度"<<roi.angle<<endl;
            // cout<<"roj的角度"<<roj.angle<<endl;
           
            //筛选完的轮廓矩形
            rrr[0]=roj;
            rrr[1]=roi;

            //二维信息接收
            sanwei[0]=aaa0;
            sanwei[1]=aaa1;

        }
    }
}
void jvxing(Mat img2)//画出中心点和围住装甲板
{
    //瞄准装甲板中心点
    circle( img2,Point( (rrr[0].center.x+rrr[1].center.x)/2,(rrr[0].center.y+rrr[1].center.y)/2 ),10,Scalar(0,0,255),FILLED );

    //画装甲板矩形
    Point2f vect0[4];
    Point2f vect1[4];
    rrr[0].points(vect0);
    rrr[1].points(vect1);
    if(vect0[0].x<vect1[0].x)
    {
        if(vect0[3].y>vect0[1].y){rectangle(img2,vect0[0],vect1[2],Scalar(0,255,0),5);}
        else{rectangle(img2,vect0[2],vect1[0],Scalar(0,255,0),5);}
    }
    else
    {
        if(vect0[3].y>vect0[1].y){rectangle(img2,vect0[2],vect1[0],Scalar(0,255,0),5);}
        else{rectangle(img2,vect0[0],vect1[2],Scalar(0,255,0),5);}
    }

    //围住灯，看看问题
    // Point2f vect[4];
    //     rrr[0].points(vect);
    //     for(int j=0;j<4;j++)
    //     {
    //         line(img2,vect[j],vect[(j+1)%4],Scalar(0,255,0),5);
    //     }
    //     rrr[1].points(vect);
    //     for(int j=0;j<4;j++)
    //     {
    //         line(img2,vect[j],vect[(j+1)%4],Scalar(0,255,0),5);
    //     }
       
}
int sanweia(Mat framimg0)//输出平移向量和旋转向量
{
    Mat cameraMatrix = (Mat_<double>(3, 3) <<976.3268391629416, 0, 126.2400522817966,0, 745.3827832976327, 199.5296404260386,0, 0, 1);//内参矩阵
    Mat distCoeffs = (Mat_<double>(5, 1) << 0.5200687726809431, -0.3777490426637254, -0.045160952365915, -0.1042501407959941, -0.1492688106449521);//畸变系数
   
    vector<Point2f> image;
    if(sanwei[0].x<sanwei[1].x)//相机坐标系上的二维坐标
    {
        image=vector<Point2f>
        {
        Point2f(sanwei[0].x+sanwei[0].width,sanwei[0].y),
        Point2f(sanwei[1].x,sanwei[1].y),
        Point2f(sanwei[1].x,sanwei[1].y+sanwei[1].height),
        Point2f(sanwei[0].x+sanwei[0].width,sanwei[0].y+sanwei[0].height)
        };
    }
    else if(sanwei[1].x<sanwei[0].x)
    {
        image=vector<Point2f>
        {
            Point2f(sanwei[1].x+sanwei[1].width,sanwei[1].y),
            Point2f(sanwei[0].x,sanwei[0].y),
            Point2f(sanwei[0].x,sanwei[0].y+sanwei[0].height),
            Point2f(sanwei[1].x+sanwei[1].width,sanwei[1].y+sanwei[1].height)
        };
    }
    else if(sanwei[1].x==sanwei[0].x){return 0;}
   
    vector<Point3f> obj=vector<Point3f>//世界坐标系
    {
        Point3f(-6.15f,2.5f,0),
        Point3f(-6.15f,-2.5f,0),
        Point3f(6.15f,-2.5f,0),
        Point3f(6.15f,2.5f,0)
    };
    Mat rvec, tvec;
    solvePnP(obj,image,cameraMatrix,distCoeffs,rvec,tvec);

    cout<<"三维坐标"<<tvec<<endl;

    return 0;
}
int main()
{
     VideoCapture cap(0);
    cap.set(CAP_PROP_EXPOSURE,500);
       for(;;)
    {
        Mat fram;
        cap>>fram;
        // fram=imread("/home/duan/桌面/_cgi-bin_mmwebwx-bin_webwxgetmsgimg &MsgID=5918751192324073520&skey=@crypt_7217a448_74fbdec6adf975ce5157eb9cf2974bac&mmweb_appid=wx_webfilehelper.jpeg");
        Mat framimg=fram;

        //调整大小
        resize(fram,fram,Size(0,0),2,2);
        resize(framimg,framimg,Size(0,0),2,2);

        //预处理
        fram=yvchuli(fram);
       
        //查找轮廓
        vector<vector<Point>>abb;
        abb=findlunkuo(fram,framimg);
       
        //筛选轮廓矩形
        shaixuan(abb,fram);

       //画装甲板矩形和中心点
       jvxing(framimg);

       //三维信息
       sanweia(framimg);

       //窗口
       imshow("检测窗口",fram);
       imshow("结果窗口",framimg);

        waitKey(1);
    }
    return 0;
}