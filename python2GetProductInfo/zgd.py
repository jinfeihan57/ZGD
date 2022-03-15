'''
  
'''

from flask import Flask, redirect, url_for, request

from flask_sqlalchemy import SQLAlchemy

import json

from settings import Config

app = Flask(__name__)

app.config.from_object("settings.Config")

db = SQLAlchemy(app)
