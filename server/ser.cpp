#include "ser.h"

bool mysql_client::mysql_ConnectServer()
{
    MYSQL* mysql=mysql_init(&mysql_con);
    if(mysql==NULL)
    {
        return false;
    }

    mysql=mysql_real_connect(mysql,db_ips.c_str(),db_username.c_str(),db_password.c_str(),db_dbname.c_str(),3306,NULL,0);
    if(mysql==NULL)
    {
        cout<<"connect db_server err"<<endl;
        return false;
    }
    return true;
}
bool mysql_client::mysql_Register(const string &tel,const string &password,const string &name)
{
    //insert into user_info values(0,13300000000,'小李','123456',1);
    string sql=string("insert into user_info values(0,'")+tel+string("','")+name+string("','")+password+string("',1);");
    if(mysql_query(&mysql_con,sql.c_str())!=0)
    {
        return false;
    }
    return true;
}
bool socket_listen::socket_init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        return false;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());

    int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (-1 == res)
    {
        cout << "bind err" << endl;
        close(sockfd);
        return false;
    }
    res = listen(sockfd, LIS_MAX);
    if (res == -1)
    {
        return false;
    }
    return true;
}

int socket_listen::accept_client()
{
    int c = accept(sockfd, NULL, NULL);
    return c;
}
void socket_con::Send_err()
{
      Json::Value res_val;
      res_val["status"]="ERR";
      send(c,res_val.toStyledString().c_str(),strlen(res_val.toStyledString().c_str()),0);
}
void socket_con::Send_ok()
{
     Json::Value res_val;
     res_val["status"]="OK";
     send(c,res_val.toStyledString().c_str(),strlen(res_val.toStyledString().c_str()),0);
}
//用户注册
void socket_con::User_Resgister()
{
    string tel,password,username;
    tel=val["user_tel"].asString();
    password=val["user_password"].asString();
    username=val["user_name"].asString();

    if(tel.empty()||password.empty()||username.empty())
    {
        Send_err();
    }
    mysql_client cli;
    if(!cli.mysql_ConnectServer())
    {
        Send_err();
        return ;
    }

    if(!cli.mysql_Register(tel,password,username))
    {
        Send_err();
        return ;
    }

    Send_ok();
    return;
}
//用户登陆
void socket_con::User_Login()
{

}
void socket_con::Recv_data()
{
    char buff[128] = {0};
    int n = recv(c, buff, 255, 0);
    if (n <= 0)
    {
        cout << "client close" << endl;
        delete this;
        return;
    }

    // 测试
    cout << "recv:" << buff << endl;

    Json::Reader Read;
    if (!Read.parse(buff, val))
    {
        cout << "Recv_data:解析Json失败"<<endl;
        Send_err();
        return;
    }
    
    int ops=val["type"].asInt();
    //DL=1，ZC，CKYY，YD，CKYD，QXYD，TC
    switch(ops)
    {
    case DL:
        User_Login();
        break;
    case ZC:
        User_Resgister();
        break;

    default:
        break;
    }

// 解析
}
void SOCK_CON_CALLBACK(int c, short ev, void *arg)
{
    socket_con *q = (socket_con *)arg;

    if (ev & EV_READ)
    {
        q->Recv_data();
    }
}

void SOCK_LIS_CALLBACK(int sockfd, short ev, void *arg)
{
    socket_listen *p = (socket_listen *)arg;
    if (p == NULL)
    {
        return;
    }
    if (ev & EV_READ) // 处理事件
    {
        int c = p->accept_client();
        if (c == -1)
        {
            return;
        }
        cout << "accept c=" << c << endl;

        socket_con *q = new socket_con(c);

        struct event *c_ev = event_new(p->Get_base(), c, EV_READ | EV_PERSIST, SOCK_CON_CALLBACK, q);
        if (c_ev == NULL)
        {
            close(c);
            delete q;
            return;
        }
        q->Set_ev(c_ev);
        // 添加到libevent
        event_add(c_ev, NULL);
    }
}
int main()
{
    // 监听套接字
    socket_listen sock_ser;
    if (!sock_ser.socket_init())
    {
        cout << "socket_init err" << endl;
        exit(1);
    }

    // 创建libevent base
    struct event_base *base = event_init();
    if (base == NULL)
    {
        cout << "base nurr" << endl;
        exit(1);
    }

    // 设置socket_listen中的libevent的base
    sock_ser.Set_base(base);

    // 添加sockfd到libevent
    struct event *sock_ev = event_new(base, sock_ser.Get_sockfd(), EV_READ | EV_PERSIST, SOCK_LIS_CALLBACK, &sock_ser);
    event_add(sock_ev, NULL);
    // 启动事件循环
    event_base_dispatch(base);

    // 释放资源
    event_free(sock_ev);
    event_base_free(base);
}