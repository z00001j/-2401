-- 创建数据库
CREATE DATABASE IF NOT EXISTS Project_DB CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE Project_DB;

-- 用户表
CREATE TABLE IF NOT EXISTS user_info (
    userid INT PRIMARY KEY AUTO_INCREMENT,
    tel CHAR(11) NOT NULL UNIQUE,
    username VARCHAR(20) NOT NULL,
    password VARCHAR(32) NOT NULL,
    status TINYINT DEFAULT 1,
    create_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 票务表
CREATE TABLE IF NOT EXISTS ticket_info (
    tk_id INT PRIMARY KEY AUTO_INCREMENT,
    addr VARCHAR(50) NOT NULL,
    max INT NOT NULL,
    num INT DEFAULT 0,
    use_date DATE NOT NULL
);

-- 预约表
CREATE TABLE IF NOT EXISTS sub_ticket (
    yi_id INT PRIMARY KEY AUTO_INCREMENT,
    tk_id INT NOT NULL,
    tel CHAR(11) NOT NULL,
    curr_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (tk_id) REFERENCES ticket_info(tk_id),
    FOREIGN KEY (tel) REFERENCES user_info(tel)
);
