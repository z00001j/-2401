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
    cout<<"登陆"<<endl;
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
            break;
        case YD:
            break;
        case QXYD:
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