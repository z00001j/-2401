#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<event.h>
#include<jsoncpp/json/json.h>
#include<mysql/mysql.h>

using namespace std;
#define  LIS_MAX 10

enum OP_TYPE{DL=1,ZC,CKYY,YD,CKYD,QXYD,TC};

class mysql_client
{
public:
    mysql_client()
    {
        db_ips="127.0.0.1";
        db_username="root";
        db_dbname="Project_DB";
        db_password="060429";
    }

    ~mysql_client()
    {
        mysql_close(&mysql_con);
    }

    bool mysql_ConnectServer();
    bool mysql_Register(const string &tel,const string &password,const string &name);
    bool mysql_Login(const string &tel,const string &password, string &name);
    bool mysql_Show_Ticket(Json::Value &resval);
private:
    MYSQL mysql_con;
    string db_ips;
    string db_username;
    string db_dbname;
    string db_password;

};

class socket_listen
{
public:
    socket_listen()
    {
        sockfd=-1;
        m_port=6000;
        m_ips="127.0.0.1";
    }
    socket_listen(string ips,short port):m_ips(ips),m_port(port)
    {
        sockfd=-1;
    }
    bool socket_init();
    int accept_client();
    
    void Set_base(struct event_base* base)
    {
        this->base=base;
    }

    int Get_sockfd() const
    {
        return sockfd;
    }

    struct event_base* Get_base()const
    {
        return base;
    }
private:
    int sockfd;
    short m_port;
    string m_ips;
    struct event_base* base;
};

class socket_con
{    
public:
    socket_con(int fd):c(fd)
    {
        c_ev=NULL;
    }
    void Set_ev(struct event* ev)
    {
        c_ev=ev;
    }

    ~socket_con()
    {
        event_free(c_ev);
        close(c);
    }
    void Recv_data();
    void Send_err();
    void Send_ok();

    void User_Resgister();
    void User_Login();
    void User_Show_Ticket();


private:
    int c;
    struct event* c_ev;

    Json::Value val;
    
};
