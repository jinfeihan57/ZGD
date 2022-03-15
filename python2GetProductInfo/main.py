'''
  启动主进程
'''
import sys

from views import app
from models import db
# command = sys.argv[1]

# if command == "migrate":
#     db.create_all()
#db.create_all()
# if command == "runserver":
#     app.run(host="127.0.0.1", port=8000, debug=True)
app.run(host="0.0.0.0", port=5215, debug=True)