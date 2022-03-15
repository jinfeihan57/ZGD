'''
 参数配置文件 数据库 flask ...
'''

import os

BASE_DIR = os.path.abspath(os.path.dirname(__file__))

# 数据库设置
DIALCT = "mysql+pymysql"

# Ubuntu 16 数据库
# USERNAME = "pi"
# PASSWORD = "raspberry"
# HOST = "192.168.1.107"

# raspberry pi 数据库
USERNAME = "root"
PASSWORD = "raspberry"
HOST = "192.168.1.105"

# win_server 数据库
# USERNAME = "root"
# PASSWORD = "Passion57"
# HOST = "127.0.0.1"

PORT = "3306"
DATABASE = "zgddb"
DB_URI = "{}://{}:{}@{}:{}/{}?charset=utf8".format(DIALCT,USERNAME,PASSWORD,HOST,PORT,DATABASE)

# 使用对象 设置 flask 
class Config:
  SQLALCHEMY_DATABASE_URI = DB_URI
  SQLALCHEMY_COMMIT_ON_TEARDOWN = True
  SQLALCHEMY_TRACK_MODIFICATIONS = True
  DEBUG = True
