#include"cli.h"

bool socket_client::Connect_server()
{
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        cout<<"create socket err"<<endl;
        return false;
    }

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(6000);
    saddr.sin_addr.s_addr=inet_addr(ips.c_str());
    int res=connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(res==-1)
    {
        cout<<"connect err"<<endl;
        return false;
    }

    cout<<"connect to Server success"<<endl;
    return true;
}

void socket_client::print_info()
{
    if(dl_flg)
    {
        cout<<"---已登录------用户名："<<username<<"----"<<endl;
        cout<<"1.查看预约  2.预定  3.查看我的预约  4.取消预约   5.退出"<<endl;
        cout<<"-------------------------------------------------"<<endl;
        cout<<"请输入选项编号:"<<endl;
        cin>>user_op;
        user_op+=OFFSET;
    }
    else
    {
        cout<<"---未登录------游客----"<<endl;
        cout<<"1.登陆   2.注册  3.退出"<<endl;
        cout<<"---------------------"<<endl;
        cout<<"请输入选项编号:"<<endl;
        cin>>user_op;
        if(user_op==3)
        {
            user_op=TC;
        }
    }
}
void socket_client::User_Resgister()
{
    cout<<"请输入用户手机号码："<<endl;
    cin>>usertel;
    cout<<"请输入用户名："<<endl;
    cin>>username;
    string password,tmp;
    cout<<"请输入密码"<<endl;
    cin>>password;
    cout<<"请再次输入密码"<<endl;
    cin>>tmp;

    if(usertel.empty()||username.empty())
    {
        cout<<"手机或用户名不能空"<<endl;
        return ;
    }
    if(password.compare(tmp)!=0)
    {
        cout<<"密码不一致"<<endl;
        return ;
    }

    Json::Value val;
    val["type"]=ZC;
    val["user_tel"]=usertel;
    val["user_name"]=username;
    val["user_password"]=password;
    
    send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
    
    char buff[256]={0};
    if(recv(sockfd,buff,255,0)<=0)
    {
        cout<<"ser close"<<endl;
        return;
    }

    val.clear();
    Json::Reader Read;
    if(!Read.parse(buff,val))
    {
        cout<<"Json 解析失败"<<endl;
        return ;
    }

    string s=val["status"].asString();
    if(s.compare("OK")!=0)
    {
        cout<<"注册失败"<<endl;
        return;
    }
    dl_flg=true;
    cout<<"注册成功"<<endl;
    return;
}
void socket_client::User_Login()
{
    string tel,password;
    cout<<"请输入手机号码"<<endl;
    cin>>tel;
    cout<<"请输入密码"<<endl;
    cin>>password;

    if(tel.empty()||password.empty())
    {
        cout<<"帐号与密码不能为空"<<endl;
        return ;
    }
    
    Json::Value val;
    val["type"]=DL;
    val["user_tel"]=tel;
    val["user_password"]=password;

    send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);

    char buff[256]={0};
    int n=recv(sockfd,buff,255,0);
    if(n<=0)
    {
        cout<<"ser close"<<endl;
    }

    val.clear();
    Json::Reader Read;
    if(!Read.parse(buff,val))
    {
        cout<<"解析json失败"<<endl;
        return;
    }

    string st=val["status"].asString();
    if(st.compare("OK")!=0)
    {
        cout<<"登陆失败"<<endl;
        return;
    } 

    dl_flg=true;

    username=val["user_name"].asString();
    usertel=tel;

    cout<<"登陆成功"<<endl;
    return ;
}
void socket_client::User_Show_Ticket()
{
    Json::Value val;
    val["type"]=CKYY;
    send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
    //
    char buff[4096]={0};
    int n=recv(sockfd,buff,4095,0);
    //cout<<buff<<endl;
    if(n<=0)
    {
        cout<<"ser close"<<endl;
        return;
    }

    m_val.clear();
    Json::Reader Read;
    if(!Read.parse(buff,m_val))
    {
        cout<<"解析失败"<<endl;
        return;
    }

    string st=m_val["status"].asString(); 
    if(st.compare("OK")!=0)
    {
        cout<<"查询预约信息失败"<<endl;
        return;
    }

    int num=m_val["num"].asInt();
    if(num==0)
    {
        cout<<"没有可预约的信息"<<endl;
        return;
    }

    cout<<"编号  地点名称    总票数  已预定 时间    "<<endl;
    for(int i=0;i<num;i++)
    {
        cout<<"----------------------------------------"<<endl;
        cout<<m_val["arr"][i]["tk_id"].asString()<<"  ";
        cout<<m_val["arr"][i]["add"].asString()<<"  ";
        cout<<m_val["arr"][i]["max"].asString()<<"  ";
        cout<<m_val["arr"][i]["num"].asString()<<"  ";
        cout<<m_val["arr"][i]["use_date"].asString()<<"  "<<endl;
        cout<<"----------------------------------------"<<endl;
    }
    cout<<endl;
}
//显示我的预约
void socket_client::User_Show_Sub_Ticket()
{
    Json::Value val;
    val["type"]=CKYD;
    val["tel"]=usertel;
    send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);

    char buff[4096]={0};
    int n=recv(sockfd,buff,4095,0);
    if(n<=0)
    {
        cout<<"ser close"<<endl;
        return ;
    }

    m_val.clear();
    Json::Reader Read;
    if(!Read.parse(buff,m_val))
    {
        cout<<"解析失败"<<endl;
        return;
    }

    string st=m_val["status"].asString();
    if(st.compare("OK")!=0)
    {
        cout<<"查询预定消息失败"<<endl;
        return;
    }

    int num=m_val["num"].asInt();
    if(num==0)
    {
        cout<<"没有任何预定信息"<<endl;
        return;
    }

    cout<<"编号     tk_id    地点         使用时间        购买时间   "<<endl;
    for(int i=0;i<num;i++)
    {
        cout<<"------------------------------------------------------------"<<endl;
        cout<<m_val["arr"][i]["yi_id"].asString()<<"      ";
        cout<<m_val["arr"][i]["tk_id"].asString()<<"        ";
        cout<<m_val["arr"][i]["addr"].asString()<<"     ";
        cout<<m_val["arr"][i]["use_date"].asString()<<"     ";
        cout<<m_val["arr"][i]["curr_time"].asString()<<endl;
        cout<<"------------------------------------------------------------"<<endl;
    }
    cout<<endl;
    return;
}
//取消一条我的预约
void socket_client::User_Cancel_Sub_Ticket()
{
    User_Show_Sub_Ticket();
    cout<<"请输入你要删除的编号："<<endl;
    int index=0;
    cin>>index;

    Json::Value val;
    val["type"]=QXYD;
    val["index"]=index;
    send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);

    char buff[4096]={0};
    int n=recv(sockfd,buff,4095,0);
    if(n<=0)
    {
        cout<<"ser close"<<endl;
        return ;
    }

    val.clear();
    Json::Reader Read;
    if(!Read.parse(buff,val))
    {
        cout<<"Json解析失败"<<endl;
        return;
    }

    string st=val["status"].asString();
    if(st.compare("OK")!=0)
    {
        cout<<"取消预定失败"<<endl;
        return;
    }
    cout<<"取消预定成功"<<endl;
    return;
}
void socket_client::User_Subscribe_Ticket()
{
    User_Show_Ticket();
    cout<<"请输入要预定的编号："<<endl;
    int index=0;
    cin>>index;
    //index 有效性检查

    Json::Value val;
    val["type"]=YD;
    val["tel"]=usertel;
    val["index"]=index;

    send(sockfd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);

    char buff[256]={0};
    int n=recv(sockfd,buff,255,0);
    if(n<=0)
    {
        cout<<"ser close"<<endl;
        return;
    }

    val.clear();
    Json::Reader Read;
    if(!Read.parse(buff,val))
    {
        cout<<"json解析失败"<<endl;
        return;
    }

    string st=val["status"].asString();
    if(st.compare("OK")!=0)
    {
        cout<<"预定失败"<<endl;
        return;
    }
    cout<<"预定成功"<<endl;
    return;
}

void socket_client::Run()
{
    while(runing)
    {
        print_info();

        switch (user_op)
        {
        case DL:
            User_Login();
            break;
        case ZC:
            User_Resgister();
            break;  
        case CKYY:
            User_Show_Ticket();
            break;
        case YD:
            User_Subscribe_Ticket();
            break;
        case QXYD:
            User_Cancel_Sub_Ticket();
            break;
        case CKYD:
            User_Show_Sub_Ticket();
            break;
        case TC:
            runing=false; 
            break;
        default:
            cout<<"输入无效，请重新输入"<<endl;
            break;
        }
    }
}

int main()
{
    socket_client cli;
    if(!cli.Connect_server())
    {
        exit(1);
    }
    cli.Run();

    exit(0);
}