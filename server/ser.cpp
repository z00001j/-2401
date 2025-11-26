#include"ser.h"

bool socket_listen::socket_init()
{
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if( -1==sockfd )
    {
        return false;
    }
    
    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(6000);
    saddr.sin_addr.s_addr=inet_addr(m_ips.c_str());

    int res=bind(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if( -1==res)
    {
        cout<<"bind err"<<endl;
        close(sockfd);
        return false;
    }
    res=listen(sockfd,LIS_MAX);
    if(res==-1)
    {
        return false;
    }
    return true;
}

int socket_listen::accept_client()
{
    int c=accept(sockfd,NULL,NULL);
    return c;
}
void socket_con::Recv_data()
{
    char buff[128]={0};
    int n=recv(c,buff,255,0);
    if(n<=0)
    {
        cout<<"client close"<<endl;
        delete this;
        return;
    }   

    //测试
    cout<<"recv:"<<buff<<endl;
    //解析
    Json::Value val;
    val["status"]="OK";
    send(c,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0);
}
void SOCK_CON_CALLBACK(int c,short ev,void* arg)
{
    socket_con* q=(socket_con*)arg;
   
    if(ev&EV_READ)
    {
        q->Recv_data();
    }
}

void SOCK_LIS_CALLBACK(int sockfd,short ev,void* arg)
{
    socket_listen* p=(socket_listen*)arg;
    if(p==NULL)
    {
        return ;
    }
    if(ev& EV_READ)//处理事件
    {
        int c=p->accept_client();
        if( c==-1 )
        {
            return;
        }
        cout<<"accept c="<<c<<endl;

        socket_con* q=new socket_con(c);

        struct event* c_ev=event_new(p->Get_base(),c,EV_READ|EV_PERSIST,SOCK_CON_CALLBACK,q);
        if(c_ev==NULL)
        {
            close(c);
            delete q;
            return ;
        }
        q->Set_ev(c_ev);
        //添加到libevent
        event_add(c_ev,NULL);
    }
    

}
int main()
{
    //监听套接字
    socket_listen sock_ser;
    if(!sock_ser.socket_init())
    {
        cout<<"socket_init err"<<endl;
        exit(1);
    }


    //创建libevent base
    struct event_base * base=event_init();
    if(base==NULL)
    {
        cout<<"base nurr"<<endl;
        exit(1);
    }

    //设置socket_listen中的libevent的base
    sock_ser.Set_base(base);

    //添加sockfd到libevent
    struct event *sock_ev=event_new(base,sock_ser.Get_sockfd(),EV_READ|EV_PERSIST,SOCK_LIS_CALLBACK,&sock_ser);
    event_add(sock_ev,NULL);
    //启动事件循环
    event_base_dispatch(base);
    
    //释放资源
    event_free(sock_ev);
    event_base_free(base);
}