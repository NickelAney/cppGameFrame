
#include "WriterThread.h"

#include <game/dbsrv/config/ConfigMgr.h>

WriterThread::WriterThread(int id):
	id_(id),
	thread_(std::tr1::bind(&WriterThread::threadHandler, this), "WriterThread")
{

}

WriterThread::~WriterThread()
{

}

void WriterThread::start()
{
	// mysql
	std::string host = sConfigMgr.MainConfig.GetStringDefault("mysql", "host", "192.168.1.6");
	std::string port_or_socket = sConfigMgr.MainConfig.GetStringDefault("mysql", "port_or_socket", "3306");
	std::string user = sConfigMgr.MainConfig.GetStringDefault("mysql", "user", "root");
	std::string password = sConfigMgr.MainConfig.GetStringDefault("mysql", "password", "4399mysql#CQPZM");
	std::string database = sConfigMgr.MainConfig.GetStringDefault("mysql", "database", "newkabu");
	MySQLConnectionInfo coninfo(host, port_or_socket, user, password, database);
	mysql_.setConnectionInfo(coninfo);

	mysql_.open();
	thread_.start();
}

void WriterThread::stop()
{
	WriterThreadParam param;
	param.Type = WRITERTHREAD_STOP;
	queue_.put(param);
	thread_.join();

	mysql_.close();
}

void WriterThread::push(struct WriterThreadParam& param)
{
	queue_.put(param);
}

static int saveSqlNum = 0;
void WriterThread::threadHandler()
{
	LOG_INFO << "WriterThread::threadHandler start... id[" << id_ << "]";
	while (true)
	{
		WriterThreadParam param = queue_.take();
		if (param.Type == WRITERTHREAD_CMD)
		{
			handler(param);
			// 每保存100条sql 就sleep 1 s 一下吧， 防止mysql io拥堵
			if (saveSqlNum++ >= 100)
			{
				saveSqlNum = 0;
				sleep(1);
			}
		}
		else if (param.Type == WRITERTHREAD_PING)
		{
			mysql_.ping();
		}
		else if (param.Type == WRITERTHREAD_STOP)
		{
			break;
		}
	}
	LOG_INFO << "WriterThread::threadHandler stop... id[" << id_ << "]";
}

void WriterThread::handler(struct WriterThreadParam& param)
{
	char* sql = static_cast<char*>(param.sql);
	LOG_DEBUG << "WriterThread::handler, sql: " << sql;
	if (!mysql_.execute(sql, param.length))
	{
		// 保存到数据库失败~~~
		// 怎么处理呢？？？
		LOG_WARN << "[SQL]" << param.sql;
	}
	// 释放掉这个内存空间啦
	delete[] sql;
}